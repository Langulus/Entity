///                                                                           
/// Langulus::Entity                                                          
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Module.hpp"
#include <Anyness/TMap.hpp>
#include <Anyness/TAny.hpp>


namespace Langulus::Entity
{

   using ModuleList = TAny<Module*>;


   ///                                                                        
   ///   Runtime                                                              
   ///                                                                        
   ///   Handles loading and unloading of modules, dispatches verbs to        
   /// modules, handles overall system state and console input. Overall, it   
   /// takes care of the minimally required OS aspect of running a program.   
   ///   You can view a Runtime as a single process inside Langulus. You can  
   /// create infinite nested subprocesses in any Thing. Each new runtime     
   /// will act as the environment for that Thing, as well as all of its      
   /// children, modules, units, etc.                                         
   ///                                                                        
   class Runtime final {
   protected:
      ///                                                                     
      ///   Library handle                                                    
      ///                                                                     
      class SharedLibrary {
      friend class Runtime;
      protected:
         // Library handle, returned by dlopen or LoadLibrary           
         uintptr_t mHandle {};
         // Exported entry function, that registers library types       
         Module::EntryFunction mEntry {};
         // Exported module instantiation function, produces modules    
         Module::CreateFunction mCreator {};
         // Information function, returning module description          
         Module::InfoFunction mInfo {};
         // Type of the module instance                                 
         DMeta mModuleType {};
         // Set of types, that were registered upon entry               
         MetaList mTypes;
         // Number of references for the library                        
         Count mReferences {1};

      public:
         /// Check if the shared library handle is valid                      
         NOD() constexpr bool IsValid() const noexcept {
            return mHandle != 0;
         }

         /// Compare shared libraries by handle only                          
         NOD() constexpr bool operator == (const SharedLibrary& rhs) const noexcept {
            return mHandle == rhs.mHandle;
         }

         /// Hashing for to being able to contain it in a map                 
         NOD() constexpr Hash GetHash() const noexcept {
            return HashNumber(mHandle);
         }
      };

      Thing* mOwner {};

      // Loaded shared libraries, indexed by filename                   
      // This is a static registry - all Runtimes use the same shared   
      // library objects, but manage their own module instantiations    
      static TUnorderedMap<Token, SharedLibrary> mLibraries;

      // Shared library dependencies for all externally registered types
      static TUnorderedMap<const RTTI::Meta*, SharedLibrary> mDependencies;

      // Instantiated modules, sorted by priority                       
      TMap<Real, ModuleList> mModules;

      // Instantiated modules, indexed by type                          
      TUnorderedMap<DMeta, ModuleList> mModulesByType;

      void UnloadSharedLibrary(const SharedLibrary&);

   public:
      LANGULUS_CONVERSIONS(Debug);

      Runtime() = delete;
      Runtime(Runtime&&) noexcept = default;

      LANGULUS_API(ENTITY) Runtime(Thing*) noexcept;
      LANGULUS_API(ENTITY) ~Runtime();

      NOD() auto GetOwner() const noexcept { return mOwner; }

      NOD() LANGULUS_API(ENTITY)
      SharedLibrary LoadSharedLibrary(const Token&);

      NOD() LANGULUS_API(ENTITY)
      Module* InstantiateModule(const Token&, const Any& = {});

      NOD() LANGULUS_API(ENTITY)
      Module* InstantiateModule(const SharedLibrary&, const Any& = {});

      NOD() LANGULUS_API(ENTITY)
      SharedLibrary GetDependency(DMeta) const noexcept;

      NOD() LANGULUS_API(ENTITY)
      const ModuleList& GetModules(DMeta) const noexcept;

      #if LANGULUS_FEATURE(MANAGED_REFLECTION)
         NOD() LANGULUS_API(ENTITY)
         SharedLibrary GetDependency(const Token&) const noexcept;

         NOD() LANGULUS_API(ENTITY)
         const ModuleList& GetModules(const Token&) const noexcept;
      #endif

      NOD() LANGULUS_API(ENTITY)
      A::File* GetFile(const Path&);

      LANGULUS_API(ENTITY)
      void Update(Time);

      NOD() LANGULUS_API(ENTITY)
      explicit operator Debug() const;
   };

} // namespace Langulus::Entity
