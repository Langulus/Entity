///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Thing.hpp"
#include "Runtime.hpp"
#include "../include/Langulus/Asset.hpp"
#include "../include/Langulus/Graphics.hpp"
#include "../include/Langulus/Image.hpp"
#include "../include/Langulus/IO.hpp"
#include "../include/Langulus/Material.hpp"
#include "../include/Langulus/Mesh.hpp"
#include "../include/Langulus/Physical.hpp"
#include "../include/Langulus/Platform.hpp"
#include "../include/Langulus/UI.hpp"
#include "../include/Langulus/AI.hpp"
#include "../include/Langulus/Input.hpp"
#include "../include/Langulus/Life.hpp"
#include "../include/Langulus/Network.hpp"
#include "../include/Langulus/User.hpp"

#if LANGULUS_OS(WINDOWS)
   #include <Windows.h>
#endif

#if LANGULUS_OS(LINUX) or LANGULUS_COMPILER(WASM)
   #include <dlfcn.h>
#endif

#if LANGULUS_OS(ANDROID)
   //TODO
#endif

#if LANGULUS_OS(MACOS)
   //TODO
#endif

#if LANGULUS_OS(UNIX)
   //TODO
#endif

#if LANGULUS_OS(FREEBSD)
   //TODO
#endif

#if 0
   #define VERBOSE(...) Logger::Verbose(__VA_ARGS__)
#else
   #define VERBOSE(...) LANGULUS(NOOP)
#endif


namespace Langulus::Entity
{

   TUnorderedMap<Token, Runtime::SharedLibrary> Runtime::mLibraries;

   /// Close a shared library handle, unloading it                            
   ///   @param library - the library handle                                  
   #if LANGULUS_OS(WINDOWS)
      void UnloadSharedLibrary(HMODULE library) {
         FreeLibrary(library);
      }
   #else
      void UnloadSharedLibrary(void* library) {
         dlclose(library);
      }
   #endif

   /// Runtime construction                                                   
   ///   @param owner - the owner of the runtime                              
   Runtime::Runtime(Thing* owner) noexcept
      : mOwner {owner} {
      VERBOSE(this, ": Initializing...");

      // Reflect fundamentals                                           
      (void)MetaDataOf<A::Number>();
      (void)MetaDataOf<A::Integer>();
      (void)MetaDataOf<A::Signed>();
      (void)MetaDataOf<A::Unsigned>();
      (void)MetaDataOf<A::UnsignedInteger>();
      (void)MetaDataOf<A::Real>();
      (void)MetaDataOf<A::SignedInteger>();
      (void)MetaDataOf<A::Char>();
      (void)MetaDataOf<A::Bool>();

      Math::RegisterTraits();
      Math::RegisterVerbs();

      // We always prefer 'type' definitions that are in the main       
      // boundary, to avoid segfaults when unloading libraries from     
      // mModulesByType - it is indexed by a DMeta                      
      // So we make sure that the base modules are defined here         
      (void)MetaDataOf<A::PlatformModule>();
      (void)MetaDataOf<A::Platform>();
      (void)MetaDataOf<A::Window>();
      (void)MetaDataOf<A::Cursor>();

      (void)MetaDataOf<A::PhysicalModule>();
      (void)MetaDataOf<A::Physical>();
      (void)MetaDataOf<A::World>();
      (void)MetaDataOf<A::Instance>();

      (void)MetaDataOf<A::UIModule>();
      (void)MetaDataOf<A::UIUnit>();
      (void)MetaDataOf<A::UISystem>();
      (void)MetaDataOf<A::UIButton>();
      (void)MetaDataOf<A::UILabel>();
      (void)MetaDataOf<A::UIInput>();

      (void)MetaDataOf<A::FileSystem>();
      (void)MetaDataOf<A::File>();
      (void)MetaDataOf<A::Folder>();

      (void)MetaDataOf<A::GraphicsModule>();
      (void)MetaDataOf<A::Graphics>();
      (void)MetaDataOf<A::Renderer>();
      (void)MetaDataOf<A::Layer>();
      (void)MetaDataOf<A::Camera>();
      (void)MetaDataOf<A::Renderable>();
      (void)MetaDataOf<A::Light>();

      (void)MetaDataOf<A::AssetModule>();
      (void)MetaDataOf<A::Asset>();
      (void)MetaDataOf<A::Image>();
      (void)MetaDataOf<A::Font>();
      (void)MetaDataOf<A::Material>();
      (void)MetaDataOf<A::Mesh>();

      (void)MetaDataOf<A::AIModule>();
      (void)MetaDataOf<A::AIUnit>();
      (void)MetaDataOf<A::Mind>();

      (void)MetaDataOf<A::InputModule>();
      (void)MetaDataOf<A::Input>();
      (void)MetaDataOf<A::InputGatherer>();
      (void)MetaDataOf<A::InputListener>();

      (void)MetaDataOf<A::Life>();
      (void)MetaDataOf<A::LifeUnit>();
      (void)MetaDataOf<A::Ecosystem>();
      (void)MetaDataOf<A::Organism>();

      (void)MetaDataOf<A::Network>();
      (void)MetaDataOf<A::NetworkUnit>();
      (void)MetaDataOf<A::Client>();
      (void)MetaDataOf<A::Server>();
      (void)MetaDataOf<A::Shared>();

      (void)MetaDataOf<A::UserModule>();
      (void)MetaDataOf<A::User>();

      VERBOSE(this, ": Initialized");
   }

