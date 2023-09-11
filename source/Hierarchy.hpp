///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Module.hpp"
#include <Anyness/Ref.hpp>
#include <Anyness/TAny.hpp>
#include <Anyness/Trait.hpp>

namespace Langulus::Entity
{
   using TraitList = TAny<Trait>;
}

/// Seek interface for Hierarchy/Thing/Unit                                   
#define LANGULUS_SEEK_INTERFACE() \
   template<Seek = Seek::HereAndAbove> \
   NOD() Unit* SeekUnit(DMeta, Index = IndexFirst); \
   template<Seek = Seek::HereAndAbove> \
   NOD() const Unit* SeekUnit(DMeta, Index = IndexFirst) const; \
   template<Seek = Seek::HereAndAbove> \
   NOD() Unit* SeekUnitAux(const Any&, DMeta, Index = IndexFirst); \
   template<Seek = Seek::HereAndAbove> \
   NOD() const Unit* SeekUnitAux(const Any&, DMeta, Index = IndexFirst) const; \
   \
   template<Seek = Seek::HereAndAbove> \
   NOD() Unit* SeekUnitExt(const Construct&, Index = IndexFirst); \
   template<Seek = Seek::HereAndAbove> \
   NOD() const Unit* SeekUnitExt(const Construct&, Index = IndexFirst) const; \
   template<Seek = Seek::HereAndAbove> \
   NOD() Unit* SeekUnitAuxExt(const Any&, const Construct&, Index = IndexFirst); \
   template<Seek = Seek::HereAndAbove> \
   NOD() const Unit* SeekUnitAuxExt(const Any&, const Construct&, Index = IndexFirst) const; \
   \
   template<Seek = Seek::HereAndAbove> \
   NOD() Trait SeekTrait(TMeta, Index = IndexFirst); \
   template<Seek = Seek::HereAndAbove> \
   NOD() Trait SeekTrait(TMeta, Index = IndexFirst) const; \
   template<Seek = Seek::HereAndAbove, CT::Data D> \
   NOD() bool SeekTrait(TMeta, D&, Index = IndexFirst) const; \
   \
   template<Seek = Seek::HereAndAbove> \
   NOD() Trait SeekTraitAux(const Any&, TMeta, Index = IndexFirst) const; \
   template<Seek = Seek::HereAndAbove, CT::Data D> \
   NOD() bool SeekTraitAux(const Any&, TMeta, D&, Index = IndexFirst) const; \
   \
   template<Seek = Seek::HereAndAbove, CT::Data D> \
   bool SeekValue(D&, Index = IndexFirst) const; \
   template<Seek = Seek::HereAndAbove, CT::Data D> \
   bool SeekValueAux(const Any&, D&, Index = IndexFirst) const; \
   \
   template<CT::Data T, Seek SEEK = Seek::HereAndAbove> \
   NOD() LANGULUS(INLINED) \
   Decay<T>* SeekUnit(Index offset = IndexFirst) { \
      return static_cast<Decay<T>*>( \
         SeekUnit<SEEK>(MetaData::Of<Decay<T>>(), offset) \
      ); \
   } \
   template<CT::Data T, Seek SEEK = Seek::HereAndAbove> \
   NOD() LANGULUS(INLINED) \
   const Decay<T>* SeekUnit(Index offset = IndexFirst) const { \
      return static_cast<const Decay<T>*>( \
         SeekUnit<SEEK>(MetaData::Of<Decay<T>>(), offset) \
      ); \
   } \
   template<CT::Data T, Seek SEEK = Seek::HereAndAbove> \
   NOD() LANGULUS(INLINED) \
   Decay<T>* SeekUnitAux(const Any& aux, Index offset = IndexFirst) { \
      return static_cast<Decay<T>*>( \
         SeekUnitAux<SEEK>(aux, MetaData::Of<Decay<T>>(), offset) \
      ); \
   } \
   template<CT::Data T, Seek SEEK = Seek::HereAndAbove> \
   NOD() LANGULUS(INLINED) \
   const Decay<T>* SeekUnitAux(const Any& aux, Index offset = IndexFirst) const { \
      return static_cast<const Decay<T>*>( \
         SeekUnitAux<SEEK>(aux, MetaData::Of<Decay<T>>(), offset) \
      ); \
   } \
   \
   template<CT::Data T, Seek SEEK = Seek::HereAndAbove> \
   NOD() LANGULUS(INLINED) \
   Decay<T>* SeekUnitAuxExt(const Any& aux, Index offset = IndexFirst) { \
      return static_cast<Decay<T>*>( \
         SeekUnitAuxExt<SEEK>(aux, Construct::From<T>(), offset) \
      ); \
   } \
   template<CT::Data T, Seek SEEK = Seek::HereAndAbove> \
   NOD() LANGULUS(INLINED) \
   const Decay<T>* SeekUnitAuxExt(const Any& aux, Index offset = IndexFirst) const { \
      return static_cast<const Decay<T>*>( \
         SeekUnitAuxExt<SEEK>(aux, Construct::From<T>(), offset) \
      ); \
   } \
   template<CT::Data T, Seek SEEK = Seek::HereAndAbove> \
   NOD() LANGULUS(INLINED) \
   Decay<T>* SeekUnitAuxExt(const Any& aux, const Any& construct, Index offset = IndexFirst) { \
      return static_cast<Decay<T>*>( \
         SeekUnitAuxExt<SEEK>(aux, Construct::From<T>(construct), offset) \
      ); \
   } \
   template<CT::Data T, Seek SEEK = Seek::HereAndAbove> \
   NOD() LANGULUS(INLINED) \
   const Decay<T>* SeekUnitAuxExt(const Any& aux, const Any& construct, Index offset = IndexFirst) const { \
      return static_cast<const Decay<T>*>( \
         SeekUnitAuxExt<SEEK>(aux, Construct::From<T>(construct), offset) \
      ); \
   } \
   \
   template<CT::Trait T = Trait, Seek SEEK = Seek::HereAndAbove> \
   NOD() LANGULUS(INLINED) \
   Trait SeekTrait(Index offset = IndexFirst) { \
      return SeekTrait<SEEK>(T::GetTrait(), offset); \
   } \
   template<CT::Trait T = Trait, Seek SEEK = Seek::HereAndAbove> \
   NOD() LANGULUS(INLINED) \
   Trait SeekTrait(Index offset = IndexFirst) const { \
      return SeekTrait<SEEK>(T::GetTrait(), offset); \
   } \
   template<CT::Trait T = Trait, Seek SEEK = Seek::HereAndAbove, CT::Data D> \
   LANGULUS(INLINED) \
   bool SeekTrait(D& output, Index offset = IndexFirst) const { \
      return SeekTrait<SEEK, D>(T::GetTrait(), output, offset); \
   } \
   \
   template<CT::Trait T = Trait, Seek SEEK = Seek::HereAndAbove> \
   NOD() LANGULUS(INLINED) \
   Trait SeekTraitAux(const Any& aux, Index offset = IndexFirst) const { \
      return SeekTraitAux<SEEK>(aux, T::GetTrait(), offset); \
   } \
   template<CT::Trait T = Trait, Seek SEEK = Seek::HereAndAbove, CT::Data D> \
   LANGULUS(INLINED) \
   bool SeekTraitAux(const Any& aux, D& output, Index offset = IndexFirst) const { \
      return SeekTraitAux<SEEK, D>(aux, T::GetTrait(), output, offset); \
   }

