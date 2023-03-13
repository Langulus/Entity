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

   /// Collects all units of the given type inside the hierarchy              
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @param meta - the units to seek for                                  
   ///   @return the gathered units that match the type                       
   template<Seek SEEK>
   LANGULUS(ALWAYSINLINE)
   TAny<Unit*> Hierarchy::GatherUnits(DMeta meta) {
      TAny<Unit*> result;
      for (auto owner : *this)
         result += owner->template GatherUnits<SEEK>(meta);
      return Abandon(result);
   }

   /// Collects all units of the given type inside the hierarchy              
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @param meta - the units to seek for                                  
   ///   @return the gathered units that match the type                       
   template<Seek SEEK>
   LANGULUS(ALWAYSINLINE)
   TAny<const Unit*> Hierarchy::GatherUnits(DMeta meta) const {
      TAny<const Unit*> result;
      for (auto owner : *this)
         result += owner->template GatherUnits<SEEK>(meta);
      return Abandon(result);
   }
      
   /// Collects all traits of the given type inside the hierarchy             
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @param trait - the trait to seek for                                 
   ///   @return the gathered traits that match the type                      
   template<Seek SEEK>
   LANGULUS(ALWAYSINLINE)
   TAny<Trait> Hierarchy::GatherTraits(TMeta trait) {
      TAny<Trait> result;
      for (auto owner : *this)
         result += owner->template GatherTraits<SEEK>(trait);
      return Abandon(result);
   }

   /// Collects all traits of the given type inside the hierarchy             
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @param trait - the trait to seek for                                 
   ///   @return the gathered traits that match the type                      
   template<Seek SEEK>
   LANGULUS(ALWAYSINLINE)
   TAny<Trait> Hierarchy::GatherTraits(TMeta trait) const {
      TAny<Trait> result;
      for (auto owner : *this)
         result += owner->template GatherTraits<SEEK>(trait);
      return Abandon(result);
   }

   /// Gather all values convertible to a type                                
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @tparam D - type to convert to                                       
   ///   @return the gathered values                                          
   template<Seek SEEK, CT::Data D>
   LANGULUS(ALWAYSINLINE)
   TAny<D> Hierarchy::GatherValues() const {
      TAny<D> result;
      for (auto owner : *this)
         result += owner->template GatherValues<SEEK, D>();
      return Abandon(result);
   }

} // namespace Langulus::Entity