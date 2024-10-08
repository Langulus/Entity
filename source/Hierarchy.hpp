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
      ///                                                                     
      template<Seek = Seek::HereAndAbove>
      NOD() const A::Unit*  SeekUnit(DMeta, Index = 0) const;
      template<CT::Data T = A::Unit, Seek = Seek::HereAndAbove>
      NOD()       Decay<T>* SeekUnit(Index = 0);
      template<CT::Data T = A::Unit, Seek = Seek::HereAndAbove>
      NOD() const Decay<T>* SeekUnit(Index = 0) const;

      template<Seek = Seek::HereAndAbove>
      NOD() const A::Unit*  SeekUnitAux(const Neat&, DMeta, Index = 0) const;
      template<CT::Data T = A::Unit, Seek = Seek::HereAndAbove>
      NOD()       Decay<T>* SeekUnitAux(const Neat&, Index = 0);
      template<CT::Data T = A::Unit, Seek = Seek::HereAndAbove>
      NOD() const Decay<T>* SeekUnitAux(const Neat&, Index = 0) const;

      template<Seek = Seek::HereAndAbove>
      NOD() const A::Unit*  SeekUnitExt(DMeta, const Neat&, Index = 0) const;
      template<CT::Data T = A::Unit, Seek = Seek::HereAndAbove>
      NOD()       Decay<T>* SeekUnitExt(const Neat&, Index = 0);
      template<CT::Data T = A::Unit, Seek = Seek::HereAndAbove>
      NOD() const Decay<T>* SeekUnitExt(const Neat&, Index = 0) const;

      template<Seek = Seek::HereAndAbove>
      NOD() const A::Unit*  SeekUnitAuxExt(DMeta, const Neat&, const Neat&, Index = 0) const;
      template<CT::Data T = A::Unit, Seek = Seek::HereAndAbove>
      NOD()       Decay<T>* SeekUnitAuxExt(const Neat&, const Neat&, Index = 0);
      template<CT::Data T = A::Unit, Seek = Seek::HereAndAbove>
      NOD() const Decay<T>* SeekUnitAuxExt(const Neat&, const Neat&, Index = 0) const;

      template<Seek = Seek::HereAndAbove>
      NOD() Trait           SeekTrait(TMeta, Index = 0) const;
      template<CT::Trait = Trait, Seek = Seek::HereAndAbove>
      NOD() Trait           SeekTrait(Index = 0);
      template<CT::Trait = Trait, Seek = Seek::HereAndAbove>
      NOD() Trait           SeekTrait(Index = 0) const;

      template<Seek = Seek::HereAndAbove>
      NOD() Trait SeekTraitAux(const Neat&, TMeta, Index = 0) const;
      template<CT::Trait = Trait, Seek = Seek::HereAndAbove>
      NOD() Trait SeekTraitAux(const Neat&, Index = 0);
      template<CT::Trait = Trait, Seek = Seek::HereAndAbove>
      NOD() Trait SeekTraitAux(const Neat&, Index = 0) const;


      template<CT::Trait = Trait, Seek = Seek::HereAndAbove>
      bool SeekValue(CT::NotTagged auto&, Index = IndexFirst) const;
      template<CT::Trait = Trait, Seek = Seek::HereAndAbove>
      bool SeekValueAux(const Neat&, CT::NotTagged auto&, Index = 0) const;

      template<Seek = Seek::HereAndAbove>
      bool SeekValue(CT::Tagged auto&, Index = IndexFirst) const;
      template<Seek = Seek::HereAndAbove>
      bool SeekValueAux(const Neat&, CT::Tagged auto&, Index = 0) const;


      template<Seek = Seek::HereAndAbove>
      NOD() TMany<const A::Unit*> GatherUnits(DMeta) const;
      template<CT::Data T = A::Unit, Seek = Seek::HereAndAbove>
      NOD() TMany<T*> GatherUnits();
      template<CT::Data T = A::Unit, Seek = Seek::HereAndAbove>
      NOD() TMany<const T*> GatherUnits() const;

      template<Seek = Seek::HereAndAbove>
      NOD() TMany<const A::Unit*> GatherUnitsExt(DMeta, const Neat&) const;
      template<CT::Data T = A::Unit, Seek = Seek::HereAndAbove>
      NOD() TMany<T*> GatherUnitsExt(const Neat&);
      template<CT::Data T = A::Unit, Seek = Seek::HereAndAbove>
      NOD() TMany<const T*> GatherUnitsExt(const Neat&) const;


      template<Seek = Seek::HereAndAbove>
      NOD() TraitList GatherTraits(TMeta) const;
      template<CT::Trait = Trait, Seek = Seek::HereAndAbove>
      NOD() TraitList GatherTraits();
      template<CT::Trait = Trait, Seek = Seek::HereAndAbove>
      NOD() TraitList GatherTraits() const;

      
   #if LANGULUS_FEATURE(MANAGED_REFLECTION)
      ///                                                                     
      /// Token based interface                                               
      /// Available only when managed reflection is enabled                   
      ///                                                                     
      template<Seek = Seek::HereAndAbove>
      NOD() A::Unit* SeekUnit(const Token&, Index = 0);
      template<Seek = Seek::HereAndAbove>
      NOD() const A::Unit* SeekUnit(const Token&, Index = 0) const;

      template<Seek = Seek::HereAndAbove>
      NOD() A::Unit* SeekUnitAux(const Neat&, const Token&, Index = 0);
      template<Seek = Seek::HereAndAbove>
      NOD() const A::Unit* SeekUnitAux(const Neat&, const Token&, Index = 0) const;
      
      template<Seek = Seek::HereAndAbove>
      NOD() Trait SeekTrait(const Token&, Index = 0);
      template<Seek = Seek::HereAndAbove>
      NOD() Trait SeekTrait(const Token&, Index = 0) const;

      template<Seek = Seek::HereAndAbove>
      NOD() Trait SeekTraitAux(const Neat&, const Token&, Index = 0);
      template<Seek = Seek::HereAndAbove>
      NOD() Trait SeekTraitAux(const Neat&, const Token&, Index = 0) const;

      template<Seek = Seek::HereAndAbove>
      bool SeekValue(const Token&, CT::Data auto&, Index = 0) const;
      template<Seek = Seek::HereAndAbove>
      bool SeekValueAux(const Token&, const Neat&, CT::Data auto&, Index = 0) const;


      template<Seek = Seek::HereAndAbove>
      NOD() TMany<A::Unit*> GatherUnits(const Token&);
      template<Seek = Seek::HereAndAbove>
      NOD() TMany<const A::Unit*> GatherUnits(const Token&) const;
      
      template<Seek = Seek::HereAndAbove>
      NOD() TraitList GatherTraits(const Token&);
      template<Seek = Seek::HereAndAbove>
      NOD() TraitList GatherTraits(const Token&) const;
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

      template<Seek = Seek::HereAndAbove>
      NOD() A::Unit* SeekUnit(DMeta, Index = 0);
      template<Seek = Seek::HereAndAbove>
      NOD() A::Unit* SeekUnitAux(const Neat&, DMeta, Index = 0);
      template<Seek = Seek::HereAndAbove>
      NOD() A::Unit* SeekUnitExt(DMeta, const Neat&, Index = 0);
      template<Seek = Seek::HereAndAbove>
      NOD() A::Unit* SeekUnitAuxExt(DMeta, const Neat&, const Neat&, Index = 0);

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
      NOD() TMany<A::Unit*> GatherUnitsExt(DMeta, const Neat&);
      template<Seek = Seek::HereAndAbove>
      NOD() TMany<A::Unit*> GatherUnits(DMeta);

      template<Seek = Seek::HereAndAbove>
      NOD() TraitList GatherTraits(TMeta);

      template<CT::Data D, Seek = Seek::HereAndAbove>
      NOD() TMany<D> GatherValues() const;
   };

} // namespace Langulus::Entity