#if LANGULUS_FEATURE(MANAGED_MEMORY)
   #define LANGULUS_SEEK_TOKEN_INTERFACE() \
      template<Seek SEEK = Seek::HereAndAbove> \
      NOD() LANGULUS(INLINED) \
      Unit* SeekUnit(const Token& token, Index offset = IndexFirst) { \
         return SeekUnit<SEEK>(RTTI::GetMetaData(token), offset); \
      } \
      template<Seek SEEK = Seek::HereAndAbove> \
      NOD() LANGULUS(INLINED) \
      const Unit* SeekUnit(const Token& token, Index offset = IndexFirst) const { \
         return SeekUnit<SEEK>(RTTI::GetMetaData(token), offset); \
      } \
      template<Seek SEEK = Seek::HereAndAbove> \
      NOD() LANGULUS(INLINED) \
      Unit* SeekUnitAux(const Any& aux, const Token& token, Index offset = IndexFirst) { \
         return SeekUnitAux<SEEK>(aux, RTTI::GetMetaData(token), offset); \
      } \
      template<Seek SEEK = Seek::HereAndAbove> \
      NOD() LANGULUS(INLINED) \
      const Unit* SeekUnitAux(const Any& aux, const Token& token, Index offset = IndexFirst) const { \
         return SeekUnitAux<SEEK>(aux, RTTI::GetMetaData(token), offset); \
      } \
      \
      template<Seek SEEK = Seek::HereAndAbove> \
      NOD() LANGULUS(INLINED) \
      Trait SeekTrait(const Token& token, Index offset = IndexFirst) { \
         return SeekTrait<SEEK>(RTTI::GetMetaTrait(token), offset); \
      } \
      template<Seek SEEK = Seek::HereAndAbove> \
      NOD() LANGULUS(INLINED) \
      Trait SeekTrait(const Token& token, Index offset = IndexFirst) const { \
         return SeekTrait<SEEK>(RTTI::GetMetaTrait(token), offset); \
      } \
      template<Seek SEEK = Seek::HereAndAbove, CT::Data D> \
      LANGULUS(INLINED) \
      bool SeekTrait(const Token& token, D& output, Index offset = IndexFirst) const { \
         return SeekTrait<SEEK>(RTTI::GetMetaTrait(token), output, offset); \
      } \
      \
      template<Seek SEEK = Seek::HereAndAbove> \
      NOD() LANGULUS(INLINED) \
      Trait SeekTraitAux(const Any& aux, const Token& token, Index offset = IndexFirst) const { \
         return SeekTraitAux<SEEK>(aux, RTTI::GetMetaTrait(token), offset); \
      } \
      template<Seek SEEK = Seek::HereAndAbove, CT::Data D> \
      LANGULUS(INLINED) \
      bool SeekTraitAux(const Any& aux, const Token& token, D& output, Index offset = IndexFirst) const { \
         return SeekTraitAux<SEEK>(aux, RTTI::GetMetaTrait(token), output, offset); \
      }