   /// Runtime destruction                                                    
   Runtime::~Runtime() {
      VERBOSE(this, ": Shutting down...");

      // First-stage destruction: tear down any potential circular      
      // references                                                     
      for (auto list : mModules) {
         for (auto& mod : list.mValue)
            mod->Teardown();
      }

      // Second-stage destruction: cycle through N libraries N^2        
      // times, unloading anything that is no longer in use each time   
      auto attempts = mLibraries.GetCount();
      while (attempts) {
         for (auto library : KeepIterator(mLibraries)) {
            if (UnloadSharedLibrary(library.GetValue()))
               library = mLibraries.RemoveIt(library);
         }

         --attempts;
      }

      // If after all attempts there's still active libraries, then     
      // something is definitely not right. This will always result in  
      // a crash, since we can't really throw inside a destructor.      
      if (mLibraries) {
         Logger::Error(this, ": Can't unload last module(s): ");
         for (auto library : mLibraries)
            Logger::Append(library.mKey, " ");

         Logger::Error(this, ": This likely involves a memory leak "
            "that withholds managed data reflected by the given modules");

         IF_LANGULUS_MANAGED_MEMORY(Allocator::CollectGarbage());
         IF_LANGULUS_MEMORY_STATISTICS(Allocator::DumpPools());
         LANGULUS_OOPS(Destruct, "Can't unload last module(s)");
      }
   }

   /// Create a module instance or return an already instantiated one         
   ///   @param name - module name                                            
   ///   @param descriptor - module initialization descriptor                 
   ///   @return the new module instance                                      
   auto Runtime::InstantiateModule(const Token& name, const Many& descriptor) -> A::Module* {
      // Load the library if not loaded yet                             
      const auto library = LoadSharedLibrary(name);

      // Check if module is already instantiated                        
      auto& foundModules = GetModules(library.mModuleType);
      if (foundModules)
         return foundModules[0];

      // A module instance doesn't exist yet, so instantiate it         
      VERBOSE(this, ": Module `", name, "` is not instantiated yet"
         ", so attempting to create it...");
      const auto instance = InstantiateModule(library, descriptor);
      if (not instance)
         (void) UnloadSharedLibrary(library);
      return instance;
   }
   
