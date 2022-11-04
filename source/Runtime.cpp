///                                                                           
/// Langulus::Entity                                                          
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#include "Thing.hpp"
#include "Runtime.hpp"

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
      Logger::Verbose() << "Initializing...";
      Logger::Verbose() << "Initialized";
   }

   /// Runtime destruction                                                    
   Runtime::~Runtime() {
      while (!mLibraries.IsEmpty())
         UnloadSharedLibrary(mLibraries.GetValue(IndexFirst));
   }

   /// Create a module instance or return an already instantiated one         
   ///   @param name - module name                                            
   ///   @param descriptor - module initialization descriptor                 
   ///   @return the new module instance                                      
   Module* Runtime::InstantiateModule(const Token& name, const Any& descriptor) {
      // Check if module is already instantiated                        
      auto& foundModules = GetModules(name);
      if (!foundModules.IsEmpty())
         return foundModules[0];

      // A module instance doesn't exist yet, so instantiate it         
      Logger::Verbose()
         << "Module `" << name << "` is not instantiated yet"
         << ", so attempting to create it...";

      auto library = LoadSharedLibrary(name);
      return InstantiateModule(library, descriptor);
   }
   
   /// Create a module instance or return an already instantiated one         
   ///   @param library - the library handle                                  
   ///   @param descriptor - module initialization descriptor                 
   ///   @return the new module instance                                      
   Module* Runtime::InstantiateModule(const SharedLibrary& library, const Any& descriptor) {
      if (!library.IsValid())
         return nullptr;

      // Use the creation point of the library to instantiate module    
      const auto& info = library.mInfo();
      auto module = library.mCreator(this, descriptor);
      if (!module) {
         Logger::Error()
            << "Module `" << info.mName << "` creator didn't provide a module";
         return nullptr;
      }

      // Register the module in the various maps, for fast access       
      try {
         mModules[info.mPriority] << module;
         mInstantiations[library] << module;
         mModulesByType[module->GetType()] << module;
      }
      catch (...) {
         Logger::Error()
            << "Registering module `" << info.mName << "` failed";

         // Make sure we end up in an invariant state                   
         mModules[info.mPriority].RemoveValue(module);
         if (mModules[info.mPriority].IsEmpty())
            mModules.RemoveKey(info.mPriority);

         mInstantiations[library].RemoveValue(module);
         if (mInstantiations[library].IsEmpty())
            mInstantiations.RemoveKey(library);

         mModulesByType[module->GetType()].RemoveValue(module);
         if (mModulesByType[module->GetType()].IsEmpty())
            mModulesByType.RemoveKey(module->GetType());

         delete module;
         return nullptr;
      }

      // Done, if reached                                               
      Logger::Verbose()
         << "Module `" << info.mName
         << "` registered with priority " << info.mPriority;
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

      path += "Mod.";
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
         Logger::Error() << "Failed to load module `" << path << "` - file is missing or corrupted; Error code: ";
         #if LANGULUS_OS(WINDOWS)
            Logger::Append() << ::GetLastError();
         #endif
         return {};
      }

      // Great success!                                                 
      Logger::Info() << "Module `" << path << "` loaded";

      SharedLibrary library;
      static_assert(sizeof(library.mHandle) == sizeof(dll), "Size mismatch");
      library.mHandle = reinterpret_cast<decltype(library.mHandle)>(dll);

      // Get entry, creator, info and exit points from the library      
      #if LANGULUS_OS(WINDOWS)
         library.mEntry = reinterpret_cast<Module::EntryPoint>(
            GetProcAddress(dll, LANGULUS_MODULE_ENTRY_TOKEN()));
         library.mCreator = reinterpret_cast<Module::CreatePoint>(
            GetProcAddress(dll, LANGULUS_MODULE_CREATE_TOKEN()));
         library.mInfo = reinterpret_cast<Module::InfoPoint>(
            GetProcAddress(dll, LANGULUS_MODULE_INFO_TOKEN()));
      #elif LANGULUS_OS(LINUX)
         library.mEntry = reinterpret_cast<Module::EntryPoint>(
            dlsym(dll, LANGULUS_MODULE_ENTRY_TOKEN()));
         library.mCreator = reinterpret_cast<Module::CreatePoint>(
            dlsym(dll, LANGULUS_MODULE_CREATE_TOKEN()));
         library.mInfo = reinterpret_cast<Module::InfoPoint>(
            dlsym(dll, LANGULUS_MODULE_INFO_TOKEN()));
      #else 
         #error Unsupported OS
      #endif   

      if (!library.mEntry) {
         Logger::Error()
            << "Module `" << path << "` has no valid entry point - "
            << "the function " LANGULUS_MODULE_ENTRY_TOKEN() " is missing";
         UnloadSharedLibrary(library);
         return {};
      }

      if (!library.mCreator) {
         Logger::Error()
            << "Module `" << path << "` has no valid instantiation point - "
            << "the function " LANGULUS_MODULE_CREATE_TOKEN() " is missing";
         UnloadSharedLibrary(library);
         return {};
      }

      if (!library.mInfo) {
         Logger::Error()
            << "Module `" << path << "` has no valid information point - "
            << "the function " LANGULUS_MODULE_INFO_TOKEN() " is missing";
         UnloadSharedLibrary(library);
         return {};
      }

      // Link the module - this shall merge RTTI definitions            
      // It might throw if out of memory or on meta collision, while    
      // registering new types on the other side                        
      try {
         library.mTypes = library.mEntry();

         mLibraries.Insert(path, library);
         for (auto externalType : library.mTypes)
            mDependencies.Insert(externalType, library);
      }
      catch (...) {
         // Make sure we end up in an invariant state                   
         Logger::Error() << "Could not enter `" << path << "` due to an exception";
         mLibraries.RemoveKey(path);
         for (auto externalType : library.mTypes)
            mDependencies.RemoveKey(externalType);
         UnloadSharedLibrary(library);
         return {};
      }

      return library;
   }

   /// Unload a DLL/SO extension module                                       
   ///   @param module - the module handle to unload                          
   void Runtime::UnloadSharedLibrary(const SharedLibrary& module) {
      if (module.mHandle == 0)
         return;

      for (auto externalType : module.mTypes) {
         mDependencies.RemoveKey(externalType);
         RTTI::Database.Unregister(externalType);
      }

      #if LANGULUS_OS(WINDOWS)
         ::Langulus::Entity::UnloadSharedLibrary(
            reinterpret_cast<HMODULE>(module.mHandle));
      #elif LANGULUS_OS(LINUX)
         ::Langulus::Entity::UnloadSharedLibrary(
            reinterpret_cast<void*>(module.mHandle));
      #else 
         #error Unsupported OS
      #endif
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
      auto found = mDependencies.FindKeyIndex(type);
      if (found)
         return mInstantiations[mDependencies.GetValue(found)];

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

   /// Update the runtime, by updating all module instantiations              
   ///   @param dt - delta time between update calls                          
   void Runtime::Update(Time dt) {
      for (auto pair : mInstantiations) {
         for (auto module : pair.mValue)
            module->Update(dt);
      }
   }

} // namespace Langulus::Entity
