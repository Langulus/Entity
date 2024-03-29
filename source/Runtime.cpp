///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#include "Thing.hpp"
#include "Runtime.hpp"
#include "External.inl"
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

#define VERBOSE(...) //Logger::Verbose(__VA_ARGS__)


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
      VERBOSE(this, ": Initializing...");
      VERBOSE(this, ": Initialized");
   }

   /// Runtime destruction                                                    
   Runtime::~Runtime() {
      VERBOSE(this, ": Shutting down...");

      // Cycle through all libraries N^2 times, unloading anything      
      // that is no longer in use each time                             
      auto attempts = mLibraries.GetCount();
      while (attempts) {
         for (auto library : KeepIterator(mLibraries)) {
            if (UnloadSharedLibrary(library->mValue))
               library = mLibraries.RemoveIt(library);
         }

         --attempts;
      }

      // If after all attempts there's still active libraries, then     
      // something is definitely not right. This will always result in  
      // a crash, since we can't really throw inside a destructor       
      if (mLibraries) {
         Logger::Error(this, ": Can't unload last module(s): ");
         for (auto library : mLibraries)
            Logger::Append(library->mKey, " ");
         Logger::Error(this, ": This likely involves a memory leak, "
            "that withholds managed data, reflected by the given modules");

         IF_LANGULUS_MANAGED_MEMORY(Allocator::CollectGarbage());
         IF_LANGULUS_MEMORY_STATISTICS(Allocator::DumpPools());
         LANGULUS_THROW(Destruct, "Can't unload last module(s)");
      }
   }

   /// Create a module instance or return an already instantiated one         
   ///   @param name - module name                                            
   ///   @param descriptor - module initialization descriptor                 
   ///   @return the new module instance                                      
   Module* Runtime::InstantiateModule(const Token& name, const Neat& descriptor) {
      // Load the library if not loaded yet                             
      const auto library = LoadSharedLibrary(name);

      // Check if module is already instantiated                        
      auto& foundModules = GetModules(library.mModuleType);
      if (foundModules)
         return foundModules[0];

      // A module instance doesn't exist yet, so instantiate it         
      VERBOSE(this, ": Module `", name, "` is not instantiated yet"
         ", so attempting to create it...");
      return InstantiateModule(library, descriptor);
   }
   
   /// Register by all bases in mModulesByType                                
   ///   @param map - [in/out] the map to fill                                
   ///   @param module - the module instance to push                          
   ///   @param type - the type to unregister the module as                   
   void RegisterAllBases(TUnorderedMap<DMeta, ModuleList>& map, Module* module, DMeta type) {
      VERBOSE("Registering `", type, '`');
      auto found = map.FindIt(type);
      if (found)
         found->mValue << module;
      else
         map.Insert(type, module);

      for (auto& base : type->mBases) {
         if (base.GetType()->IsExact<Resolvable>())
            break;
         RegisterAllBases(map, module, base.GetType());
      }
   }

   /// Unregister by all bases in mModulesByType (in reverse order)           
   ///   @param map - [in/out] the map to unregister from                     
   ///   @param module - the module instance to push                          
   ///   @param type - the type to register the module as                     
   void UnregisterAllBases(TUnorderedMap<DMeta, ModuleList>& map, Module* module, DMeta type) {
      for (auto& base : type->mBases) {
         if (base.GetType()->IsExact<Resolvable>())
            break;
         UnregisterAllBases(map, module, base.GetType());
      }

      const auto found = map.FindIt(type);
      if (found) {
         auto& list = found->mValue;
         if (list.Remove(module) and not list) {
            VERBOSE("Unregistering `", type, '`');
            map.RemoveIt(found);
         }
      }
   }

   /// Create a module instance or return an already instantiated one         
   ///   @param library - the library handle                                  
   ///   @param descriptor - module initialization descriptor                 
   ///   @return the new module instance                                      
   Module* Runtime::InstantiateModule(const SharedLibrary& library, const Neat& descriptor) {
      if (not library.IsValid())
         return nullptr;

      // Use the creation point of the library to instantiate module    
      const auto info = library.mInfo();
      Module* module {};

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
            found->mValue << module;
         else
            mModules.Insert(info->mPriority, ModuleList {module});

         RegisterAllBases(mModulesByType, module, module->GetType());
      }
      catch (...) {
         Logger::Error("Registering module `", info->mName, "` failed");

         // Make sure we end up in an invariant state                   
         auto found = mModules.FindIt(info->mPriority);
         if (found) {
            found->mValue.Remove(module);
            if (not found->mValue)
               mModules.RemoveIt(found);
         }

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
   ///   @param name - the name for the dynamic library                       
   ///                 the filename will be derived from it, by prefixing     
   ///                 with `Mod.`, and suffixing with `.so` or `.dll`        
   ///   @return the module handle (OS dependent)                             
   Runtime::SharedLibrary Runtime::LoadSharedLibrary(const Token& name) {
      // Check if this library is already loaded                        
      const auto preloaded = mLibraries.FindIt(name);
      if (preloaded)
         return preloaded->mValue;

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

      if (not dll) {
         Logger::Error("Failed to load module `", path, "` - file is missing or corrupted; Error code: ");
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

      if (not library.mEntry) {
         Logger::Error("Module `", path, "` has no valid entry point - ",
            "the function " LANGULUS_MODULE_ENTRY_TOKEN() " is missing");
         (void)UnloadSharedLibrary(library);
         return {};
      }

      if (not library.mCreator) {
         Logger::Error("Module `", path, "` has no valid instantiation point - ",
            "the function " LANGULUS_MODULE_CREATE_TOKEN() " is missing");
         (void)UnloadSharedLibrary(library);
         return {};
      }

      if (not library.mInfo) {
         Logger::Error("Module `", path, "` has no valid information point - ",
            "the function " LANGULUS_MODULE_INFO_TOKEN() " is missing");
         (void)UnloadSharedLibrary(library);
         return {};
      }

      // Link the module - this shall merge RTTI definitions            
      // It might throw if out of memory or on meta collision, while    
      // registering new types on the other side                        
      try {
         // Make sure that all types used by the Runtime are reflected  
         // here, and not inside any library (nasty bugs otherwise)     
         (void)MetaOf<Neat>();
         (void)MetaOf<Any>();
         (void)MetaOf<TAny<Any>>();
         (void)MetaOf<Real>();
         (void)MetaOf<Runtime>();
         (void)MetaOf<RTTI::Meta*>();
         (void)MetaOf<const RTTI::Meta*>();
         (void)MetaOf<DMeta>();
         (void)MetaOf<Module>();
         (void)MetaOf<Unit>();
         (void)MetaOf<ModuleList>();
         (void)MetaOf<SharedLibrary>();

         library.mEntry(library.mModuleType, library.mTypes);

         mLibraries.Insert(name, library);

         for (auto externalType : library.mTypes)
            mDependencies.Insert(externalType, library);

         Logger::Info(
            "Module `", library.mInfo()->mName, 
            "` exposed the following types: ", Logger::DarkGreen
         );

         bool first = true;
         for (auto& t : library.mTypes) {
            if (not first)
               Logger::Append(", ");
            Logger::Append(t);
            first = false;
         }
      }
      catch (...) {
         // Make sure we end up in an invariant state                   
         Logger::Error("Could not enter `", path, "` due to an exception");
         if (UnloadSharedLibrary(library))
            mLibraries.RemoveKey(name);
         return {};
      }

      // Great success!                                                 
      Logger::Info("Module `", library.mInfo()->mName, "` loaded (", path, ')');
      return library;
   }

   /// Unload a DLL/SO extension module                                       
   ///   @param library - the library handle to unload                        
   ///   @return true if shared library was unloaded successfully             
   bool Runtime::UnloadSharedLibrary(const SharedLibrary& library) {
      if (library.mHandle == 0)
         return true;

      Logger::Info("Unloading module `", library.mInfo()->mName, "`...");

      for (auto list : KeepIterator(mModules)) {
         for (auto mod : KeepIterator(list->mValue)) {
            if (mod->Is(library.mModuleType)) {
               // Delete module instance                                
               const auto modType = mod->GetType();
               UnregisterAllBases(mModulesByType, *mod, modType);
               delete *mod;
               mod = list->mValue.RemoveIt(mod);
            }
         }

         if (not list->mValue)
            list = mModules.RemoveIt(list);
      }

      // Remove dependencies                                            
      for (auto externalType : library.mTypes) {
         VERBOSE(this, ": Removing dependencies: ", externalType);
         mDependencies.RemoveKey(externalType);
      }

      // Collect garbage, and check if library's boundary is still used 
      const auto wasMarked = library.mMarkedForUnload;
      const auto boundary = library.mInfo()->mName;

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
                  "Unload has been postponed to the next library unload."
               );
               const_cast<SharedLibrary&>(library).mMarkedForUnload = true;
            }

            return false;
         }
      #endif

      // Unregister external types, if no longer used                   
      IF_LANGULUS_MANAGED_REFLECTION(RTTI::UnloadLibrary(boundary));
      Logger::Info(
         "Module `", boundary, "` unloaded ",
         Logger::DarkYellow, (wasMarked ? "(scheduled)" : "")
      );

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
      return true;
   }

   /// Get the dependency module of a given type                              
   ///   @param type - the type to search for                                 
   ///   @return the shared library handle, you should check if it's valid    
   Runtime::SharedLibrary Runtime::GetDependency(DMeta type) const noexcept {
      auto found = mDependencies.FindIt(type);
      return found ? found->mValue : SharedLibrary {};
   }

   /// Get a module instance by type                                          
   ///   @param type - the type to search for                                 
   ///   @return the module instance                                          
   const ModuleList& Runtime::GetModules(DMeta type) const noexcept {
      auto found = mModulesByType.FindIt(type);
      if (found)
         return found->mValue;

      static const ModuleList emptyFallback {};
      return emptyFallback;
   }