   /// Register by all bases in mModulesByType                                
   ///   @param map - [in/out] the map to fill                                
   ///   @param module - the module instance to push                          
   ///   @param type - the type to unregister the module as                   
   void RegisterAllBases(TUnorderedMap<DMeta, ModuleList>& map, A::Module* module, DMeta type) {
      VERBOSE("Registering `", type, '`');
      auto found = map.FindIt(type);
      if (found)
         found.GetValue() << module;
      else {
         // We always prefer 'type' definition that is in the main      
         // boundary, to avoid segfaults when unloading libraries from  
         // mModulesByType - it is indexed by a DMeta                   
         auto localDefinition = RTTI::GetMetaData(type->mToken, RTTI::MainBoundary);
         if (localDefinition)
            map.Insert(localDefinition, module);
         else
            map.Insert(type, module);
      }

      for (auto& base : type->mBases) {
         if (base.mType->IsExact<Resolvable>())
            break;
         RegisterAllBases(map, module, base.mType);
      }
   }

   /// Unregister by all bases in mModulesByType (in reverse order)           
   ///   @param map - [in/out] the map to unregister from                     
   ///   @param module - the module instance to push                          
   ///   @param type - the type to register the module as                     
   void UnregisterAllBases(TUnorderedMap<DMeta, ModuleList>& map, A::Module* module, DMeta type) {
      for (auto& base : type->mBases) {
         if (base.mType->IsExact<Resolvable>())
            break;
         UnregisterAllBases(map, module, base.mType);
      }

      const auto found = map.FindIt(type);
      if (found) {
         auto& list = found.GetValue();
         if (list.Remove(module) and not list) {
            VERBOSE("Unregistering `", type, '`');
            map.RemoveIt(found);
            if (not map)
               map.Reset();
         }
      }
   }

   /// Create a module instance or return an already instantiated one         
   ///   @param library - the library handle                                  
   ///   @param descriptor - module initialization descriptor                 
   ///   @return the new module instance                                      
   auto Runtime::InstantiateModule(const SharedLibrary& library, const Many& descriptor) -> A::Module* {
      if (not library.IsValid())
         return nullptr;

      // Use the creation point of the library to instantiate module    
      const auto info = library.mInfo();
      A::Module* module {};

      try { module = library.mCreator(this, descriptor); }
      catch (...) {
         Logger::Error("Module `", info->mName,
            "` creator has thrown an exception");
         return nullptr;
      }

      if (not module) {
         Logger::Error("Module `", info->mName,
            "` creator didn't provide a module");
         return nullptr;
      }

      // Register the module in the various maps, for fast retrieval    
      try {
         auto found = mModules.FindIt(info->mPriority);
         if (found)
            found.GetValue() << module;
         else
            mModules.Insert(info->mPriority, ModuleList {module});

         RegisterAllBases(mModulesByType, module, module->GetType());
      }
      catch (...) {
         Logger::Error("Registering module `", info->mName, "` failed");

         // Make sure we end up in an invariant state                   
         auto found = mModules.FindIt(info->mPriority);
         if (found) {
            found.GetValue().Remove(module);
            if (not found.GetValue())
               mModules.RemoveIt(found);
         }

         if (not mModules)
            mModules.Reset();

         UnregisterAllBases(mModulesByType, module, module->GetType());
         delete module;
         return nullptr;
      }

      // Done, if reached                                               
      VERBOSE(this, ": Module `", info->mName,
         "` registered with priority ", info->mPriority);
      return module;
   }

