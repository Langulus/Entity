///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Module.hpp"


namespace Langulus::A
{
   struct File;
   struct Folder;
}

namespace Langulus::Entity
{

   class Thing;


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
         A::Module::EntryFunction mEntry {};
         // Exported module instantiation function, produces modules    
         A::Module::CreateFunction mCreator {};
         // Information function, returning module description          
         A::Module::InfoFunction mInfo {};
         // Type of the module instance                                 
         DMeta mModuleType {};
         // The RTTI::Boundary of the library                           
         Token mBoundary;
         // Whether or not library is marked for unload                 
         bool mMarkedForUnload {};

      public:
         constexpr SharedLibrary() noexcept = default;

         /// Explicit abandon construction, for optimized containment         
         ///   @param other - the library to abandon                          
         SharedLibrary(Abandoned<SharedLibrary>&& other) noexcept
            : mHandle {other->mHandle}
            , mEntry {other->mEntry}
            , mCreator {other->mCreator}
            , mInfo {other->mInfo}
            , mModuleType {other->mModuleType}
            , mBoundary {other->mBoundary}
            , mMarkedForUnload {other->mMarkedForUnload} {}

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
            return HashBytes(&mHandle, static_cast<int>(sizeof(mHandle)));
         }
      };
      
      // The owner of the runtime                                       
      Thing* mOwner {};
      // Loaded shared libraries, indexed by filename                   
      // This is a static registry - all Runtimes use the same shared   
      // library objects, but manage their own module instantiations    
      static TUnorderedMap<Token, SharedLibrary> mLibraries;
      // Instantiated modules, sorted by priority                       
      TOrderedMap<Real, ModuleList> mModules;
      // Instantiated modules, indexed by type                          
      TUnorderedMap<DMeta, ModuleList> mModulesByType;

   protected:
      NOD() LANGULUS_API(ENTITY)
      SharedLibrary LoadSharedLibrary(const Token&);
      NOD() bool UnloadSharedLibrary(const SharedLibrary&);

   public:
      LANGULUS_CONVERTS_TO(Text);

      Runtime() = delete;
      Runtime(Runtime&&) noexcept = default;

      LANGULUS_API(ENTITY)  Runtime(Thing*) noexcept;
      LANGULUS_API(ENTITY) ~Runtime();

      NOD() auto GetOwner() const noexcept { return mOwner; }

      NOD() LANGULUS_API(ENTITY)
      A::Module* InstantiateModule(const Token&, const Neat& = {});

      NOD() LANGULUS_API(ENTITY)
      A::Module* InstantiateModule(const SharedLibrary&, const Neat& = {});

      NOD() LANGULUS_API(ENTITY)
      SharedLibrary GetDependency(DMeta) const noexcept;

      NOD() LANGULUS_API(ENTITY)
      const ModuleList& GetModules(DMeta) const noexcept;

      template<CT::Module M>
      NOD() const ModuleList& GetModules() const noexcept {
         return GetModules(MetaDataOf<M>());
      }

      #if LANGULUS_FEATURE(MANAGED_REFLECTION)
         NOD() LANGULUS_API(ENTITY)
         SharedLibrary GetDependencyToken(const Token&) const noexcept;

         NOD() LANGULUS_API(ENTITY)
         const ModuleList& GetModulesToken(const Token&) const noexcept;
      #endif

      NOD() LANGULUS_API(ENTITY)
      Ref<A::File> GetFile(const Path&);
      NOD() LANGULUS_API(ENTITY)
      Ref<A::Folder> GetFolder(const Path&);
      NOD() LANGULUS_API(ENTITY)
      const Path& GetWorkingPath() const;
      NOD() LANGULUS_API(ENTITY)
      const Path& GetDataPath() const;

      LANGULUS_API(ENTITY)
      bool Update(Time);

      NOD() LANGULUS_API(ENTITY)
      explicit operator Text() const;
   };

} // namespace Langulus::Entity
