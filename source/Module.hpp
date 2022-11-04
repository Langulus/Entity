///                                                                           
/// Langulus::Entity                                                          
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Common.hpp"

LANGULUS_EXCEPTION(Module);

namespace Langulus
{

   using MetaList = Anyness::TAny<const RTTI::Meta*>;

   /// Helper function, that reflects and registers a type list               
   ///   @return a container with list of all the registered types            
   template<class... T>
   MetaList RegisterTypeList() {
      static const MetaList types {
         MetaList::Wrap(MetaOf<T>()...)
      };
      return types;
   }

} // namespace Langulus


namespace Langulus::Entity
{

   /// Predeclarations                                                        

   class Module;
   class Runtime;


   ///                                                                        
   ///   External module interface                                            
   ///                                                                        
   class Module : public Resolvable {
      LANGULUS(PRODUCER) Runtime;
      LANGULUS(UNINSERTABLE) false;
      LANGULUS_BASES(Resolvable);

   private:
      // Runtime that owns the module instance                          
      Runtime* mRuntime;

   public:
      Module(DMeta classid, Runtime* runtime) SAFETY_NOEXCEPT()
         : Resolvable {classid}
         , mRuntime {runtime} {}

      /// Never allow construction of uninitialized modules                   
      Module() = delete;
      Module(const Module&) = delete;
      Module(Module&&) = delete;
      virtual ~Module() {}
      
      Module& operator = (const Module&) noexcept = default;
      Module& operator = (Module&&) noexcept = default;

      struct Info {
         // Define the order in which module updates, relative to others
         Real mPriority = 0;
         // Name of the module                                          
         Token mName;
         // Information about the module                                
         Token mInfo;
         // Relative module dedicated file folder, under Data/Modules/  
         Token mDepository;
         // Module abstract type                                        
         DMeta mCategory {};
      };

      using EntryFunction = MetaList(*)();
      using CreateFunction = Module*(*)(Runtime*, const Any&);
      using InfoFunction = const Info&(*)();

      NOD() Runtime* GetRuntime() const noexcept { return mRuntime; }

   public:
      virtual void Update(Time) = 0;
   };

} // namespace Langulus::Entity

namespace Langulus::CT
{

   /// Any type that inherits Module is considered a module                   
   template<class T>
   concept Module = DerivedFrom<T, ::Langulus::Entity::Module>;

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
   extern "C" { \
      LANGULUS_EXPORT() ::Langulus::MetaList LANGULUS_MODULE_ENTRY() () { \
         return ::Langulus::RegisterTypeList<m, __VA_ARGS__>();\
      } \
      \
      LANGULUS_EXPORT() ::Langulus::Entity::Module* LANGULUS_MODULE_CREATE() ( \
         ::Langulus::Entity::Runtime* rt, const ::Langulus::Anyness::Any& desc) { \
         static_assert(::Langulus::CT::DerivedFrom<m, ::Langulus::Entity::Module>, \
            "Langulus module class interface " \
            #m " doesn't inherit ::Langulus::Entity::Module"); \
         static_assert(!::Langulus::CT::Abstract<m>, \
            "Langulus module class interface " \
            #m " is abstract, have you forgotten to define its interface?"); \
         return new m {rt, desc}; \
      } \
      \
      LANGULUS_EXPORT() const ::Langulus::Entity::Module::Info& LANGULUS_MODULE_INFO() () { \
         static const ::Langulus::Entity::Module::Info i { \
            prio, name, info, depo, cat \
         }; \
         return i; \
      } \
   }