   /// Load a shared library for a module                                     
   ///   @param name - the name for the dynamic library - the filename will   
   ///      be derived from it, by prefixing with `LangulusMod`, and          
   ///      suffixing with `.so` or `.dll` the name also should correspond to 
   ///      the RTTI::Boundary                                                
   ///   @return the module handle (OS dependent)                             
   LANGULUS(NOINLINE)
   auto Runtime::LoadSharedLibrary(const Token& name) -> SharedLibrary {
      // Check if this library is already loaded                        
      const auto preloaded = mLibraries.FindIt(name);
      if (preloaded)
         return preloaded.GetValue();

      // File prefix                                                    
      Path path;
      #if LANGULUS_OS(LINUX)
         path += "./lib";
      #endif
      path += "LangulusMod";
      path += name;

      // File postfix                                                   
      #if LANGULUS_OS(WINDOWS)
         path += ".dll";
      #else
         path += ".so";
      #endif

      // Make sure string ends with terminator                          
      path = path.Terminate();

      // Load the library                                               
      #if LANGULUS_OS(WINDOWS)
         const auto dll = LoadLibraryA(path.GetRaw());
      #else
         const auto dll = dlopen(path.GetRaw(), RTLD_NOW);
      #endif

      if (not dll) {
         Logger::Error("Failed to load module `", path, "` - "
            "file is missing or corrupted; Error code: ");
         #if LANGULUS_OS(WINDOWS)
            Logger::Append(::GetLastError());
         #else
            Logger::Append(dlerror());
         #endif
         return {};
      }

      SharedLibrary library;
      static_assert(sizeof(library.mHandle) == sizeof(dll), "Size mismatch");
      library.mHandle = reinterpret_cast<decltype(library.mHandle)>(dll);

      // Get entry, creator, info and exit points from the library      
      #if LANGULUS_OS(WINDOWS)
         library.mEntry = reinterpret_cast<A::Module::EntryFunction>(
            GetProcAddress(dll, LANGULUS_MODULE_ENTRY_TOKEN()));
         library.mCreator = reinterpret_cast<A::Module::CreateFunction>(
            GetProcAddress(dll, LANGULUS_MODULE_CREATE_TOKEN()));
         library.mInfo = reinterpret_cast<A::Module::InfoFunction>(
            GetProcAddress(dll, LANGULUS_MODULE_INFO_TOKEN()));
      #else
         library.mEntry = reinterpret_cast<A::Module::EntryFunction>(
            dlsym(dll, LANGULUS_MODULE_ENTRY_TOKEN()));
         library.mCreator = reinterpret_cast<A::Module::CreateFunction>(
            dlsym(dll, LANGULUS_MODULE_CREATE_TOKEN()));
         library.mInfo = reinterpret_cast<A::Module::InfoFunction>(
            dlsym(dll, LANGULUS_MODULE_INFO_TOKEN()));
      #endif   

      if (not library.mEntry) {
         Logger::Error("Module `", path, "` has no valid entry point - ",
            "the function " LANGULUS_MODULE_ENTRY_TOKEN() " is missing");
         (void) UnloadSharedLibrary(library);
         return {};
      }

      if (not library.mCreator) {
         Logger::Error("Module `", path, "` has no valid instantiation point - ",
            "the function " LANGULUS_MODULE_CREATE_TOKEN() " is missing");
         (void) UnloadSharedLibrary(library);
         return {};
      }

      if (not library.mInfo) {
         Logger::Error("Module `", path, "` has no valid information point - ",
            "the function " LANGULUS_MODULE_INFO_TOKEN() " is missing");
         (void) UnloadSharedLibrary(library);
         return {};
      }

      // Link the module - this shall merge RTTI definitions            
      // It might throw if out of memory or on meta collision, while    
      // registering new types on the other side                        
      try {
         // Invoke the entry point, it should reflect external data     
         MetaList types;
         library.mEntry(library.mModuleType, types);
         if (types.IsEmpty()) {
            Logger::Error("A module must register at least one type"
               " - the module instantiation type");
            (void)UnloadSharedLibrary(library);
            return {};
         }

         // Make sure all registered types have the proper boundary     
         bool firstType = true;
         for (auto externalType : types) {
            if (firstType)
               library.mBoundary = externalType->mLibraryName;
            else if (library.mBoundary == RTTI::MainBoundary
                 or  library.mBoundary != externalType->mLibraryName) {
               Logger::Error(
                  "The external type `", externalType->mToken,
                  "` registered by module `", path,
                  "` has a mismatching boundary `", externalType->mLibraryName, '`');
               (void)UnloadSharedLibrary(library);
               return {};
            }
            firstType = false;
         }

         // Test if the boundary conflicts with any of the previously   
         // loaded libraries                                            
         for (auto lib : mLibraries) {
            if (lib.mValue.mBoundary == library.mBoundary) {
               Logger::Error(
                  "The library `", path, "` boundary `", library.mBoundary,
                  "` conflicts with already loaded library `", lib.mKey,
                  "` boundary `", lib.mValue.mBoundary, '`'
               );
               (void)UnloadSharedLibrary(library);
               return {};
            }
         }

         // Library is properly registered                              
         mLibraries.Insert(name, library);

         // Do some info logging                                        
         Logger::Info("Module `", library.mInfo()->mName, 
            "` exposed the following types: ", Logger::DarkGreen);
         bool first = true;
         for (auto& t : types) {
            if (not first) Logger::Append(", ");
            Logger::Append(t->mToken);
            first = false;
         }
      }
      catch (...) {
         // Make sure we end up in an invariant state                   
         Logger::Error("Could not enter `", path, "` due to an exception");
         if (UnloadSharedLibrary(library))
            mLibraries.RemoveKey(name);
         if (not mLibraries)
            mLibraries.Reset();
         return {};
      }

      // Great success!                                                 
      Logger::Info("Module `", library.mInfo()->mName, "` loaded (", path, ')');
      return library;
   }

