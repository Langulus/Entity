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
   template<Seek SEEK>
   LANGULUS(ALWAYSINLINE)
   Unit* Unit::SeekUnit(DMeta meta, Index offset) {
      return mOwners.template SeekUnit<SEEK>(meta, offset);
   }

   /// Find a unit by type and index from the hierarchy                       
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param meta - the type of the unit to seek for                       
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<Seek SEEK>
   LANGULUS(ALWAYSINLINE)
   const Unit* Unit::SeekUnit(DMeta meta, Index offset) const {
      return mOwners.template SeekUnit<SEEK>(meta, offset);
   }

   /// Find a unit by type and index from the hierarchy                       
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param aux - descriptor to search through                            
   ///   @param meta - the type of the unit to seek for                       
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<Seek SEEK>
   LANGULUS(ALWAYSINLINE)
   Unit* Unit::SeekUnitAux(const Any& aux, DMeta meta, Index offset) {
      return mOwners.template SeekUnitAux<SEEK>(aux, meta, offset);
   }

   /// Find a unit by type and index from the hierarchy (const)               
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param aux - descriptor to search through                            
   ///   @param unit - the type of the unit to seek for                       
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<Seek SEEK>
   LANGULUS(ALWAYSINLINE)
   const Unit* Unit::SeekUnitAux(const Any& aux, DMeta unit, Index offset) const {
      return mOwners.template SeekUnitAux<SEEK>(aux, unit, offset);
   }
      
   /// Seek a unit with specific properties                                   
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param ext - the type and properties of the unit to seek for         
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<Seek SEEK>
   LANGULUS(ALWAYSINLINE)
   Unit* Unit::SeekUnitExt(const Construct& ext, Index offset) {
      return mOwners.template SeekUnitExt<SEEK>(ext, offset);
   }

   /// Seek a unit with specific properties (const)                           
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param ext - the type and properties of the unit to seek for         
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<Seek SEEK>
   LANGULUS(ALWAYSINLINE)
   const Unit* Unit::SeekUnitExt(const Construct& ext, Index offset) const {
      return mOwners.template SeekUnitExt<SEEK>(ext, offset);
   }

   /// Seek a unit with specific properties                                   
   /// Scan an auxiliary descriptor first                                     
   ///   @tparam SEEK - the direction in which we're scanning the hierarchy   
   ///   @param aux - local descriptor to seek through first                  
   ///   @param ext - the unit type and properties to seek for                
   ///   @param offset - the Nth match to return                              
   ///   @return a pointer to the found unit, or nullptr if not found         
   template<Seek SEEK>
   LANGULUS(ALWAYSINLINE)
   Unit* Unit::SeekUnitAuxExt(const Any& aux, const Construct& ext, Index offset) {
      return mOwners.template SeekUnitAuxExt<SEEK>(aux, ext, offset);
   }

   /// Seek a unit with specific properties (const)                           
   /// Scan an auxiliary descriptor first                                     
   ///   @tparam SEEK - the direction in which we're scanning the hierarchy   
   ///   @param aux - local descriptor to seek through first                  
   ///   @param ext - the unit type and properties to seek for                
   ///   @param offset - the Nth match to return                              
   ///   @return a pointer to the found unit, or nullptr if not found         
   template<Seek SEEK>
   LANGULUS(ALWAYSINLINE)
   const Unit* Unit::SeekUnitAuxExt(const Any& aux, const Construct& ext, Index offset) const {
      return mOwners.template SeekUnitAuxExt<SEEK>(aux, ext, offset);
   }

   /// Find a trait by type (and index), searching into the hierarchy         
   ///   @tparam SEEK - direction to search at                                
   ///   @param meta - the trait to search for                                
   ///   @param offset - the offset to apply                                  
   ///   @return the trait, which is not empty, if trait was found            
   template<Seek SEEK>
   LANGULUS(ALWAYSINLINE)
   Trait Unit::SeekTrait(TMeta meta, Index offset) {
      return mOwners.template SeekTrait<SEEK>(meta, offset);
   }

   /// Find a trait, searching into the hierarchy (const)                     
   ///   @tparam SEEK - direction to search at                                
   ///   @param meta - the trait type to search for                           
   ///   @param offset - the number of the matching trait to use              
   ///   @return the trait, which is not empty, if trait was found            
   template<Seek SEEK>
   LANGULUS(ALWAYSINLINE)
   Trait Unit::SeekTrait(TMeta meta, Index offset) const {
      return mOwners.template SeekTrait<SEEK>(meta, offset);
   }
   
   /// Find a trait by type (and index) from the hierarchy, and attempt       
   /// converting it to a desired output type                                 
   /// Supports pinnable outputs, and won't change anything if D is pinned    
   ///   @tparam SEEK - direction to search at                                
   ///   @tparam D - type of output (deducible)                               
   ///   @param meta - the trait type to search for                           
   ///   @param output - [out] the output                                     
   ///   @param offset - the number of the matching trait to use              
   ///   @return true if output was rewritten                                 
   template<Seek SEEK, CT::Data D>
   LANGULUS(ALWAYSINLINE)
   bool Unit::SeekTrait(TMeta meta, D& output, Index offset) const {
      return mOwners.template SeekTrait<SEEK, D>(meta, output, offset);
   }
   
   /// Find a trait, searching into the hierarchy (const)                     
   ///   @tparam SEEK - direction to search at                                
   ///   @param aux - descriptor to search through                            
   ///   @param meta - the trait type to search for                           
   ///   @param offset - the number of the matching trait to use              
   ///   @return the trait, which is not empty, if trait was found            
   template<Seek SEEK>
   LANGULUS(ALWAYSINLINE)
   Trait Unit::SeekTraitAux(const Any& aux, TMeta meta, Index offset) const {
      return mOwners.template SeekTraitAux<SEEK>(aux, meta, offset);
   }
   
   /// Find a trait by type (and index) from the hierarchy, and attempt       
   /// converting it to a desired output type. Scan the aux container first   
   /// Supports pinnable outputs, and pinnables will be pinned if trait was   
   /// found in the aux container.                                            
   ///   @tparam SEEK - direction to search at                                
   ///   @tparam D - type of output (deducible)                               
   ///   @param aux - descriptor to search through                            
   ///   @param meta - the trait type to search for                           
   ///   @param output - [out] the output                                     
   ///   @param offset - the number of the matching trait to use              
   ///   @return the trait, which is not empty, if trait was found            
   template<Seek SEEK, CT::Data D>
   LANGULUS(ALWAYSINLINE)
   bool Unit::SeekTraitAux(const Any& aux, TMeta meta, D& output, Index offset) const {
      return mOwners.template SeekTraitAux<SEEK, D>(aux, meta, output, offset);
   }

   /// Seek a value from static/dynamic/unit traits in the hierarchy          
   /// Supports pinnable outputs, and won't change anything if D is pinned    
   ///   @tparam SEEK - the direction we're searching the hierarchy in        
   ///   @tparam D - the type of value we're searching for (deducible)        
   ///   @param output - [out] the found value will be written here           
   ///   @param offset - the number of the matching trait to use              
   ///   @return true if value has been found and rewritten                   
   template<Seek SEEK, CT::Data D>
   LANGULUS(ALWAYSINLINE)
   bool Unit::SeekValue(D& output, Index offset) const {
      return mOwners.template SeekValue<SEEK, D>(output, offset);
   }

   /// Find a value (regardless of trait) from the hierarchy, and attempt     
   /// converting it to a desired output type. Scan the aux container first   
   /// Supports pinnable outputs, and pinnables will be pinned if trait was   
   /// found in the aux container.                                            
   ///   @tparam SEEK - the direction we're searching the hierarchy in        
   ///   @tparam D - the type of value we're searching for (deducible)        
   ///   @param aux - descriptor to search through first                      
   ///   @param output - [out] the found value will be written here           
   ///   @param offset - the number of the matching trait to use              
   ///   @return true if value has been found and rewritten                   
   template<Seek SEEK, CT::Data D>
   LANGULUS(ALWAYSINLINE)
   bool Unit::SeekValueAux(const Any& aux, D& output, Index offset) const {
      return mOwners.template SeekValueAux<SEEK, D>(aux, output, offset);
   }

} // namespace Langulus::Entity