///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Hierarchy.hpp"


namespace Langulus::A
{

   using Entity::Thing;
   using Entity::Hierarchy;
   using Entity::Runtime;


   ///                                                                        
   ///   An abstract unit                                                     
   ///                                                                        
   /// Unit is a shorter name for a component, or extension. It's used for    
   /// composing Things' behavior. Units are usually produced from factories  
   /// inside external, dynamically loaded modules. There are units for       
   /// graphics, input, AI, content, and whatever extensions you make.        
   ///                                                                        
   struct LANGULUS_API(ENTITY) Unit : Resolvable, Entity::SeekInterface<Unit> {
      LANGULUS(UNINSERTABLE) false;
      LANGULUS_BASES(Resolvable);

   protected: IF_LANGULUS_TESTING(public:)
      friend class Thing;

      // Things that are coupled with this unit                         
      // Owners act as an environment for the unit's context, providing 
      // additional traits and other units for interoperability         
      #if LANGULUS_COMPILER(MSVC)
         #pragma warning(suppress: 4251)
         Hierarchy mOwners;
      #else
         Hierarchy mOwners;
      #endif

   public:
      /// A unit can not be default-created, nor be shallow-copied            
      Unit() = delete;
      Unit(const Unit&) = delete;
      Unit& operator = (const Unit&) = delete;

      /// A unit can only be moved or created with type and owner             
      Unit(DMeta) noexcept;
      Unit(Unit&&) noexcept;

      Unit& operator = (Unit&&) noexcept;

      void Select(Flow::Verb&);

   public:
      virtual void Refresh();

      Runtime* GetRuntime() const noexcept;
      const Hierarchy& GetOwners() const noexcept;
      bool CompareDescriptor(const Neat&) const;
      
      ///                                                                     
      ///   Flow                                                              
      ///                                                                     
      template<Seek = Seek::HereAndAbove, CT::VerbBased V>
      V& RunIn(V&);

      ///                                                                     
      ///   Seek                                                              
      ///                                                                     
      using SeekInterface::SeekUnit;
      using SeekInterface::SeekUnitAux;
      using SeekInterface::SeekUnitExt;
      using SeekInterface::SeekUnitAuxExt;
      using SeekInterface::SeekTrait;
      using SeekInterface::SeekTraitAux;
      using SeekInterface::SeekValue;
      using SeekInterface::SeekValueAux;

      template<Seek = Seek::HereAndAbove>
      NOD() Unit* SeekUnit(DMeta, Index = 0);
      template<Seek = Seek::HereAndAbove>
      NOD() Unit* SeekUnitAux(const Neat&, DMeta, Index = 0);
      template<Seek = Seek::HereAndAbove>
      NOD() Unit* SeekUnitExt(DMeta, const Neat&, Index = 0);
      template<Seek = Seek::HereAndAbove>
      NOD() Unit* SeekUnitAuxExt(DMeta, const Neat&, const Neat&, Index = 0);

      template<Seek = Seek::HereAndAbove>
      NOD() Trait SeekTrait(TMeta, Index = 0);
      template<Seek = Seek::HereAndAbove>
      NOD() Trait SeekTraitAux(const Neat&, TMeta, Index = 0);

      template<Seek = Seek::HereAndAbove>
      bool SeekValue(TMeta, CT::Data auto&, Index = 0) const;
      template<Seek = Seek::HereAndAbove>
      bool SeekValueAux(TMeta, const Neat&, CT::Data auto&, Index = 0) const;

      ///                                                                     
      ///   Gather                                                            
      ///                                                                     
      using SeekInterface::GatherUnits;
      using SeekInterface::GatherUnitsExt;
      using SeekInterface::GatherTraits;

      template<Seek = Seek::HereAndAbove>
      NOD() TAny<Unit*> GatherUnits(DMeta);
      template<Seek = Seek::HereAndAbove>
      NOD() TAny<Unit*> GatherUnitsExt(DMeta, const Neat&);

      template<Seek = Seek::HereAndAbove>
      NOD() TraitList GatherTraits(TMeta);

      template<CT::Data D, Seek = Seek::HereAndAbove>
      NOD() TAny<D> GatherValues() const;

   protected:
      void Couple(const Neat&);
      void Decouple(const Thing*);
      void ReplaceOwner(const Thing*, const Thing*);
   };

} // namespace Langulus::Entity

namespace Langulus::CT
{

   /// Any type that inherits Unit is considered a unit                       
   template<class T>
   concept Unit = DerivedFrom<T, A::Unit>;

} // namespace Langulus::CT
