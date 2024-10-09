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

      auto GetRuntime() const noexcept -> Runtime*;
      auto GetOwners()  const noexcept -> const Hierarchy&;
      bool CompareDescriptor(const Many&) const;
      
      ///                                                                     
      ///   Flow                                                              
      ///                                                                     
      template<Seek = Seek::HereAndAbove, CT::VerbBased V>
      V& RunIn(V&);

      ///                                                                     
      ///   Seek                                                              
      ///                                                                     
      using SeekInterface::SeekUnitAux;
      using SeekInterface::SeekUnitExt;
      using SeekInterface::SeekUnitAuxExt;
      using SeekInterface::SeekTraitAux;
      using SeekInterface::SeekValueAux;

      template<Seek = Seek::HereAndAbove> NOD()
      auto SeekUnitAux(const Many&, DMeta, Index = 0) -> Unit*;
      template<Seek = Seek::HereAndAbove> NOD()
      auto SeekUnitExt(DMeta, const Many&, Index = 0) -> Unit*;
      template<Seek = Seek::HereAndAbove> NOD()
      auto SeekUnitAuxExt(DMeta, const Many&, const Many&, Index = 0) -> Unit*;

      template<Seek = Seek::HereAndAbove> NOD()
      auto SeekTraitAux(const Many&, TMeta, Index = 0) -> Langulus::Trait;

      template<Seek = Seek::HereAndAbove>
      bool SeekValueAux(TMeta, const Many&, CT::Data auto&, Index = 0) const;

      ///                                                                     
      ///   Gather                                                            
      ///                                                                     
      using SeekInterface::GatherUnits;
      using SeekInterface::GatherUnitsExt;
      using SeekInterface::GatherTraits;

      template<Seek = Seek::HereAndAbove> NOD()
      auto GatherUnits(DMeta) -> TMany<Unit*>;
      template<Seek = Seek::HereAndAbove> NOD()
      auto GatherUnitsExt(DMeta, const Many&) -> TMany<Unit*>;

      template<Seek = Seek::HereAndAbove> NOD()
      auto GatherTraits(TMeta) -> TraitList;

      template<CT::Data D, Seek = Seek::HereAndAbove> NOD()
      auto GatherValues() const -> TMany<D>;

   protected:
      void Couple(const Many&, const Thing* fallback = nullptr);
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
