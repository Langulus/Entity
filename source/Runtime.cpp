///                                                                           
/// Langulus::Entity                                                          
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#include "Thing.hpp"
#include "Runtime.hpp"
#include "External.hpp"
#include <Anyness/Path.hpp>

#if LANGULUS_OS(WINDOWS)
   #include <Windows.h>
#endif

#if LANGULUS_OS(LINUX)
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

#define VERBOSE(...) //Logger::Verbose(this, ": ", ...)

namespace Langulus::Entity
{

   TUnorderedMap<Token, Runtime::SharedLibrary> Runtime::mLibraries;
   TUnorderedMap<const RTTI::Meta*, Runtime::SharedLibrary> Runtime::mDependencies;

   /// Close a shared library handle, unloading it                            
   ///   @param library - the library handle                                  
   #if LANGULUS_OS(WINDOWS)
      void UnloadSharedLibrary(HMODULE library) {
         FreeLibrary(library);
      }
   #elif LANGULUS_OS(LINUX)
      void UnloadSharedLibrary(void* library) {
         dlclose(library);
      }
   #else
      #error Unsupported OS
   #endif

   /// Runtime construction                                                   
   ///   @param owner - the owner of the runtime                              
   Runtime::Runtime(Thing* owner) noexcept
      : mOwner {owner} {
      VERBOSE("Initializing...");
      VERBOSE("Initialized");
   }

   /// Runtime destruction                                                    
   Runtime::~Runtime() {
      VERBOSE("Shutting down...");
      for (auto library = mLibraries.begin(); library != mLibraries.end(); ++library) {
         if (0 == --library->mValue.mReferences) {
            UnloadSharedLibrary(library->mValue);
            library = mLibraries.RemoveIndex(library);
         }
      }
   }

   /// Create a module instance or return an already instantiated one         
   ///   @param name - module name                                            
   ///   @param descriptor - module initialization descriptor                 
   ///   @return the new module instance                                      
   Module* Runtime::InstantiateModule(const Token& name, const Descriptor& descriptor) {
      // Load the library if not loaded yet                             
      const auto library = LoadSharedLibrary(name);

      // Check if module is already instantiated                        
      auto& foundModules = GetModules(library.mModuleType);
      if (!foundModules.IsEmpty())
         return foundModules[0];

      // A module instance doesn't exist yet, so instantiate it         
      VERBOSE("Module `", name, "` is not instantiated yet"
         ", so attempting to create it...");
      return InstantiateModule(library, descriptor);
   }
   
   /// Register by all bases in mModulesByType                                
   ///   @param map - [in/out] the map to fill                                
   ///   @param module - the module instance to push                          
   ///   @param type - the type to unregister the module as                   
   void RegisterAllBases(TUnorderedMap<DMeta, ModuleList>& map, Module* module, DMeta type) {
      VERBOSE("Registering `", type, '`');
      map[type] << module;
      for (auto& base : type->mBases) {
         if (base.mType->template Is<Resolvable>())
            break;
         RegisterAllBases(map, module, base.mType);
      }
   }

   /// Unregister by all bases in mModulesByType (in reverse order)           
   ///   @param map - [in/out] the map to unregister from                     
   ///   @param module - the module instance to push                          
   ///   @param type - the type to register the module as                     
   void UnregisterAllBases(TUnorderedMap<DMeta, ModuleList>& map, Module* module, DMeta type) {
      for (auto& base : type->mBases) {
         if (base.mType->template Is<Resolvable>())
            break;
         UnregisterAllBases(map, module, base.mType);
      }

      const auto found = map.FindKeyIndex(type);
      if (found) {
         auto& list = map.GetValue(found);
         if (list.Remove(module) && list.IsEmpty()) {
            VERBOSE("Unregistering `", type, '`');
            map.RemoveIndex(found);
         }
      }
   }

   /// Create a module instance or return an already instantiated one         
   ///   @param library - the library handle                                  
   ///   @param descriptor - module initialization descriptor                 
   ///   @return the new module instance                                      
   Module* Runtime::InstantiateModule(const SharedLibrary& library, const Descriptor& descriptor) {
      if (!library.IsValid())
         return nullptr;

      // Use the creation point of the library to instantiate module    
      const auto info = library.mInfo();
      auto module = library.mCreator(this, descriptor);
      if (!module) {
         Logger::Error("Module `", info->mName, "` creator didn't provide a module");
         return nullptr;
      }

      // Register the module in the various maps, for fast retrieval    
      try {
         mModules[info->mPriority] << module;
         RegisterAllBases(mModulesByType, module, module->GetType());
      }
      catch (...) {
         Logger::Error("Registering module `", info->mName, "` failed");

         // Make sure we end up in an invariant state                   
         mModules[info->mPriority].Remove(module);
         if (mModules[info->mPriority].IsEmpty())
            mModules.RemoveKey(info->mPriority);

         UnregisterAllBases(mModulesByType, module, module->GetType());
         delete module;
         return nullptr;
      }

      // Done, if reached                                               
      VERBOSE("Module `", info->mName,
         "` registered with priority ", info->mPriority);
      return module;
   }