#else
   #define LANGULUS_SEEK_TOKEN_INTERFACE()
#endif

/// Gather interface for Hierarchy/Thing/Unit                                 
#define LANGULUS_GATHER_INTERFACE() \
   template<Seek = Seek::HereAndAbove> \
   NOD() TAny<Unit*> GatherUnits(const Construct&); \
   template<Seek = Seek::HereAndAbove> \
   NOD() TAny<const Unit*> GatherUnits(const Construct&) const; \
   template<Seek = Seek::HereAndAbove> \
   NOD() TAny<Unit*> GatherUnits(DMeta); \
   template<Seek = Seek::HereAndAbove> \
   NOD() TAny<const Unit*> GatherUnits(DMeta) const; \
   \
   template<Seek = Seek::HereAndAbove> \
   NOD() TraitList GatherTraits(TMeta); \
   template<Seek = Seek::HereAndAbove> \
   NOD() TraitList GatherTraits(TMeta) const; \
   \
   template<Seek = Seek::HereAndAbove, CT::Data D> \
   NOD() TAny<D> GatherValues() const; \
   \
   template<CT::Data T = Unit, Seek SEEK = Seek::HereAndAbove> \
   NOD() LANGULUS(INLINED) \
   TAny<Decay<T>*> GatherUnits() { \
      return GatherUnits<SEEK>(MetaData::Of<Decay<T>>()); \
   } \
   template<CT::Data T = Unit, Seek SEEK = Seek::HereAndAbove> \
   NOD() LANGULUS(INLINED) \
   TAny<const Decay<T>*> GatherUnits() const { \
      return GatherUnits<SEEK>(MetaData::Of<Decay<T>>()); \
   } \
   template<CT::Trait T = Trait, Seek SEEK = Seek::HereAndAbove> \
   NOD() LANGULUS(INLINED) \
   TraitList GatherTraits() { \
      return GatherTraits<SEEK>(T::GetTrait()); \
   } \
   template<CT::Trait T = Trait, Seek SEEK = Seek::HereAndAbove> \
   NOD() LANGULUS(INLINED) \
   TraitList GatherTraits() const { \
      return GatherTraits<SEEK>(T::GetTrait()); \
   }

#if LANGULUS_FEATURE(MANAGED_MEMORY)
   #define LANGULUS_GATHER_TOKEN_INTERFACE() \
      template<Seek SEEK = Seek::HereAndAbove> \
      NOD() LANGULUS(INLINED) \
      TAny<Unit*> GatherUnits(const Token& token) { \
         return GatherUnits<SEEK>(RTTI::GetMetaData(token)); \
      } \
      template<Seek SEEK = Seek::HereAndAbove> \
      NOD() LANGULUS(INLINED) \
      TAny<const Unit*> GatherUnits(const Token& token) const { \
         return GatherUnits<SEEK>(RTTI::GetMetaData(token)); \
      } \
      \
      template<Seek SEEK = Seek::HereAndAbove> \
      NOD() LANGULUS(INLINED) \
      TraitList GatherTraits(const Token& token) { \
         return GatherTraits<SEEK>(RTTI::GetMetaTrait(token)); \
      } \
      template<Seek SEEK = Seek::HereAndAbove> \
      NOD() LANGULUS(INLINED) \
      TraitList GatherTraits(const Token& token) const { \
         return GatherTraits<SEEK>(RTTI::GetMetaTrait(token)); \
      }
#else
   #define LANGULUS_GATHER_TOKEN_INTERFACE()
#endif

namespace Langulus::Entity
{

   ///                                                                        
   ///   A hierarchy interface                                                
   ///                                                                        
   /// Simply a container of Things, with various quality-of-life             
   /// functions related to hierarchical retrieval of things, units, traits.  
   ///                                                                        
   struct Hierarchy : TAny<Ref<Thing>> {
      using TAny::TAny;
      using TAny::operator =;

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
   };

} // namespace Langulus::Entity
