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
#include "Thing.hpp"
#include "Unit.hpp"
#include "Pin.inl"

#define TEMPLATE()   template<class THIS>
#define TME()        SeekInterface<THIS>


namespace Langulus::Entity
{

   /// Collects all units of the given type inside the hierarchy              
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @param meta - the units to seek for                                  
   ///   @return the gathered units that match the type                       
   TEMPLATE() template<Seek SEEK> LANGULUS(INLINED)
   TMany<const A::Unit*> TME()::GatherUnits(DMeta meta) const {
      return const_cast<THIS*>(static_cast<const THIS*>(this))
         ->template GatherUnits<SEEK>(meta);
   }

   TEMPLATE() template<CT::Data T, Seek SEEK> LANGULUS(INLINED)
   TMany<T*> TME()::GatherUnits() {
      return static_cast<THIS*>(this)
         ->template GatherUnits<SEEK>(MetaDataOf<Decay<T>>());
   }

   TEMPLATE() template<CT::Data T, Seek SEEK> LANGULUS(INLINED)
   TMany<const T*> TME()::GatherUnits() const {
      return const_cast<TME()*>(this)
         ->template GatherUnits<T, SEEK>();
   }

   /// Collects all traits of the given type inside the hierarchy             
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @param trait - the trait to seek for                                 
   ///   @return the gathered traits that match the type                      
   TEMPLATE() template<Seek SEEK> LANGULUS(INLINED)
   TraitList TME()::GatherTraits(TMeta trait) const {
      return const_cast<THIS*>(static_cast<const THIS*>(this))
         ->template GatherTraits<SEEK>(trait);
   }

   TEMPLATE() template<CT::Trait T, Seek SEEK> LANGULUS(INLINED)
   TraitList TME()::GatherTraits() {
      return static_cast<THIS*>(this)
         ->template GatherTraits<SEEK>(T::GetTrait());
   }

   TEMPLATE() template<CT::Trait T, Seek SEEK> LANGULUS(INLINED)
   TraitList TME()::GatherTraits() const {
      return const_cast<TME()*>(this)
         ->template GatherTraits<T, SEEK>();
   }

   #if LANGULUS_FEATURE(MANAGED_REFLECTION)
      TEMPLATE() template<Seek SEEK> LANGULUS(INLINED)
      TMany<A::Unit*> TME()::GatherUnits(const Token& token) {
         return static_cast<THIS*>(this)
            ->template GatherUnits<SEEK>(RTTI::GetMetaData(token));
      }

      TEMPLATE() template<Seek SEEK> LANGULUS(INLINED)
      TMany<const A::Unit*> TME()::GatherUnits(const Token& token) const {
         return static_cast<THIS*>(this)
            ->template GatherUnits<SEEK>(RTTI::GetMetaData(token));
      }
      
      TEMPLATE() template<Seek SEEK> LANGULUS(INLINED)
      TraitList TME()::GatherTraits(const Token& token) {
         return static_cast<THIS*>(this)
            ->template GatherTraits<SEEK>(RTTI::GetMetaTrait(token));
      }

      TEMPLATE() template<Seek SEEK> LANGULUS(INLINED)
      TraitList TME()::GatherTraits(const Token& token) const {
         return static_cast<THIS*>(this)
            ->template GatherTraits<SEEK>(RTTI::GetMetaTrait(token));
      }
   #endif

} // namespace Langulus::Entity

#undef TEMPLATE
#undef TME

namespace Langulus::Entity
{

   /// Collects all units of the given type inside the hierarchy              
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @param meta - the units to seek for                                  
   ///   @return the gathered units that match the type                       
   template<Seek SEEK> LANGULUS(INLINED)
   TMany<A::Unit*> Hierarchy::GatherUnits(DMeta meta) {
      TMany<A::Unit*> result;
      for (auto owner : *this)
         result += owner->template GatherUnits<SEEK>(meta);
      return Abandon(result);
   }
      
   /// Collects all traits of the given type inside the hierarchy             
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @param trait - the trait to seek for                                 
   ///   @return the gathered traits that match the type                      
   template<Seek SEEK> LANGULUS(INLINED)
   TMany<Trait> Hierarchy::GatherTraits(TMeta trait) {
      TMany<Trait> result;
      for (auto owner : *this)
         result += owner->template GatherTraits<SEEK>(trait);
      return Abandon(result);
   }

   /// Gather all values convertible to a type                                
   ///   @tparam D - type to convert to                                       
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @return the gathered values                                          
   template<CT::Data D, Seek SEEK> LANGULUS(INLINED)
   TMany<D> Hierarchy::GatherValues() const {
      TMany<D> result;
      for (auto owner : *this)
         result += owner->template GatherValues<D, SEEK>();
      return Abandon(result);
   }

} // namespace Langulus::Entity