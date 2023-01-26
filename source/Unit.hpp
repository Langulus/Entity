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

      template<SeekStyle SEEK = SeekStyle::HereAndAbove>
      bool DoInHierarchy(Verb&);
      
      ///                                                                     
      ///   Seek                                                              
      ///                                                                     
      #if LANGULUS_FEATURE(MANAGED_MEMORY)
         template<SeekStyle = SeekStyle::HereAndAbove>
         NOD() Unit* SeekUnit(const Token&, const Index& = IndexFirst);
         template<SeekStyle = SeekStyle::HereAndAbove>
         NOD() const Unit* SeekUnit(const Token&, const Index& = IndexFirst) const;
         template<SeekStyle = SeekStyle::HereAndAbove>
         NOD() const Unit* SeekUnit(const Any&, const Token&, Index = IndexFirst) const;
      #endif

      template<SeekStyle = SeekStyle::HereAndAbove>
      NOD() Unit* SeekUnit(DMeta, const Index& = IndexFirst);
      template<SeekStyle = SeekStyle::HereAndAbove>
      NOD() const Unit* SeekUnit(DMeta, const Index& = IndexFirst) const;
      template<SeekStyle = SeekStyle::HereAndAbove>
      NOD() const Unit* SeekUnit(const Any&, DMeta, const Index& = IndexFirst) const;

      template<CT::Data T, SeekStyle = SeekStyle::HereAndAbove>
      NOD() Decay<T>* SeekUnit(const Index& = IndexFirst);
      template<CT::Data T, SeekStyle = SeekStyle::HereAndAbove>
      NOD() const Decay<T>* SeekUnit(const Index& = IndexFirst) const;
      template<CT::Data T, SeekStyle = SeekStyle::HereAndAbove>
      NOD() const Decay<T>* SeekUnit(const Any&, const Index& = IndexFirst) const;

      #if LANGULUS_FEATURE(MANAGED_MEMORY)
         template<SeekStyle = SeekStyle::HereAndAbove>
         NOD() Trait SeekTrait(const Token&, const Index& = IndexFirst);
         template<SeekStyle = SeekStyle::HereAndAbove>
         NOD() Trait SeekTrait(const Token&, const Index& = IndexFirst) const;
         template<SeekStyle = SeekStyle::HereAndAbove, CT::Data D>
         NOD() bool SeekTrait(const Token&, D&, const Index& = IndexFirst) const;

         template<SeekStyle = SeekStyle::HereAndAbove>
         NOD() Trait SeekTrait(const Any&, const Token&, const Index& = IndexFirst) const;
         template<SeekStyle = SeekStyle::HereAndAbove, CT::Data D>
         NOD() bool SeekTrait(const Any&, const Token&, D&, const Index& = IndexFirst) const;
      #endif

      template<SeekStyle = SeekStyle::HereAndAbove>
      NOD() Trait SeekTrait(TMeta, const Index& = IndexFirst);
      template<SeekStyle = SeekStyle::HereAndAbove>
      NOD() Trait SeekTrait(TMeta, const Index& = IndexFirst) const;
      template<SeekStyle = SeekStyle::HereAndAbove, CT::Data D>
      NOD() bool SeekTrait(TMeta, D&, const Index& = IndexFirst) const;

      template<SeekStyle = SeekStyle::HereAndAbove>
      NOD() Trait SeekTrait(const Any&, TMeta, const Index& = IndexFirst) const;
      template<SeekStyle = SeekStyle::HereAndAbove, CT::Data D>
      NOD() bool SeekTrait(const Any&, TMeta, D&, const Index& = IndexFirst) const;

      template<CT::Trait T = Trait, SeekStyle = SeekStyle::HereAndAbove>
      NOD() Trait SeekTrait(const Index& = IndexFirst);
      template<CT::Trait T = Trait, SeekStyle = SeekStyle::HereAndAbove>
      NOD() Trait SeekTrait(const Index& = IndexFirst) const;
      template<CT::Trait T = Trait, SeekStyle = SeekStyle::HereAndAbove, CT::Data D>
      NOD() bool SeekTrait(D&, const Index& = IndexFirst) const;

      template<CT::Trait T = Trait, SeekStyle = SeekStyle::HereAndAbove>
      NOD() Trait SeekTrait(const Any&, const Index& = IndexFirst) const;
      template<CT::Trait T = Trait, SeekStyle = SeekStyle::HereAndAbove, CT::Data D>
      NOD() bool SeekTrait(const Any&, D&, const Index& = IndexFirst) const;

      template<SeekStyle = SeekStyle::HereAndAbove, CT::Data D>
      bool SeekValue(D&, const Index& = IndexFirst) const;
      template<SeekStyle = SeekStyle::HereAndAbove, CT::Data D>
      bool SeekValue(const Any&, D&, const Index& = IndexFirst) const;

      ///                                                                     
      ///   Gather                                                            
      ///                                                                     
      #if LANGULUS_FEATURE(MANAGED_MEMORY)
         template<SeekStyle = SeekStyle::HereAndAbove>
         NOD() TAny<Unit*> GatherUnits(const Token&);
         template<SeekStyle = SeekStyle::HereAndAbove>
         NOD() TAny<const Unit*> GatherUnits(const Token&) const;
      #endif 

      template<SeekStyle = SeekStyle::HereAndAbove>
      NOD() TAny<Unit*> GatherUnits(DMeta);
      template<SeekStyle = SeekStyle::HereAndAbove>
      NOD() TAny<const Unit*> GatherUnits(DMeta) const;
      template<CT::Data T = Unit, SeekStyle = SeekStyle::HereAndAbove>
      NOD() TAny<Decay<T>*> GatherUnits();
      template<CT::Data T = Unit, SeekStyle = SeekStyle::HereAndAbove>
      NOD() TAny<const Decay<T>*> GatherUnits() const;

      #if LANGULUS_FEATURE(MANAGED_MEMORY)
         template<SeekStyle = SeekStyle::HereAndAbove>
         NOD() TAny<Trait> GatherTraits(const Token&);
         template<SeekStyle = SeekStyle::HereAndAbove>
         NOD() TAny<Trait> GatherTraits(const Token&) const;
      #endif 

      template<SeekStyle = SeekStyle::HereAndAbove>
      NOD() TAny<Trait> GatherTraits(TMeta);
      template<SeekStyle = SeekStyle::HereAndAbove>
      NOD() TAny<Trait> GatherTraits(TMeta) const;
      template<CT::Trait T = Trait, SeekStyle = SeekStyle::HereAndAbove>
      NOD() TAny<Trait> GatherTraits();
      template<CT::Trait T = Trait, SeekStyle = SeekStyle::HereAndAbove>
      NOD() TAny<Trait> GatherTraits() const;

      template<SeekStyle = SeekStyle::HereAndAbove, CT::Data D>
      NOD() TAny<D> GatherValues() const;

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