   /// Load a shared library for a module                                     
   ///   @param name - the name for the dynamic library                       
   ///                 the filename will be derived from it, by prefixing     
   ///                 with `Mod.`, and suffixing with `.so` or `.dll`        
   ///   @return the module handle (OS dependent)                             
   Runtime::SharedLibrary Runtime::LoadSharedLibrary(const Token& name) {
      // Check if this library is already loaded                        
      const auto preloaded = mLibraries.FindKeyIndex(name);
      if (preloaded) {
         // Never even attempt to load libraries more than once         
         return mLibraries.GetValue(preloaded);
      }

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
      #elif LANGULUS_OS(LINUX)
         path += ".so";
      #else
         #error Unsupported OS
      #endif

      // Make sure string ends with terminator                          
      path = path.Terminate();

      // Load the library                                               
      #if LANGULUS_OS(WINDOWS)
         const auto dll = LoadLibraryA(path.GetRaw());
      #elif LANGULUS_OS(LINUX)
         const auto dll = dlopen(path.GetRaw(), RTLD_NOW);
      #else 
         #error Unsupported OS
      #endif

      if (!dll) {
         Logger::Error("Failed to load module `", path, "` - file is missing or corrupted; Error code: ");
         #if LANGULUS_OS(WINDOWS)
            Logger::Append(::GetLastError());
         #endif
         return {};
      }

      SharedLibrary library;
      static_assert(sizeof(library.mHandle) == sizeof(dll), "Size mismatch");
      library.mHandle = reinterpret_cast<decltype(library.mHandle)>(dll);

      // Get entry, creator, info and exit points from the library      
      #if LANGULUS_OS(WINDOWS)
         library.mEntry = reinterpret_cast<Module::EntryFunction>(
            GetProcAddress(dll, LANGULUS_MODULE_ENTRY_TOKEN()));
         library.mCreator = reinterpret_cast<Module::CreateFunction>(
            GetProcAddress(dll, LANGULUS_MODULE_CREATE_TOKEN()));
         library.mInfo = reinterpret_cast<Module::InfoFunction>(
            GetProcAddress(dll, LANGULUS_MODULE_INFO_TOKEN()));
      #elif LANGULUS_OS(LINUX)
         library.mEntry = reinterpret_cast<Module::EntryFunction>(
            dlsym(dll, LANGULUS_MODULE_ENTRY_TOKEN()));
         library.mCreator = reinterpret_cast<Module::CreateFunction>(
            dlsym(dll, LANGULUS_MODULE_CREATE_TOKEN()));
         library.mInfo = reinterpret_cast<Module::InfoFunction>(
            dlsym(dll, LANGULUS_MODULE_INFO_TOKEN()));
      #else 
         #error Unsupported OS
      #endif   

      if (!library.mEntry) {
         Logger::Error("Module `", path, "` has no valid entry point - ",
            "the function " LANGULUS_MODULE_ENTRY_TOKEN() " is missing");
         UnloadSharedLibrary(library);
         return {};
      }

      if (!library.mCreator) {
         Logger::Error("Module `", path, "` has no valid instantiation point - ",
            "the function " LANGULUS_MODULE_CREATE_TOKEN() " is missing");
         UnloadSharedLibrary(library);
         return {};
      }

      if (!library.mInfo) {
         Logger::Error("Module `", path, "` has no valid information point - ",
            "the function " LANGULUS_MODULE_INFO_TOKEN() " is missing");
         UnloadSharedLibrary(library);
         return {};
      }

      // Link the module - this shall merge RTTI definitions            
      // It might throw if out of memory or on meta collision, while    
      // registering new types on the other side                        
      try {
         // Make sure that const RTTI::Meta* type is registered here,   
         // and not inside the library (nasty bugs otherwise)           
         //TODO find a more elegant solution to this, preferably one that doesn't involve prebuild steps with cppast
         (void)MetaOf<Runtime>();
         (void)MetaOf<DMeta>();
         (void)MetaOf<Module>();
         (void)MetaOf<Unit>();

         library.mEntry(library.mModuleType, library.mTypes);

         mLibraries.Insert(name, library);

         for (auto externalType : library.mTypes)
            mDependencies.Insert(externalType, library);

         Logger::Info("Module `", library.mInfo()->mName, 
            "` exposed the following types: ", library.mTypes);
      }
      catch (...) {
         // Make sure we end up in an invariant state                   
         Logger::Error("Could not enter `", path, "` due to an exception");
         UnloadSharedLibrary(library);
         mLibraries.RemoveKey(name);
         return {};
      }

      // Great success!                                                 
      Logger::Info("Module `", library.mInfo()->mName, "` loaded (", path, ')');
      return library;
   }

