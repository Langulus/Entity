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
      
   /// Find a specific unit, searching into the hierarchy                     
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @param meta - the unit to seek for                                   
   ///   @param offset - which of the matches to return                       
   ///   @return the found unit, or nullptr if no such unit was found         
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Unit* Hierarchy::SeekUnit(DMeta meta, Index offset) {
      for (auto owner : *this) {
         Unit* result = owner->template SeekUnit<SEEK>(meta, offset);
         if (result)
            return result;
      }

      return nullptr;
   }

   /// Find a unit by type and index from the hierarchy                       
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param meta - the type of the unit to seek for                       
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   const Unit* Hierarchy::SeekUnit(DMeta meta, Index offset) const {
      return const_cast<Hierarchy*>(this)->template
         SeekUnit<SEEK>(meta, offset);
   }

   /// Find a unit by type and index from the hierarchy                       
   /// Scan a locally provided descriptor first                               
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param descriptor - descriptor to search through                     
   ///   @param meta - the type of the unit to seek for                       
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Unit* Hierarchy::SeekUnit(const Any& descriptor, DMeta meta, Index offset) {
      // Scan descriptor even if hierarchy is empty                     
      Unit* result {};
      descriptor.ForEachDeep([&](const Unit* unit) {
         if (unit->CastsTo(meta)) {
            // Found match                                              
            if (offset == 0) {
               // We're done                                            
               result = const_cast<Unit*>(unit);
               return false;
            }
            else --offset;
         }
         return true;
      });

      if (result)
         return result;

      // If reached, then no unit was found in the descriptor           
      // Let's delve into the hierarchy                                 
      return SeekUnit<SEEK>(meta, offset);
   }
   
   /// Find a unit by type and index from the hierarchy (const)               
   /// Scan a locally provided descriptor first                               
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param descriptor - descriptor to search through                     
   ///   @param unit - the type of the unit to seek for                       
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   const Unit* Hierarchy::SeekUnit(const Any& descriptor, DMeta unit, Index offset) const {
      return const_cast<Hierarchy*>(this)->template
         SeekUnit<SEEK>(descriptor, unit, offset);
   }

   /// Seek a unit with specific properties                                   
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param construct - the type and properties of the unit to seek for   
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Unit* Hierarchy::SeekUnitExt(const Construct& construct, Index offset) {
      for (auto owner : *this) {
         Unit* result = owner->template SeekUnitExt<SEEK>(construct, offset);
         if (result)
            return result;
      }

      return nullptr;
   }

   /// Seek a unit with specific properties (const)                           
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param construct - the type and properties of the unit to seek for   
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   const Unit* Hierarchy::SeekUnitExt(const Construct& construct, Index offset) const {
      return const_cast<Hierarchy*>(this)->template
         SeekUnitExt<SEEK>(construct, offset);
   }

   /// Seek a unit with specific properties                                   
   /// Scan a locally provided descriptor first                               
   ///   @tparam SEEK - the direction in which we're scanning the hierarchy   
   ///   @param descriptor - local descriptor to seek through first           
   ///   @param construct - the unit type and properties to seek for          
   ///   @param offset - the Nth match to return                              
   ///   @return a pointer to the found unit, or nullptr if not found         
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Unit* Hierarchy::SeekUnitExt(const Any& descriptor, const Construct& construct, Index offset) {
      // Scan descriptor even if hierarchy is empty                     
      Unit* result {};
      descriptor.ForEachDeep([&](const Unit* u) {
         if (u->CastsTo(construct.GetType())) {
            //TODO check construct arguments
            // Found match                                              
            if (offset == 0) {
               // We're done                                            
               result = const_cast<Unit*>(u);
               return false;
            }
            else --offset;
         }
         return true;
      });

      if (result)
         return result;

      // If reached, then no unit was found in the descriptor           
      // Let's delve into the hierarchy                                 
      return SeekUnitExt<SEEK>(construct, offset);
   }

   /// Seek a unit with specific properties (const)                           
   /// Scan a locally provided descriptor first                               
   ///   @tparam SEEK - the direction in which we're scanning the hierarchy   
   ///   @param descriptor - local descriptor to seek through first           
   ///   @param construct - the unit type and properties to seek for          
   ///   @param offset - the Nth match to return                              
   ///   @return a pointer to the found unit, or nullptr if not found         
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   const Unit* Hierarchy::SeekUnitExt(const Any& descriptor, const Construct& construct, Index offset) const {
      return const_cast<Hierarchy*>(this)->template
         SeekUnitExt<SEEK>(descriptor, construct, offset);
   }
   
   /// Find a trait by type (and index), searching into the hierarchy         
   ///   @tparam SEEK - direction to search at                                
   ///   @param trait - the trait to search for                               
   ///   @param offset - the offset to apply                                  
   ///   @return the trait, which is not empty, if trait was found            
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Trait Hierarchy::SeekTrait(TMeta trait, Index offset) {
      for (auto owner : *this) {
         auto result = owner->template SeekTrait<SEEK>(trait, offset);
         if (!result.IsEmpty())
            return result;
      }

      return {};
   }

   /// Find a trait, searching into the hierarchy (const)                     
   ///   @tparam SEEK - direction to search at                                
   ///   @param trait - the trait type to search for                          
   ///   @param offset - the number of the matching trait to use              
   ///   @return the trait, which is not empty, if trait was found            
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Trait Hierarchy::SeekTrait(TMeta trait, Index offset) const {
      Trait result = const_cast<Hierarchy*>(this)->template
         SeekTrait<SEEK>(trait, offset);
      result.MakeConst();
      return Abandon(result);
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
   bool Hierarchy::SeekTrait(TMeta trait, D& output, Index offset) const {
      if constexpr (CT::Pinnable<D>) {
         // Never touch pinned values                                   
         if (output.mLocked)
            return false;
      }

      // Let's delve into the hierarchy                                 
      for (auto owner : *this) {
         if (owner->template SeekTrait<SEEK, D>(trait, output, offset)) {
            // Value was found                                          
            return true;
         }
      }

      // If reached, nothing was found                                  
      return false;
   }
   
   /// Find a trait, searching into the hierarchy (const)                     
   ///   @tparam SEEK - direction to search at                                
   ///   @param descriptor - descriptor to search through                     
   ///   @param trait - the trait type to search for                          
   ///   @param offset - the number of the matching trait to use              
   ///   @return the trait, which is not empty, if trait was found            
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Trait Hierarchy::SeekTrait(const Any& descriptor, TMeta meta, Index offset) const {
      // Scan descriptor                                                
      Trait result;
      descriptor.ForEachDeep([&](const Trait& trait) {
         if (trait.TraitIs(meta)) {
            if (offset == 0) {
               // Match found                                           
               result = trait;
               return false;
            }
            
            --offset;
         }

         return true;         // Just keep searching...                 
      });

      if (!result.IsEmpty())
         return Abandon(result);

      // If reached, then no trait was found in the descriptor          
      // Let's delve into the hierarchy                                 
      return SeekTrait<SEEK>(meta, offset);
   }

   /// Seek a value from static/dynamic/unit traits in the hierarchy          
   ///   @tparam SEEK - the direction we're searching the hierarchy in        
   ///   @tparam D - the type of value we're searching for (deducible)        
   ///   @param output - [out] the found value will be written here           
   ///   @param offset - the number of the matching trait to use              
   ///   @return true if value has been found and rewritten                   
   template<SeekStyle SEEK, CT::Data D>
   LANGULUS(ALWAYSINLINE)
   bool Hierarchy::SeekValue(D& output, Index offset) const {
      if constexpr (CT::Pinnable<D>) {
         // Never touch pinned values                                   
         if (output.mLocked)
            return false;
      }

      // Let's delve into the hierarchy                                 
      for (auto owner : *this) {
         if (owner->template SeekValue<SEEK, D>(output, offset)) {
            // Value was found                                          
            return true;
         }
      }

      // If reached, nothing was found                                  
      return false;
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
   bool Hierarchy::SeekValue(const Any& descriptor, D& output, Index offset) const {
      if constexpr (CT::Pinnable<D>) {
         // Never touch pinned values                                   
         if (output.mLocked)
            return false;
      }

      // Scan descriptor                                                
      bool done = false;
      descriptor.ForEachDeep([&](const Block& group) {
         try {
            // Found match if these don't throw                         
            output = group.template AsCast<D>();
            done = true;
            return false;
         }
         catch(...) { }
         return true;
      });

      if (done) {
         // Data was found in the descriptor                            
         if constexpr (CT::Pinnable<D>) {
            // Make sure to pin the pinnable value                      
            output.mLocked = true;
         }

         return true;
      }

      // If reached, then no data was found in the descriptor           
      // Let's delve into the hierarchy                                 
      return SeekValue<SEEK, D>(output, offset);
   }

} // namespace Langulus::Entity