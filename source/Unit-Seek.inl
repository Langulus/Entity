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


namespace Langulus::Entity
{
      
   /// Find a specific unit, searching into the hierarchy                     
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @param meta - the unit to seek for                                   
   ///   @param offset - which of the matches to return                       
   ///   @return the found unit, or nullptr if no such unit was found         
   template<Seek SEEK> LANGULUS(INLINED)
   Unit* Unit::SeekUnit(DMeta meta, Index offset) {
      return mOwners.template SeekUnit<SEEK>(meta, offset);
   }

   /// Find a unit by type and index from the hierarchy                       
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param aux - descriptor to search through                            
   ///   @param meta - the type of the unit to seek for                       
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<Seek SEEK> LANGULUS(INLINED)
   Unit* Unit::SeekUnitAux(const Neat& aux, DMeta meta, Index offset) {
      return mOwners.template SeekUnitAux<SEEK>(aux, meta, offset);
   }
      
   /// Seek a unit with specific properties                                   
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param type - the type of the unit to seek for                       
   ///   @param ext - the properties of the unit to seek for                  
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<Seek SEEK> LANGULUS(INLINED)
   Unit* Unit::SeekUnitExt(DMeta type, const Neat& ext, Index offset) {
      return mOwners.template SeekUnitExt<SEEK>(type, ext, offset);
   }

   /// Seek a unit with specific properties                                   
   /// Scan an auxiliary descriptor first                                     
   ///   @tparam SEEK - the direction in which we're scanning the hierarchy   
   ///   @param type - the type of the unit to seek for                       
   ///   @param aux - local descriptor to seek through first                  
   ///   @param ext - the unit properties to seek for                         
   ///   @param offset - the index of the unit to return                      
   ///   @return a pointer to the found unit, or nullptr if not found         
   template<Seek SEEK> LANGULUS(INLINED)
   Unit* Unit::SeekUnitAuxExt(DMeta type, const Neat& aux, const Neat& ext, Index offset) {
      return mOwners.template SeekUnitAuxExt<SEEK>(type, aux, ext, offset);
   }

   /// Find a trait by type (and index), searching into the hierarchy         
   ///   @tparam SEEK - direction to search at                                
   ///   @param meta - the trait to search for                                
   ///   @param offset - the offset to apply                                  
   ///   @return the trait, which is not empty, if trait was found            
   template<Seek SEEK> LANGULUS(INLINED)
   Trait Unit::SeekTrait(TMeta meta, Index offset) {
      return mOwners.template SeekTrait<SEEK>(meta, offset);
   }
   
   /// Find a trait, searching into the hierarchy (const)                     
   ///   @tparam SEEK - direction to search at                                
   ///   @param aux - descriptor to search through                            
   ///   @param meta - the trait type to search for                           
   ///   @param offset - the number of the matching trait to use              
   ///   @return the trait, which is not empty, if trait was found            
   template<Seek SEEK> LANGULUS(INLINED)
   Trait Unit::SeekTraitAux(const Neat& aux, TMeta meta, Index offset) {
      return mOwners.template SeekTraitAux<SEEK>(aux, meta, offset);
   }
    
   /// Find a trait by type (and index) from the hierarchy, and attempt       
   /// converting it to a desired output type                                 
   /// Supports pinnable outputs, and won't change anything if D is pinned    
   ///   @tparam SEEK - direction to search at                                
   ///   @param meta - the trait type to search for                           
   ///   @param output - [out] the output                                     
   ///   @param offset - the number of the matching trait to use              
   ///   @return true if output was rewritten                                 
   template<Seek SEEK> LANGULUS(INLINED)
   bool Unit::SeekValue(TMeta meta, CT::Data auto& output, Index offset) const {
      return mOwners.template SeekValue<SEEK>(meta, output, offset);
   }
  
   /// Find a trait by type (and index) from the hierarchy, and attempt       
   /// converting it to a desired output type. Scan the aux container first   
   /// Supports pinnable outputs, and pinnables will be pinned if trait was   
   /// found in the aux container.                                            
   ///   @tparam SEEK - direction to search at                                
   ///   @param aux - descriptor to search through                            
   ///   @param meta - the trait type to search for                           
   ///   @param output - [out] the output                                     
   ///   @param offset - the number of the matching trait to use              
   ///   @return the trait, which is not empty, if trait was found            
   template<Seek SEEK> LANGULUS(INLINED)
   bool Unit::SeekValueAux(TMeta meta, const Neat& aux, CT::Data auto& output, Index offset) const {
      return mOwners.template SeekValueAux<SEEK>(meta, aux, output, offset);
   }

} // namespace Langulus::Entity