   /// Unload a DLL/SO extension module                                       
   ///   @param library - the library handle to unload                        
   void Runtime::UnloadSharedLibrary(const SharedLibrary& library) {
      if (library.mHandle == 0)
         return;

      Logger::Info("Unloading module `", library.mInfo()->mName, "`...");

      for (auto list = mModules.begin(); list != mModules.end(); ++list) {
         for (auto mod = list->mValue.begin(); mod != list->mValue.end(); ++mod) {
            if (mod->Is(library.mModuleType)) {
               // Delete module instance                                
               const auto modType = mod->GetType();
               UnregisterAllBases(mModulesByType, *mod, modType);
               delete *mod;
               mod = list->mValue.RemoveIndex(mod);
            }
         }

         if (list->mValue.IsEmpty())
            list = mModules.RemoveIndex(list);
      }

      // Remove dependencies                                            
      for (auto externalType : library.mTypes)
         mDependencies.RemoveKey(externalType);

      // Collect garbage, and check if library's boundary is still used 
      const auto wasMarked = library.mMarkedForUnload;
      const auto boundary = library.mInfo()->mName;

      Anyness::Fractalloc.CollectGarbage();
      const auto poolsInUse = Anyness::Fractalloc.CheckBoundary(boundary);
      if (poolsInUse) {
         // We can't allow the shared object to be unloaded!            
         // It will be attempted again on Update, so that dependent     
         // libraries have a chance of being unloaded first, releasing  
         // required resources.                                         
         if (!wasMarked) {
            Logger::Warning(
               "Module `", boundary, "` can't be unloaded yet, because "
               "exposed data is still in use in ", poolsInUse, " memory pools. "
               "Unload has been postponed to the next library unload."
            );
            const_cast<SharedLibrary&>(library).mMarkedForUnload = true;
            ++mMarkedForUnload;
         }

         return;

         /*#if LANGULUS_FEATURE(MEMORY_STATISTICS)
            Anyness::Fractalloc.DumpPools();
         #endif
         LANGULUS_THROW(Destruct, "Can't unload shared library");*/
      }

      // Unregister external types, if no longer used                   
      RTTI::Database.UnloadLibrary(boundary);
      Logger::Info("Module `", boundary, "` unloaded");

      // Unload the shared object                                       
      #if LANGULUS_OS(WINDOWS)
         Entity::UnloadSharedLibrary(
            reinterpret_cast<HMODULE>(library.mHandle));
      #elif LANGULUS_OS(LINUX)
         Entity::UnloadSharedLibrary(
            reinterpret_cast<void*>(library.mHandle));
      #else 
         #error Unsupported OS
      #endif

      // Done, account for it, if it was marked for deletion            
      if (wasMarked)
         --mMarkedForUnload;
   }

   /// Get the dependency module of a given type                              
   ///   @param type - the type to search for                                 
   ///   @return the shared library handle, you should check if it's valid    
   Runtime::SharedLibrary Runtime::GetDependency(DMeta type) const noexcept {
      auto found = mDependencies.FindKeyIndex(type);
      if (found)
         return mDependencies.GetValue(found);
      return {};
   }

   /// Get a module instance by type                                          
   ///   @param type - the type to search for                                 
   ///   @return the module instance                                          
   const ModuleList& Runtime::GetModules(DMeta type) const noexcept {
      auto found = mModulesByType.FindKeyIndex(type);
      if (found)
         return mModulesByType.GetValue(found);

      static const ModuleList emptyFallback {};
      return emptyFallback;
   }

#if LANGULUS_FEATURE(MANAGED_REFLECTION)
   /// Get the dependency module of a given type by token                     
   ///   @param token - type token                                            
   ///   @return the shared library handle, you should check if it's valid    
   Runtime::SharedLibrary Runtime::GetDependency(const Token& token) const noexcept {
      return GetDependency(RTTI::Database.GetMetaData(token));
   }

   /// Get a module instance by type token                                    
   ///   @param token - type token                                            
   ///   @return the module instance, or nullptr if not found                 
   const ModuleList& Runtime::GetModules(const Token& token) const noexcept {
      return GetModules(RTTI::Database.GetMetaData(token));
   }
#endif

   /// Get a file interface, relying on external modules to find it           
   ///   @param path - the path for the file                                  
   ///   @return the file interface, or nullptr if file doesn't exist         
   A::File* Runtime::GetFile(const Path& path) {
      const auto& fileSystems = GetModules(MetaData::Of<A::FileSystem>());
      for (auto module : fileSystems) {
         const auto fs = dynamic_cast<A::FileSystem*>(module);
         if (!fs)
            continue;

         const auto file = fs->GetFile(path);
         if (file)
            return const_cast<A::File*>(file);
      }

      return nullptr;
   }

   /// Update the runtime, by updating all module instantiations by order of  
   /// their priority                                                         
   ///   @param dt - delta time between update calls                          
   void Runtime::Update(Time dt) {
      for (auto pair : mModules) {
         for (auto module : pair.mValue)
            module->Update(dt);
      }
   }

   /// Stringify the runtime, for debugging purposes                          
   Runtime::operator Debug() const {
      return IdentityOf(this);
   }

} // namespace Langulus::Entity