#if LANGULUS_FEATURE(MANAGED_REFLECTION)
   /// Get the dependency module of a given type by token                     
   ///   @param token - type token                                            
   ///   @return the shared library handle, you should check if it's valid    
   Runtime::SharedLibrary Runtime::GetDependencyToken(const Token& token) const noexcept {
      return GetDependency(RTTI::GetMetaData(token));
   }

   /// Get a module instance by type token                                    
   ///   @param token - type token                                            
   ///   @return the module instance, or nullptr if not found                 
   const ModuleList& Runtime::GetModulesToken(const Token& token) const noexcept {
      return GetModules(RTTI::GetMetaData(token));
   }
#endif

   /// Get a file interface, relying on external modules to find it           
   ///   @param path - the path for the file                                  
   ///   @return the file interface, or nullptr if file doesn't exist         
   Ptr<A::File> Runtime::GetFile(const Path& path) {
      auto& fileSystems = GetModules<A::FileSystem>();
      LANGULUS_ASSERT(fileSystems, Module,
         "Can't retrieve file `", path, "` - no file system module available");
      return fileSystems.template As<A::FileSystem*>()->GetFile(path);
   }
   
   /// Get a folder interface, relying on external modules to find it         
   ///   @param path - the path for the folder                                
   ///   @return the folder interface, or nullptr if folder doesn't exist     
   Ptr<A::Folder> Runtime::GetFolder(const Path& path) {
      auto& fileSystems = GetModules<A::FileSystem>();
      LANGULUS_ASSERT(fileSystems, Module,
         "Can't retrieve folder `", path, "` - no file system module available");
      return fileSystems.template As<A::FileSystem*>()->GetFolder(path);
   }

   /// Get the current working path (where the main exe was executed)         
   ///   @return the path                                                     
   const Path& Runtime::GetWorkingPath() const {
      auto& fileSystems = GetModules<A::FileSystem>();
      LANGULUS_ASSERT(fileSystems, Module,
         "Can't retrieve working path", " - no file system module available");
      return fileSystems.template As<A::FileSystem*>()->GetWorkingPath();
   }

   /// Get the current data path, like GetWorkingPath() / "data"              
   ///   @return the path                                                     
   const Path& Runtime::GetDataPath() const {
      auto& fileSystems = GetModules<A::FileSystem>();
      LANGULUS_ASSERT(fileSystems, Module,
         "Can't retrieve data path", " - no file system module available");
      return fileSystems.template As<A::FileSystem*>()->GetDataPath();
   }

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
   Runtime::operator Debug() const {
      return IdentityOf(this);
   }

} // namespace Langulus::Entity
