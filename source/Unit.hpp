///                                                                           
/// Langulus::Entity                                                          
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Hierarchy.hpp"
#include "Pinnable.hpp"

namespace Langulus::Entity
{

   ///                                                                        
   ///   An abstract unit                                                     
   ///                                                                        
   /// Unit is a shorter name for a component, or extension. It's used for    
   /// composing Things' behavior. Units are usually produced from factories  
   /// inside external, dynamically loaded modules. There are units for       
   /// graphics, input, AI, content, and whatever extensions you make.        
   ///                                                                        
   struct Unit : Resolvable {
      LANGULUS(UNINSERTABLE) false;
      LANGULUS_BASES(Resolvable);

   protected: TESTING(public:)
      friend class Thing;

      // Things that are coupled with this unit                         
      // Owners act as an environment for the unit's context, providing 
      // additional traits and other units for interoperability         
      static_assert(CT::Complete<Hierarchy>, "Oops");
      Hierarchy mOwners;

   public:
      /// A unit can not be default-created, nor be shallowly copied          
      Unit() = delete;
      Unit(const Unit&) = delete;
      Unit& operator = (const Unit&) = delete;

      /// A unit can only be moved or created with type and owner             
      LANGULUS_API(ENTITY) Unit(DMeta, const Any&) noexcept;
      LANGULUS_API(ENTITY) Unit(Unit&&) noexcept;
      LANGULUS_API(ENTITY) virtual ~Unit();

      LANGULUS_API(ENTITY) Unit& operator = (Unit&&) noexcept;

      LANGULUS_API(ENTITY) void Select(Verb&);

   public:
      virtual void Refresh() {}

      NOD() LANGULUS_API(ENTITY) Runtime* GetRuntime() const noexcept;
      NOD() LANGULUS_API(ENTITY) const Hierarchy& GetOwners() const noexcept;

      template<Seek = Seek::HereAndAbove>
      bool DoInHierarchy(Verb&);

      NOD() LANGULUS_API(ENTITY) bool CompareDescriptor(const Block&) const;
      
      ///                                                                     
      ///   Seek                                                              
      ///                                                                     
      LANGULUS_SEEK_INTERFACE();
      LANGULUS_SEEK_TOKEN_INTERFACE();

      ///                                                                     
      ///   Gather                                                            
      ///                                                                     
      LANGULUS_GATHER_INTERFACE();
      LANGULUS_GATHER_TOKEN_INTERFACE();

   protected:
      LANGULUS_API(ENTITY) void Couple(const Thing*);
      LANGULUS_API(ENTITY) void Decouple(const Thing*);
      LANGULUS_API(ENTITY) void ReplaceOwner(const Thing*, const Thing*);
   };

} // namespace Langulus::Entity

namespace Langulus::CT
{

   /// Any type that inherits Unit is considered a unit                       
   template<class T>
   concept Unit = DerivedFrom<T, ::Langulus::Entity::Unit>;

} // namespace Langulus::CT
