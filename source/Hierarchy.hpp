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
   struct Unit;
}

namespace Langulus::Entity
{

   class Thing;


   ///                                                                        
   ///   Seek interface for Hierarchy/Thing/Unit                              
   ///                                                                        
   /// Contains all kinds of variations of seeking/gathering functions, that  
   /// can be used to collect data from hierarchical systems. You can filter  
   /// based on type, contents, seek direction, etc.                          
   ///                                                                        
   template<class THIS>
   struct SeekInterface {
      ///                                                                     
      ///   Intentionally left undefined, you have to define them in THIS     
      ///                                                                     
      /// Can't use virtuals, because we want these to be template functions, 
      /// so that we retain the most of the static optimizations              
      /// TODO generalize these when deduce-this has been implemented well    
      ///                                                                     
      /*template<Seek = Seek::HereAndAbove>
      NOD() Unit* SeekUnit(DMeta, Index = IndexFirst) = delete;
      template<Seek = Seek::HereAndAbove>
      NOD() Unit* SeekUnitAux(const Neat&, DMeta, Index = IndexFirst) = delete;
      template<Seek = Seek::HereAndAbove>
      NOD() Unit* SeekUnitExt(DMeta, const Neat&, Index = IndexFirst) = delete;
      template<Seek = Seek::HereAndAbove>
      NOD() Unit* SeekUnitAuxExt(DMeta, const Neat&, const Neat&, Index = IndexFirst) = delete;

      template<Seek = Seek::HereAndAbove>
      NOD() Trait SeekTrait(TMeta, Index = IndexFirst) = delete;
      template<Seek = Seek::HereAndAbove>
      NOD() Trait SeekTraitAux(const Neat&, TMeta, Index = IndexFirst) = delete;

      template<Seek = Seek::HereAndAbove>
      bool SeekValue(TMeta, CT::Data auto&, Index = IndexFirst) const = delete;
      template<Seek = Seek::HereAndAbove>
      bool SeekValueAux(TMeta, const Neat&, CT::Data auto&, Index = IndexFirst) const = delete;


      template<Seek = Seek::HereAndAbove>
      NOD() TMany<Unit*> GatherUnits(DMeta) = delete;
      template<Seek = Seek::HereAndAbove>
      NOD() TMany<Unit*> GatherUnitsExt(DMeta, const Neat&) = delete;

      template<Seek = Seek::HereAndAbove>
      NOD() TraitList GatherTraits(TMeta) = delete;
      
      template<CT::Data D, Seek = Seek::HereAndAbove>
      NOD() TMany<D> GatherValues() const = delete;*/


      ///                                                                     
      /// The rest of these functions are defined for every SeekInterface     
      /// They all use static_cast<THIS*>(this) as execution context for the  
      /// above functions, which should be defined in THIS                    
      /// TODO generalize these when deduce-this has been implemented well    
      ///                                                                     
      template<Seek = Seek::HereAndAbove> NOD()
      auto SeekUnit(DMeta, Index = 0) const -> const A::Unit*;
      template<CT::Data T = A::Unit, Seek = Seek::HereAndAbove> NOD()
      auto SeekUnit(Index = 0) -> Decay<T>*;
      template<CT::Data T = A::Unit, Seek = Seek::HereAndAbove> NOD()
      auto SeekUnit(Index = 0) const -> const Decay<T>*;

      template<Seek = Seek::HereAndAbove> NOD()
      auto SeekUnitAux(const Many&, DMeta, Index = 0) const -> const A::Unit*;
      template<CT::Data T = A::Unit, Seek = Seek::HereAndAbove> NOD()
      auto SeekUnitAux(const Many&, Index = 0) -> Decay<T>*;
      template<CT::Data T = A::Unit, Seek = Seek::HereAndAbove> NOD()
      auto SeekUnitAux(const Many&, Index = 0) const -> const Decay<T>*;

      template<Seek = Seek::HereAndAbove> NOD()
      auto SeekUnitExt(DMeta, const Many&, Index = 0) const -> const A::Unit*;
      template<CT::Data T = A::Unit, Seek = Seek::HereAndAbove> NOD()
      auto SeekUnitExt(const Many&, Index = 0) -> Decay<T>*;
      template<CT::Data T = A::Unit, Seek = Seek::HereAndAbove> NOD()
      auto SeekUnitExt(const Many&, Index = 0) const -> const Decay<T>*;

