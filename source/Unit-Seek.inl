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
   
#if LANGULUS_FEATURE(MANAGED_MEMORY)
   /// Find a specific unit, searching into the hierarchy                     
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @param token - the unit token to seek for                            
   ///   @param offset - which of the matches to return                       
   ///   @return the found unit, or nullptr if no such unit was found         
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Unit* Unit::SeekUnit(const Token& token, const Index& offset) {
      return mOwners.template SeekUnit<SEEK>(token, offset);
   }

   /// Find a unit by type and index from the hierarchy                       
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param token - the unit token to seek for                            
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   const Unit* Unit::SeekUnit(const Token& token, const Index& offset) const {
      return mOwners.template SeekUnit<SEEK>(token, offset);
   }

   /// Seek a unit inside descriptor first, and then the hierarchy            
   ///   @tparam SEEK - the direction we're seeking in the hierarchy          
   ///   @param descriptor - descriptor to search through                     
   ///   @param token - the unit token to seek for                            
   ///   @param offset - the index of the unit to return                      
   ///   @return a valid unit pointer, if unit was found                      
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   const Unit* Unit::SeekUnit(const Any& descriptor, const Token& token, Index offset) const {
      return mOwners.template SeekUnit<SEEK>(descriptor, token, offset);
   }
#endif
   
   /// Find a specific unit, searching into the hierarchy                     
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @param meta - the unit to seek for                                   
   ///   @param offset - which of the matches to return                       
   ///   @return the found unit, or nullptr if no such unit was found         
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Unit* Unit::SeekUnit(DMeta meta, const Index& offset) {
      return mOwners.template SeekUnit<SEEK>(meta, offset);
   }

   /// Find a unit by type and index from the hierarchy                       
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param unit - the type of the unit to seek for                       
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   const Unit* Unit::SeekUnit(DMeta unit, const Index& offset) const {
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
   const Unit* Unit::SeekUnit(const Any& descriptor, DMeta unit, const Index& offset) const {
      return mOwners.template SeekUnit<SEEK>(descriptor, unit, offset);
   }

   /// Find a unit by index and static type from the hierarchy                
   ///   @tparam T - static unit type to search for                           
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<CT::Data T, SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Decay<T>* Unit::SeekUnit(const Index& offset) {
      return mOwners.template SeekUnit<T, SEEK>(offset);
   }

   /// Find a unit by index and static type from the hierarchy (const)        
   ///   @tparam T - static unit type to search for                           
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<CT::Data T, SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   const Decay<T>* Unit::SeekUnit(const Index& offset) const {
      return mOwners.template SeekUnit<T, SEEK>(offset);
   }
   
   /// Find a unit by index and static type from the hierarchy (const)        
   ///   @tparam T - static unit type to search for                           
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param descriptor - descriptor to search through                     
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<CT::Data T, SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   const Decay<T>* Unit::SeekUnit(const Any& descriptor, const Index& offset) const {
      return mOwners.template SeekUnit<T, SEEK>(descriptor, offset);
   }
   
#if LANGULUS_FEATURE(MANAGED_MEMORY)
   /// Find a trait by token (and index) from the hierarchy                   
   ///   @tparam SEEK - where to seek for the trait                           
   ///   @param token - the trait identifier                                  
   ///   @param index - the index of the unit to return                       
   ///   @return the trait (empty if not found)                               
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Trait Unit::SeekTrait(const Token& token, const Index& index) {
      return mOwners.template SeekTrait<SEEK>(token, index);
   }

   /// Find a trait by token (and index) from the hierarchy (const)           
   ///   @tparam SEEK - where to seek for the trait                           
   ///   @param token - the trait identifier                                  
   ///   @param index - the index of the unit to return                       
   ///   @return the trait (empty if not found)                               
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Trait Unit::SeekTrait(const Token& token, const Index& index) const {
      return mOwners.template SeekTrait<SEEK>(token, index);
   }

   /// Find a trait by token (and index) from the hierarchy, and attempt      
   /// converting it to a desired output type                                 
   /// Supports pinnable outputs                                              
   ///   @tparam SEEK - where to seek for the trait                           
   ///   @tparam D - type of output (deducible)                               
   ///   @param token - the trait identifier                                  
   ///   @param output - [out] the output                                     
   ///   @param index - the index of the unit to return                       
   ///   @return true if output was rewritten                                 
   template<SeekStyle SEEK, CT::Data D>
   LANGULUS(ALWAYSINLINE)
   bool Unit::SeekTrait(const Token& token, D& output, const Index& index) const {
      return mOwners.template SeekTrait<SEEK, D>(token, output, index);
   }

   /// Find a trait by token (and index) from the hierarchy                   
   ///   @tparam SEEK - where to seek for the trait                           
   ///   @param descriptor - local descriptor to search through first         
   ///   @param token - the trait identifier                                  
   ///   @param index - the index of the unit to return                       
   ///   @return the trait (empty if not found)                               
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Trait Unit::SeekTrait(const Any& descriptor, const Token& token, const Index& index) const {
      return mOwners.template SeekTrait<SEEK>(descriptor, token, index);
   }

   /// Find a trait by token (and index) from a local descriptor, and then    
   /// the hierarchy, and attempt converting it to a desired output type      
   /// Supports pinnable outputs                                              
   ///   @tparam SEEK - where to seek for the trait                           
   ///   @tparam D - type of output (deducible)                               
   ///   @param descriptor - local descriptor to search through first         
   ///   @param token - the trait identifier                                  
   ///   @param output - [out] the output                                     
   ///   @param index - the index of the unit to return                       
   ///   @return true if output was rewritten                                 
   template<SeekStyle SEEK, CT::Data D>
   LANGULUS(ALWAYSINLINE)
   bool Unit::SeekTrait(const Any& descriptor, const Token& token, D& output, const Index& index) const {
      return mOwners.template SeekTrait<SEEK, D>(descriptor, token, output, index);
   }
#endif

   /// Find a trait by type (and index), searching into the hierarchy         
   ///   @tparam SEEK - direction to search at                                
   ///   @param trait - the trait to search for                               
   ///   @param offset - the offset to apply                                  
   ///   @return the trait, which is not empty, if trait was found            
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Trait Unit::SeekTrait(TMeta trait, const Index& offset) {
      return mOwners.template SeekTrait<SEEK>(trait, offset);
   }

   /// Find a trait, searching into the hierarchy (const)                     
   ///   @tparam SEEK - direction to search at                                
   ///   @param trait - the trait type to search for                          
   ///   @param offset - the number of the matching trait to use              
   ///   @return the trait, which is not empty, if trait was found            
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Trait Unit::SeekTrait(TMeta trait, const Index& offset) const {
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
   bool Unit::SeekTrait(TMeta trait, D& output, const Index& offset) const {
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
   Trait Unit::SeekTrait(const Any& descriptor, TMeta trait, const Index& offset) const {
      return mOwners.template SeekTrait<SEEK>(descriptor, trait, offset);
   }

   /// Find a trait, searching into the hierarchy                             
   ///   @tparam T - the trait to search for                                  
   ///   @tparam SEEK - direction to search at                                
   ///   @param offset - the number of the matching trait to use              
   ///   @return the trait, which is not empty, if trait was found            
   template<CT::Trait T, SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Trait Unit::SeekTrait(const Index& offset) {
      return mOwners.template SeekTrait<T, SEEK>(offset);
   }

   /// Find a trait, searching into the hierarchy (const)                     
   ///   @tparam T - the trait to search for                                  
   ///   @tparam SEEK - direction to search at                                
   ///   @param offset - the number of the matching trait to use              
   ///   @return the trait, which is not empty, if trait was found            
   template<CT::Trait T, SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Trait Unit::SeekTrait(const Index& offset) const {
      return mOwners.template SeekTrait<T, SEEK>(offset);
   }
   
   /// Find a trait, searching into the hierarchy (const)                     
   ///   @tparam T - the trait to search for                                  
   ///   @tparam SEEK - direction to search at                                
   ///   @param descriptor - descriptor to search through first               
   ///   @param offset - the number of the matching trait to use              
   ///   @return the trait, which is not empty, if trait was found            
   template<CT::Trait T, SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Trait Unit::SeekTrait(const Any& descriptor, const Index& offset) const {
      return mOwners.template SeekTrait<T, SEEK>(descriptor, offset);
   }

   /// Seek a value from static/dynamic/unit traits in the hierarchy          
   ///   @tparam SEEK - the direction we're searching the hierarchy in        
   ///   @tparam D - the type of value we're searching for (deducible)        
   ///   @param output - [out] the found value will be written here           
   ///   @param offset - the number of the matching trait to use              
   ///   @return true if value has been found and rewritten                   
   template<SeekStyle SEEK, CT::Data D>
   LANGULUS(ALWAYSINLINE)
   bool Unit::SeekValue(D& output, const Index& offset) const {
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
   bool Unit::SeekValue(const Any& descriptor, D& value, const Index& offset) const {
      return mOwners.template SeekValue<SEEK, D>(descriptor, value, offset);
   }

} // namespace Langulus::Entity