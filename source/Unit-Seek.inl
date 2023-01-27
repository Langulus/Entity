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
   LANGULUS(ALWAYSINLINE)
   Unit* Unit::SeekUnit(DMeta meta, Index offset) {
      return mOwners.template SeekUnit<SEEK>(meta, offset);
   }

   /// Find a unit by type and index from the hierarchy                       
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param unit - the type of the unit to seek for                       
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   const Unit* Unit::SeekUnit(DMeta unit, Index offset) const {
      return mOwners.template SeekUnit<SEEK>(unit, offset);
   }

   /// Find a unit by type and index from the hierarchy                       
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param descriptor - descriptor to search through                     
   ///   @param unit - the type of the unit to seek for                       
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Unit* Unit::SeekUnit(const Any& descriptor, DMeta unit, Index offset) {
      return mOwners.template SeekUnit<SEEK>(descriptor, unit, offset);
   }

   /// Find a unit by type and index from the hierarchy (const)               
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param descriptor - descriptor to search through                     
   ///   @param unit - the type of the unit to seek for                       
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   const Unit* Unit::SeekUnit(const Any& descriptor, DMeta unit, Index offset) const {
      return mOwners.template SeekUnit<SEEK>(descriptor, unit, offset);
   }
      
   /// Find a trait by type (and index), searching into the hierarchy         
   ///   @tparam SEEK - direction to search at                                
   ///   @param trait - the trait to search for                               
   ///   @param offset - the offset to apply                                  
   ///   @return the trait, which is not empty, if trait was found            
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Trait Unit::SeekTrait(TMeta trait, Index offset) {
      return mOwners.template SeekTrait<SEEK>(trait, offset);
   }

   /// Find a trait, searching into the hierarchy (const)                     
   ///   @tparam SEEK - direction to search at                                
   ///   @param trait - the trait type to search for                          
   ///   @param offset - the number of the matching trait to use              
   ///   @return the trait, which is not empty, if trait was found            
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Trait Unit::SeekTrait(TMeta trait, Index offset) const {
      return mOwners.template SeekTrait<SEEK>(trait, offset);
   }
   
   /// Find a trait by type (and index) from the hierarchy, and attempt       
   /// converting it to a desired output type                                 
   /// Supports pinnable outputs                                              
   ///   @tparam SEEK - direction to search at                                
   ///   @tparam D - type of output (deducible)                               
   ///   @param trait - the trait type to search for                          
   ///   @param output - [out] the output                                     
   ///   @param offset - the number of the matching trait to use              
   ///   @return true if output was rewritten                                 
   template<SeekStyle SEEK, CT::Data D>
   LANGULUS(ALWAYSINLINE)
   bool Unit::SeekTrait(TMeta trait, D& output, Index offset) const {
      return mOwners.template SeekTrait<SEEK, D>(trait, output, offset);
   }
   
   /// Find a trait, searching into the hierarchy (const)                     
   ///   @tparam SEEK - direction to search at                                
   ///   @param descriptor - descriptor to search through                     
   ///   @param trait - the trait type to search for                          
   ///   @param offset - the number of the matching trait to use              
   ///   @return the trait, which is not empty, if trait was found            
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Trait Unit::SeekTrait(const Any& descriptor, TMeta trait, Index offset) const {
      return mOwners.template SeekTrait<SEEK>(descriptor, trait, offset);
   }

   /// Seek a value from static/dynamic/unit traits in the hierarchy          
   ///   @tparam SEEK - the direction we're searching the hierarchy in        
   ///   @tparam D - the type of value we're searching for (deducible)        
   ///   @param output - [out] the found value will be written here           
   ///   @param offset - the number of the matching trait to use              
   ///   @return true if value has been found and rewritten                   
   template<SeekStyle SEEK, CT::Data D>
   LANGULUS(ALWAYSINLINE)
   bool Unit::SeekValue(D& output, Index offset) const {
      return mOwners.template SeekValue<SEEK, D>(output, offset);
   }

   /// Find a value (regardless of trait) from the hierarchy, and attempt     
   /// converting it to a desired output type                                 
   /// Supports pinnable outputs                                              
   ///   @tparam SEEK - the direction we're searching the hierarchy in        
   ///   @tparam D - the type of value we're searching for (deducible)        
   ///   @param descriptor - descriptor to search through first               
   ///   @param value - [out] the found value will be written here            
   ///   @param offset - the number of the matching trait to use              
   ///   @return true if value has been found and rewritten                   
   template<SeekStyle SEEK, CT::Data D>
   LANGULUS(ALWAYSINLINE)
   bool Unit::SeekValue(const Any& descriptor, D& value, Index offset) const {
      return mOwners.template SeekValue<SEEK, D>(descriptor, value, offset);
   }

} // namespace Langulus::Entity