      template<Seek = Seek::HereAndAbove> NOD()
      auto SeekUnitAuxExt(DMeta, const Many&, const Many&, Index = 0) const -> const A::Unit*;
      template<CT::Data T = A::Unit, Seek = Seek::HereAndAbove> NOD()
      auto SeekUnitAuxExt(const Many&, const Many&, Index = 0) -> Decay<T>*;
      template<CT::Data T = A::Unit, Seek = Seek::HereAndAbove> NOD()
      auto SeekUnitAuxExt(const Many&, const Many&, Index = 0) const -> const Decay<T>*;

      template<Seek = Seek::HereAndAbove> NOD()
      auto SeekTrait(TMeta, Index = 0) const -> Trait;
      template<CT::Trait = Trait, Seek = Seek::HereAndAbove> NOD()
      auto SeekTrait(Index = 0) -> Trait;
      template<CT::Trait = Trait, Seek = Seek::HereAndAbove> NOD()
      auto SeekTrait(Index = 0) const -> Trait;

      template<Seek = Seek::HereAndAbove> NOD()
      auto SeekTraitAux(const Many&, TMeta, Index = 0) const -> Trait;
      template<CT::Trait = Trait, Seek = Seek::HereAndAbove> NOD()
      auto SeekTraitAux(const Many&, Index = 0) -> Trait;
      template<CT::Trait = Trait, Seek = Seek::HereAndAbove> NOD()
      auto SeekTraitAux(const Many&, Index = 0) const -> Trait;


      template<CT::Trait = Trait, Seek = Seek::HereAndAbove>
      bool SeekValue(CT::NotTagged auto&, Index = IndexFirst) const;
      template<CT::Trait = Trait, Seek = Seek::HereAndAbove>
      bool SeekValueAux(const Many&, CT::NotTagged auto&, Index = 0) const;

      template<Seek = Seek::HereAndAbove>
      bool SeekValue(CT::Tagged auto&, Index = IndexFirst) const;
      template<Seek = Seek::HereAndAbove>
      bool SeekValueAux(const Many&, CT::Tagged auto&, Index = 0) const;


      template<Seek = Seek::HereAndAbove> NOD()
      auto GatherUnits(DMeta) const -> TMany<const A::Unit*> ;
      template<CT::Data T = A::Unit, Seek = Seek::HereAndAbove> NOD()
      auto GatherUnits() -> TMany<T*>;
      template<CT::Data T = A::Unit, Seek = Seek::HereAndAbove> NOD()
      auto GatherUnits() const -> TMany<const T*>;

      template<Seek = Seek::HereAndAbove> NOD()
      auto GatherUnitsExt(DMeta, const Many&) const -> TMany<const A::Unit*>;
      template<CT::Data T = A::Unit, Seek = Seek::HereAndAbove> NOD()
      auto GatherUnitsExt(const Many&) -> TMany<T*>;
      template<CT::Data T = A::Unit, Seek = Seek::HereAndAbove> NOD()
      auto GatherUnitsExt(const Many&) const -> TMany<const T*>;


      template<Seek = Seek::HereAndAbove> NOD()
      auto GatherTraits(TMeta) const -> TraitList;
      template<CT::Trait = Trait, Seek = Seek::HereAndAbove> NOD()
      auto GatherTraits() -> TraitList;
      template<CT::Trait = Trait, Seek = Seek::HereAndAbove> NOD()
      auto GatherTraits() const -> TraitList;

      
   #if LANGULUS_FEATURE(MANAGED_REFLECTION)
      ///                                                                     
      /// Token based interface                                               
      /// Available only when managed reflection is enabled                   
      ///                                                                     
      template<Seek = Seek::HereAndAbove> NOD()
      auto SeekUnit(const Token&, Index = 0)       ->       A::Unit*;
      template<Seek = Seek::HereAndAbove> NOD()
      auto SeekUnit(const Token&, Index = 0) const -> const A::Unit*;

