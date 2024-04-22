///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Unit.hpp"
#include "Hierarchy-Gather.inl"


namespace Langulus::A
{
   
   /// Collects all units of the given type inside the hierarchy              
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @param meta - the units to seek for                                  
   ///   @return the gathered units that match the type                       
   template<Seek SEEK> LANGULUS(INLINED)
   TMany<Unit*> Unit::GatherUnits(DMeta meta) {
      return mOwners.template GatherUnits<SEEK>(meta);
   }
   
   /// Collects all traits of the given type inside the hierarchy             
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @param trait - the trait to seek for                                 
   ///   @return the gathered traits that match the type                      
   template<Seek SEEK> LANGULUS(INLINED)
   TraitList Unit::GatherTraits(TMeta trait) {
      return mOwners.template GatherTraits<SEEK>(trait);
   }

   /// Gather all values convertible to a type                                
   ///   @tparam D - type to convert to                                       
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @return the gathered values                                          
   template<CT::Data D, Seek SEEK> LANGULUS(INLINED)
   TMany<D> Unit::GatherValues() const {
      return mOwners.template GatherValues<SEEK, D>();
   }

} // namespace Langulus::Entity