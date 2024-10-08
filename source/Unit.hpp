///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
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
   ///   Unit is a shorter name for a component, or extension. It's used for  
   /// composing Things' behavior. Units are usually produced from factories  
   /// inside external, dynamically loaded modules. There are units for       
   /// graphics, input, AI, content, and whatever extensions you make.        
   ///                                                                        
   struct LANGULUS_API(ENTITY) Unit
      : virtual Resolvable
      #if LANGULUS_COMPILER(MSVC)
      #pragma warning(suppress: 4275)
      , virtual Referenced
      #else
      , virtual Referenced
      #endif
      , Entity::SeekInterface<Unit>
   {
      LANGULUS(UNINSERTABLE) false;
      LANGULUS_BASES(Resolvable);

   protected:
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
      Unit() noexcept : Resolvable {this} {}
      Unit(const Unit&) = delete;
      Unit(Unit&&) noexcept = delete;
      ~Unit();

      Unit& operator = (const Unit&) = delete;
      Unit& operator = (Unit&&) noexcept = delete;

      void Select(Flow::Verb&);

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
      NOD() Langulus::Trait SeekTrait(TMeta, Index = 0);
      template<Seek = Seek::HereAndAbove>
      NOD() Langulus::Trait SeekTraitAux(const Neat&, TMeta, Index = 0);

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
      NOD() TMany<Unit*> GatherUnits(DMeta);
      template<Seek = Seek::HereAndAbove>
      NOD() TMany<Unit*> GatherUnitsExt(DMeta, const Neat&);

      template<Seek = Seek::HereAndAbove>
      NOD() TraitList GatherTraits(TMeta);

      template<CT::Data D, Seek = Seek::HereAndAbove>
      NOD() TMany<D> GatherValues() const;

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