      template<Seek = Seek::HereAndAbove> NOD()
      auto SeekUnitAux(const Many&, const Token&, Index = 0)       ->       A::Unit*;
      template<Seek = Seek::HereAndAbove> NOD()
      auto SeekUnitAux(const Many&, const Token&, Index = 0) const -> const A::Unit*;
      
      template<Seek = Seek::HereAndAbove> NOD()
      auto SeekTrait(const Token&, Index = 0)       -> Trait;
      template<Seek = Seek::HereAndAbove> NOD()
      auto SeekTrait(const Token&, Index = 0) const -> Trait;

      template<Seek = Seek::HereAndAbove> NOD()
      auto SeekTraitAux(const Many&, const Token&, Index = 0)       -> Trait;
      template<Seek = Seek::HereAndAbove> NOD()
      auto SeekTraitAux(const Many&, const Token&, Index = 0) const -> Trait;

      template<Seek = Seek::HereAndAbove>
      bool SeekValue(const Token&, CT::Data auto&, Index = 0) const;
      template<Seek = Seek::HereAndAbove>
      bool SeekValueAux(const Token&, const Many&, CT::Data auto&, Index = 0) const;


      template<Seek = Seek::HereAndAbove> NOD()
      auto GatherUnits(const Token&)       -> TMany<A::Unit*>;
      template<Seek = Seek::HereAndAbove> NOD()
      auto GatherUnits(const Token&) const -> TMany<const A::Unit*>;
      
      template<Seek = Seek::HereAndAbove> NOD()
      auto GatherTraits(const Token&)       -> TraitList;
      template<Seek = Seek::HereAndAbove> NOD()
      auto GatherTraits(const Token&) const -> TraitList;
   #endif
   };


   ///                                                                        
   ///   A hierarchy interface                                                
   ///                                                                        
   /// Simply a container of Things, with various quality-of-life             
   /// functions related to hierarchical retrieval of things, units & traits  
   ///                                                                        
   struct Hierarchy : TMany<Thing*>, SeekInterface<Hierarchy> {
      using Base = TMany<Thing*>;
      LANGULUS_BASES(Base);

      using Base::TMany;
      using Base::operator =;
      using Base::operator ==;

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

      template<Seek = Seek::HereAndAbove> NOD()
      auto SeekUnit(DMeta, Index = 0) -> A::Unit*;
      template<Seek = Seek::HereAndAbove> NOD()
      auto SeekUnitAux(const Many&, DMeta, Index = 0) -> A::Unit*;
      template<Seek = Seek::HereAndAbove> NOD() 
      auto SeekUnitExt(DMeta, const Many&, Index = 0) -> A::Unit*;
      template<Seek = Seek::HereAndAbove> NOD()
      auto SeekUnitAuxExt(DMeta, const Many&, const Many&, Index = 0) -> A::Unit*;

      template<Seek = Seek::HereAndAbove> NOD()
      auto SeekTrait(TMeta, Index = 0) -> Trait;
      template<Seek = Seek::HereAndAbove> NOD()
      auto SeekTraitAux(const Many&, TMeta, Index = 0) -> Trait;

      template<Seek = Seek::HereAndAbove>
      bool SeekValue(TMeta, CT::Data auto&, Index = 0) const;
      template<Seek = Seek::HereAndAbove>
      bool SeekValueAux(TMeta, const Many&, CT::Data auto&, Index = 0) const;

      ///                                                                     
      ///   Gather                                                            
      ///                                                                     
      using SeekInterface::GatherUnits;
      using SeekInterface::GatherUnitsExt;
      using SeekInterface::GatherTraits;

      template<Seek = Seek::HereAndAbove> NOD()
      auto GatherUnitsExt(DMeta, const Many&) -> TMany<A::Unit*>;
      template<Seek = Seek::HereAndAbove> NOD()
      auto GatherUnits(DMeta) -> TMany<A::Unit*>;

      template<Seek = Seek::HereAndAbove> NOD()
      auto GatherTraits(TMeta) -> TraitList;

      template<CT::Data D, Seek = Seek::HereAndAbove> NOD()
      auto GatherValues() const -> TMany<D>;
   };

} // namespace Langulus::Entity