   /// Unload a DLL/SO extension module                                       
   ///   @param library - the library handle to unload                        
   ///   @return true if shared library was unloaded successfully             
   LANGULUS(NOINLINE)
   bool Runtime::UnloadSharedLibrary(const SharedLibrary& library) {
      if (library.mHandle == 0)
         return true;

      Logger::Info("Unloading module `", library.mInfo()->mName, "`...");

      for (auto list : KeepIterator(mModules)) {
         for (auto mod : KeepIterator(list.GetValue())) {
            if (mod->Is(library.mModuleType)) {
               // Delete module instance                                
               const auto modType = mod->GetType();
               UnregisterAllBases(mModulesByType, *mod, modType);
               delete *mod;
               mod = list.GetValue().RemoveIt(mod);
            }
         }

         if (not list.GetValue())
            list = mModules.RemoveIt(list);
      }

      // Make sure memory for the maps is released                      
      if (not mModulesByType)
         mModulesByType.Reset();
      if (not mModules)
         mModules.Reset();

      // Collect garbage, and check if library's boundary is still used 
      const auto wasMarked = library.mMarkedForUnload;
      const auto boundary = library.mBoundary;

      IF_LANGULUS_MANAGED_MEMORY(Allocator::CollectGarbage());

      #if LANGULUS_FEATURE(MANAGED_REFLECTION) and LANGULUS_FEATURE(MANAGED_MEMORY)
         const auto poolsInUse = Allocator::CheckBoundary(boundary);
         if (poolsInUse) {
            // We can't allow the shared object to be unloaded!         
            // It will be attempted on next unload, so that dependent   
            // libraries have a chance of being unloaded first,         
            // releasing required resources.                            
            if (not wasMarked) {
               Logger::Warning(
                  "Module `", boundary, "` can't be unloaded yet, because "
                  "exposed data is still in use in ", poolsInUse, " memory pools. "
                  "Unload has been postponed to the next library unload.");
               const_cast<SharedLibrary&>(library).mMarkedForUnload = true;
            }

            return false;
         }
      #endif

      // If reached, then the library has no known allocations, using   
      // its reflected types - now we can safely unregister these types 
      IF_LANGULUS_MANAGED_REFLECTION(RTTI::UnloadBoundary(boundary));
      Logger::Info(
         "Module `", boundary, "` unloaded ",
         Logger::DarkYellow, (wasMarked ? "(scheduled)" : "")
      );

      // Unload the shared object                                       
      #if LANGULUS_OS(WINDOWS)
         Entity::UnloadSharedLibrary(
            reinterpret_cast<HMODULE>(library.mHandle));
      #else
         Entity::UnloadSharedLibrary(
            reinterpret_cast<void*>(library.mHandle));
      #endif
      return true;
   }

