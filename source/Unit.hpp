///                                                                           
/// Langulus::Entity                                                          
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Hierarchy.hpp"

namespace Langulus::Entity
{

   ///                                                                        
   ///   An abstract unit                                                     
   ///                                                                        
   ///   Unit is a shorter name for a component, or extension. It's used for  
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
      Hierarchy mOwners;

   public:
      /// A unit can not be default-created, nor be shallowly copied          
      Unit() = delete;
      Unit(const Unit&) = delete;
      Unit& operator = (const Unit&) = delete;

      /// A unit can only be moved or created with type and owner             
      Unit(DMeta, const Any&) noexcept;
      Unit(Unit&&) noexcept;
      virtual ~Unit();

      Unit& operator = (Unit&&) noexcept;

      void Select(Verb&);

   public:
      virtual void Refresh() {}

      NOD() Runtime* GetRuntime() const noexcept;
      NOD() const Hierarchy& GetOwners() const noexcept;

      template<CT::Trait T, SeekStyle = SeekStyle::HereAndAbove>
      Trait SeekTrait(Offset = 0) const;

      template<SeekStyle = SeekStyle::HereAndAbove, CT::Data T>
      bool SeekValue(TMeta, T&, Offset = 0) const;
      template<SeekStyle = SeekStyle::HereAndAbove, CT::Trait T, CT::Data D>
      bool SeekValue(D&, Offset = 0) const;

      template<SeekStyle SEEK = SeekStyle::HereAndAbove>
      bool DoInHierarchy(Verb&);

   protected:
      void Couple(const Thing*);
      void Decouple(const Thing*);
      void ReplaceOwner(const Thing*, const Thing*);
   };

} // namespace Langulus::Entity


namespace Langulus::CT
{

   /// Any type that inherits Unit is considered a unit                       
   template<class T>
   concept Unit = DerivedFrom<T, ::Langulus::Entity::Unit>;

} // namespace Langulus::CT

#include "Unit.inl"
