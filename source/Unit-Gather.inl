///                                                                           
/// Langulus::Entity                                                          
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Unit.hpp"

namespace Langulus::Entity
{
   
   /// Collects all units of the given type inside the hierarchy              
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @param meta - the units to seek for                                  
   ///   @return the gathered units that match the type                       
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   TAny<Unit*> Unit::GatherUnits(DMeta meta) {
      return mOwners.template GatherUnits<SEEK>(meta);
   }

   /// Collects all units of the given type inside the hierarchy              
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @param meta - the units to seek for                                  
   ///   @return the gathered units that match the type                       
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   TAny<const Unit*> Unit::GatherUnits(DMeta meta) const {
      return mOwners.template GatherUnits<SEEK>(meta);
   }
   
   /// Collects all traits of the given type inside the hierarchy             
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @param trait - the trait to seek for                                 
   ///   @return the gathered traits that match the type                      
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   TAny<Trait> Unit::GatherTraits(TMeta trait) {
      return mOwners.template GatherTraits<SEEK>(trait);
   }

   /// Collects all traits of the given type inside the hierarchy             
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @param trait - the trait to seek for                                 
   ///   @return the gathered traits that match the type                      
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   TAny<Trait> Unit::GatherTraits(TMeta trait) const {
      return mOwners.template GatherTraits<SEEK>(trait);
   }

   /// Gather all values convertible to a type                                
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @tparam D - type to convert to                                       
   ///   @return the gathered values                                          
   template<SeekStyle SEEK, CT::Data D>
   LANGULUS(ALWAYSINLINE)
   TAny<D> Unit::GatherValues() const {
      return mOwners.template GatherValues<SEEK, D>();
   }

} // namespace Langulus::Entity