   /// Get the dependency module of a given type                              
   ///   @param type - the type to search for                                 
   ///   @return the shared library handle, you should check if it's valid    
   auto Runtime::GetDependency(DMeta type) const noexcept -> SharedLibrary {
      if (type->mLibraryName == RTTI::MainBoundary)
         return {};

      for (auto library : mLibraries) {
         if (library.mValue.mBoundary == type->mLibraryName)
            return library.mValue;
      }
      return {};
   }

   /// Get a module instance by type                                          
   ///   @param type - the type to search for                                 
   ///   @return the module instance                                          
   auto Runtime::GetModules(DMeta type) const noexcept -> const ModuleList& {
      auto found = mModulesByType.FindIt(type);
      if (found)
         return found.GetValue();

      static const ModuleList emptyFallback {};
      return emptyFallback;
   }

#if LANGULUS_FEATURE(MANAGED_REFLECTION)
   /// Get the dependency module of a given type by token                     
   ///   @param token - type token                                            
   ///   @return the shared library handle, you should check if it's valid    
   auto Runtime::GetDependencyToken(const Token& token) const noexcept -> SharedLibrary {
      return GetDependency(RTTI::GetMetaData(token));
   }

   /// Get a module instance by type token                                    
   ///   @param token - type token                                            
   ///   @return the module instance, or nullptr if not found                 
   auto Runtime::GetModulesToken(const Token& token) const noexcept -> const ModuleList& {
      return GetModules(RTTI::GetMetaData(token));
   }
#endif

   /// Update the runtime, by updating all module instantiations by order of  
   /// their priority                                                         
   ///   @param dt - delta time between update calls                          
   ///   @return true if no exit was requested by any of the modules          
   bool Runtime::Update(Time dt) {
      for (auto pair : mModules) {
         for (auto module : pair.mValue) {
            if (not module->Update(dt))
               return false;
         }
      }
      return true;
   }

   /// Stringify the runtime, for debugging purposes                          
   Runtime::operator Text() const {
      return IdentityOf(this);
   }
   
   /// Get a file interface, relying on external modules to find it           
   ///   @param path - the path for the file                                  
   ///   @return the file interface, or nullptr if file doesn't exist         
   auto Runtime::GetFile(const Path& path) -> Ref<A::File> {
      auto& fileSystems = GetModules<A::FileSystem>();
      LANGULUS_ASSERT(fileSystems, Module,
         "Can't retrieve file `", path, "` - no file system module available");
      return fileSystems.template As<A::FileSystem*>()->GetFile(path);
   }
   
   /// Get a folder interface, relying on external modules to find it         
   ///   @param path - the path for the folder                                
   ///   @return the folder interface, or nullptr if folder doesn't exist     
   auto Runtime::GetFolder(const Path& path) -> Ref<A::Folder> {
      auto& fileSystems = GetModules<A::FileSystem>();
      LANGULUS_ASSERT(fileSystems, Module,
         "Can't retrieve folder `", path, "` - no file system module available");
      return fileSystems.template As<A::FileSystem*>()->GetFolder(path);
   }

   /// Get the current working path (where the main exe was executed)         
   ///   @return the path                                                     
   auto Runtime::GetWorkingPath() const -> const Path& {
      auto& fileSystems = GetModules<A::FileSystem>();
      LANGULUS_ASSERT(fileSystems, Module,
         "Can't retrieve working path", " - no file system module available");
      return fileSystems.template As<A::FileSystem*>()->GetWorkingPath();
   }

   /// Get the current data path, like GetWorkingPath() / "data"              
   ///   @return the path                                                     
   auto Runtime::GetDataPath() const -> const Path& {
      auto& fileSystems = GetModules<A::FileSystem>();
      LANGULUS_ASSERT(fileSystems, Module,
         "Can't retrieve data path", " - no file system module available");
      return fileSystems.template As<A::FileSystem*>()->GetDataPath();
   }

} // namespace Langulus::Entity
