///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Common.hpp"

LANGULUS_EXCEPTION(Module);


namespace Langulus
{
   namespace A
   {
      class Module;
   }


   using MetaList = TUnorderedSet<AMeta>;
   using TraitList = TMany<Trait>;
   using ModuleList = TMany<A::Module*>;

   /// Helper function, that reflects and registers a list of any reflection  
   /// primitives, like data, verbs, and traits.                              
   /// All types in the list, as well as data member and base types, will     
   /// be associated with the module library. When library unloads, all       
   /// these definitions will be removed, because otherwise they might        
   /// contain lambdas, that no longer will exist after shared object unload. 
   /// This is possible only when managed reflection feature is enabled.      
   /// If it is not, it is your resposibility to manage these dependencies.   
   ///   @param list - a set of all the registered types. Be aware, these     
   ///      do not include bases and members, but only intentionally exposed  
   ///      types. Bases and members will still be exported if not yet        
   ///      reflected, and associated with the currently set RTTI boundary.   
   ///      These types will be unloaded when shared library is unloaded.     
   ///      Unload will be forbidden, if they're still in use.                
   template<class...T>
   void RegisterTypeList(MetaList& list) {
      // Merge to avoid duplications                                    
      (list << ... << MetaOf<T>());
   }

   namespace Entity
   {

      class Runtime;

   } // namespace Langulus::Entity

} // namespace Langulus

namespace Langulus::A
{

   ///                                                                        
   ///   External module interface                                            
   ///                                                                        
   class Module : public virtual Resolvable {
   public:
      using Runtime = Entity::Runtime;
      LANGULUS(PRODUCER) Runtime;
      LANGULUS_BASES(Resolvable);

   private:
      // Runtime that owns the module instance                          
      Runtime* mRuntime;

   public:
      Module(Runtime* runtime) IF_UNSAFE(noexcept)
         : Resolvable {this}
         , mRuntime   {runtime} {}

      Module() noexcept = delete;
      Module(const Module&) = delete;
      Module(Module&&) = delete;
      virtual ~Module() {}
      
      Module& operator = (const Module&) noexcept = delete;
      Module& operator = (Module&&) noexcept = delete;

      struct Info {
         // Define the order in which module updates, relative to others
         Langulus::Real mPriority;
         // Name of the module                                          
         const char* mName;
         // Information about the module                                
         const char* mInfo;
         // Relative module dedicated file folder, under Data/Modules/  
         const char* mDepository;
         // Module abstract type                                        
         DMeta mCategory;
      };

      using EntryFunction  = void(*)(DMeta&, MetaList&);
      using CreateFunction = Module*(*)(Runtime*, const Many&);
      using InfoFunction   = const Info*(*)();

      NOD() auto GetRuntime() const noexcept -> Runtime* {
         return mRuntime;
      }

   public:
      virtual void Teardown() = 0;

      virtual bool Update(Langulus::Time) {
         return true;
      }
   };

} // namespace Langulus::A

namespace Langulus::CT
{

   /// Any type that inherits Module is considered a module                   
   template<class T>
   concept Module = DerivedFrom<T, ::Langulus::A::Module>;

} // namespace Langulus::CT


/// Name of module entry function                                             
#define LANGULUS_MODULE_ENTRY()           LangulusModuleEntryPoint
#define LANGULUS_MODULE_ENTRY_TOKEN()     LANGULUS_STRINGIFY(LANGULUS_MODULE_ENTRY())
/// Name of module instantiation function                                     
#define LANGULUS_MODULE_CREATE()          LangulusModuleCreatePoint
#define LANGULUS_MODULE_CREATE_TOKEN()    LANGULUS_STRINGIFY(LANGULUS_MODULE_CREATE())
/// Name of module information function                                       
#define LANGULUS_MODULE_INFO()            LangulusModuleInfoPoint
#define LANGULUS_MODULE_INFO_TOKEN()      LANGULUS_STRINGIFY(LANGULUS_MODULE_INFO())


/// Convenience macro for implementing module entry and exit points           
///   @param m - the type of the module interface, must inherit Module        
///   @param prio - the priority of the module                                
///   @param name - the module identifier token                               
///   @param info - information string literal about the module               
///   @param depo - relative path for the module, under Data/Modules/         
///   @param cat - module category, i.e. some abstract type                   
///   @param ... - a type list to reflect upon module load                    
#define LANGULUS_DEFINE_MODULE(m, prio, name, info, depo, cat, ...) \
   LANGULUS_RTTI_BOUNDARY(name) \
   \
   extern "C" { \
      LANGULUS_EXPORT() \
      void LANGULUS_MODULE_ENTRY() (::Langulus::DMeta& meta, ::Langulus::MetaList& list) { \
         using DM = ::Langulus::Decay<m>; \
         meta = ::Langulus::MetaDataOf<DM>(); \
         ::Langulus::RegisterTypeList<DM, ::Langulus::Decay<cat>, __VA_ARGS__>(list); \
      } \
      \
      LANGULUS_EXPORT() \
      ::Langulus::A::Module* LANGULUS_MODULE_CREATE() ( \
         ::Langulus::Entity::Runtime* rt, const ::Langulus::Anyness::Many& desc) { \
         static_assert(::Langulus::CT::DerivedFrom<m, ::Langulus::A::Module>, \
            "Langulus module class interface " \
            #m " doesn't inherit ::Langulus::A::Module"); \
         static_assert(not ::Langulus::CT::Abstract<m>, \
            "Langulus module class interface " \
            #m " is abstract, have you forgotten to define its interface?"); \
         return new m {rt, desc}; \
      } \
      \
      LANGULUS_EXPORT() \
      const ::Langulus::A::Module::Info* LANGULUS_MODULE_INFO() () { \
         static const ::Langulus::A::Module::Info i { \
            prio, name, info, depo, ::Langulus::MetaDataOf<cat>() \
         }; \
         return &i; \
      } \
   }
