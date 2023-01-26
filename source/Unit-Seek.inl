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

   /// Find a specific unit, searching into the hierarchy                     
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @param meta - the unit to seek for                                   
   ///   @param offset - which of the matches to return                       
   ///   @return the found unit, or nullptr if no such unit was found         
   template<SeekStyle SEEK>
   Unit* Unit::SeekUnit(DMeta meta, const Index& offset) {
      return mOwners.SeekUnit<SEEK>(meta, offset);
   }

   /// Find a unit by type and index from the hierarchy                       
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param unit - the type of the unit to seek for                       
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   const Unit* Unit::SeekUnit(DMeta unit, const Index& offset) const {
      return mOwners.SeekUnit<SEEK>(meta, offset);
   }

   /// Find a unit by index and static type from the hierarchy                
   ///   @tparam T - static unit type to search for                           
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<CT::Unit T, SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Decay<T>* Unit::SeekUnit(const Index& offset) {
      return mOwners.SeekUnit<T, SEEK>(offset);
   }

   /// Find a unit by index and static type from the hierarchy (const)        
   ///   @tparam T - static unit type to search for                           
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<CT::Unit T, SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   const Decay<T>* Unit::SeekUnit(const Index& offset) const {
      return mOwners.SeekUnit<T, SEEK>(offset);
   }

   /// Find a trait, searching into the hierarchy                             
   ///   @tparam SEEK - direction to search at                                
   ///   @param var - the trait to search for                                 
   ///   @param offset - the offset to apply                                  
   ///   @return the trait, which is not empty, if trait was found            
   template<SeekStyle SEEK>
   Trait Unit::SeekTrait(TMeta trait, const Index& offset) {
      return mOwners.SeekTrait<SEEK>(trait, offset);
   }

   /// Find a trait, searching into the hierarchy (const)                     
   ///   @tparam SEEK - direction to search at                                
   ///   @param var - the trait type to search for                            
   ///   @param offset - the number of the matching trait to use              
   ///   @return the trait, which is not empty, if trait was found            
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Trait Unit::SeekTrait(TMeta trait, const Index& offset) const {
      return mOwners.SeekTrait<SEEK>(trait, offset);
   }

   /// Find a trait, searching into the hierarchy                             
   ///   @tparam T - the trait to search for                                  
   ///   @tparam SEEK - direction to search at                                
   ///   @param offset - the number of the matching trait to use              
   ///   @return the trait, which is not empty, if trait was found            
   template<CT::Trait T, SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Trait Unit::SeekTrait(const Index& offset) {
      return mOwners.SeekTrait<T, SEEK>(offset);
   }

   /// Find a trait, searching into the hierarchy (const)                     
   ///   @tparam T - the trait to search for                                  
   ///   @tparam SEEK - direction to search at                                
   ///   @param offset - the number of the matching trait to use              
   ///   @return the trait, which is not empty, if trait was found            
   template<CT::Trait T, SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Trait Unit::SeekTrait(const Index& offset) const {
      return mOwners.SeekTrait<T, SEEK>(offset);
   }

   /// Seek a value from static/dynamic/unit traits in the hierarchy          
   ///   @tparam SEEK - the direction we're searching the hierarchy in        
   ///   @tparam T - the type of value we're searching for (deducible)        
   ///   @param trait - the type of trait we're searching for                 
   ///   @param value - [out] the found values will be written here           
   ///   @return true if value has been found and rewritten                   
   template<SeekStyle SEEK, CT::Data D>
   LANGULUS(ALWAYSINLINE)
   bool Unit::SeekValue(TMeta trait, D& value, const Index& offset) const {
      return mOwners.SeekValue<SEEK, D>(trait, value, offset);
   }

   /// Seek a value from static/dynamic/unit traits in the hierarchy          
   ///   @tparam T - the type of trait we're searching for                    
   ///   @tparam SEEK - the direction we're searching the hierarchy in        
   ///   @tparam D - the type of value we're searching for (deducible)        
   ///   @param value - [out] the found values will be written here           
   ///   @return true if value has been found and rewritten                   
   template<CT::Trait T, SeekStyle SEEK, CT::Data D>
   LANGULUS(ALWAYSINLINE)
   bool Unit::SeekValue(D& value, const Index& offset) const {
      return mOwners.SeekValue<T, SEEK, D>(value, offset);
   }

} // namespace Langulus::Entity