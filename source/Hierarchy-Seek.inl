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

namespace Langulus::Entity
{
      
   /// Find a specific unit, searching into the hierarchy                     
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @param meta - the unit to seek for                                   
   ///   @param offset - which of the matches to return                       
   ///   @return the found unit, or nullptr if no such unit was found         
   template<Seek SEEK>
   LANGULUS(INLINED)
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
   template<Seek SEEK>
   LANGULUS(INLINED)
   const Unit* Hierarchy::SeekUnit(DMeta meta, Index offset) const {
      return const_cast<Hierarchy*>(this)->template
         SeekUnit<SEEK>(meta, offset);
   }

   /// Find a unit by type and index from the hierarchy                       
   /// Scan a locally provided descriptor first                               
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param aux - descriptor to search through                            
   ///   @param meta - the type of the unit to seek for                       
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<Seek SEEK>
   LANGULUS(INLINED)
   Unit* Hierarchy::SeekUnitAux(const Any& aux, DMeta meta, Index offset) {
      // Scan descriptor even if hierarchy is empty                     
      Unit* result {};
      aux.ForEachDeep([&](const Unit* unit) {
         if (unit->CastsTo(meta)) {
            // Found match                                              
            if (offset == 0) {
               // We're done                                            
               result = const_cast<Unit*>(unit);
               return Flow::Break;
            }
            else --offset;
         }

         return Flow::Continue;
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
   ///   @param aux - descriptor to search through                            
   ///   @param meta - the type of the unit to seek for                       
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<Seek SEEK>
   LANGULUS(INLINED)
   const Unit* Hierarchy::SeekUnitAux(const Any& aux, DMeta meta, Index offset) const {
      return const_cast<Hierarchy*>(this)->template
         SeekUnitAux<SEEK>(aux, meta, offset);
   }

   /// Seek a unit with specific properties                                   
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param ext - the type and properties of the unit to seek for         
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<Seek SEEK>
   LANGULUS(INLINED)
   Unit* Hierarchy::SeekUnitExt(const Construct& ext, Index offset) {
      for (auto owner : *this) {
         Unit* result = owner->template SeekUnitExt<SEEK>(ext, offset);
         if (result)
            return result;
      }

      return nullptr;
   }

   /// Seek a unit with specific properties (const)                           
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param ext - the type and properties of the unit to seek for         
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<Seek SEEK>
   LANGULUS(INLINED)
   const Unit* Hierarchy::SeekUnitExt(const Construct& ext, Index offset) const {
      return const_cast<Hierarchy*>(this)->template
         SeekUnitExt<SEEK>(ext, offset);
   }

   /// Seek a unit with specific properties                                   
   /// Scan a locally provided descriptor first                               
   ///   @tparam SEEK - the direction in which we're scanning the hierarchy   
   ///   @param aux - local descriptor to seek through first                  
   ///   @param ext - the unit type and properties to seek for                
   ///   @param offset - the Nth match to return                              
   ///   @return a pointer to the found unit, or nullptr if not found         
   template<Seek SEEK>
   LANGULUS(INLINED)
   Unit* Hierarchy::SeekUnitAuxExt(const Any& aux, const Construct& ext, Index offset) {
      // Scan descriptor even if hierarchy is empty                     
      Unit* result {};
      aux.ForEachDeep([&](const Unit* u) {
         if (u->CastsTo(ext.GetType())) {
            //TODO check construct arguments
            // Found match                                              
            if (offset == 0) {
               // We're done                                            
               result = const_cast<Unit*>(u);
               return Flow::Break;
            }
            else --offset;
         }

         return Flow::Continue;
      });

      if (result)
         return result;

      // If reached, then no unit was found in the descriptor           
      // Let's delve into the hierarchy                                 
      return SeekUnitExt<SEEK>(ext, offset);
   }

   /// Seek a unit with specific properties (const)                           
   /// Scan a locally provided descriptor first                               
   ///   @tparam SEEK - the direction in which we're scanning the hierarchy   
   ///   @param aux - local descriptor to seek through first                  
   ///   @param ext - the unit type and properties to seek for                
   ///   @param offset - the Nth match to return                              
   ///   @return a pointer to the found unit, or nullptr if not found         
   template<Seek SEEK>
   LANGULUS(INLINED)
   const Unit* Hierarchy::SeekUnitAuxExt(const Any& aux, const Construct& ext, Index offset) const {
      return const_cast<Hierarchy*>(this)->template
         SeekUnitAuxExt<SEEK>(aux, ext, offset);
   }
   
   /// Find a trait by type (and index), searching into the hierarchy         
   ///   @tparam SEEK - direction to search at                                
   ///   @param meta - the trait to search for                                
   ///   @param offset - the offset to apply                                  
   ///   @return the trait, which is not empty, if trait was found            
   template<Seek SEEK>
   LANGULUS(INLINED)
   Trait Hierarchy::SeekTrait(TMeta meta, Index offset) {
      for (auto owner : *this) {
         auto result = owner->template SeekTrait<SEEK>(meta, offset);
         if (result)
            return result;
      }

      return {};
   }

   /// Find a trait, searching into the hierarchy (const)                     
   ///   @tparam SEEK - direction to search at                                
   ///   @param meta - the trait type to search for                           
   ///   @param offset - the number of the matching trait to use              
   ///   @return the trait, which is not empty, if trait was found            
   template<Seek SEEK>
   LANGULUS(INLINED)
   Trait Hierarchy::SeekTrait(TMeta meta, Index offset) const {
      Trait result = const_cast<Hierarchy*>(this)->template
         SeekTrait<SEEK>(meta, offset);
      result.MakeConst();
      return Abandon(result);
   }
   
   /// Find a trait by type (and index) from the hierarchy, and attempt       
   /// converting it to a desired output type                                 
   /// Supports pinnable outputs                                              
   ///   @tparam SEEK - direction to search at                                
   ///   @tparam D - type of output (deducible)                               
   ///   @param meta - the trait type to search for                           
   ///   @param output - [out] the output                                     
   ///   @param offset - the number of the matching trait to use              
   ///   @return true if output was rewritten                                 
   template<Seek SEEK, CT::Data D>
   LANGULUS(INLINED)
   bool Hierarchy::SeekTrait(TMeta meta, D& output, Index offset) const {
      if constexpr (CT::Pinnable<D>) {
         // Never touch pinned values                                   
         if (output.mLocked)
            return false;
      }

      // Let's delve into the hierarchy                                 
      for (auto owner : *this) {
         if (owner->template SeekTrait<SEEK, D>(meta, output, offset)) {
            // Value was found                                          
            return true;
         }
      }

      // If reached, nothing was found                                  
      return false;
   }
   
   /// Find a trait, searching into the hierarchy (const)                     
   ///   @tparam SEEK - direction to search at                                
   ///   @param aux - descriptor to search through                            
   ///   @param meta - the trait type to search for                           
   ///   @param offset - the number of the matching trait to use              
   ///   @return the trait, which is not empty, if trait was found            
   template<Seek SEEK>
   LANGULUS(INLINED)
   Trait Hierarchy::SeekTraitAux(const Any& aux, TMeta meta, Index offset) const {
      // Scan descriptor                                                
      Trait result;
      aux.ForEachDeep([&](const Trait& trait) {
         if (trait.TraitIs(meta)) {
            if (offset == 0) {
               // Match found                                           
               result = trait;
               return Flow::Break;
            }
            
            --offset;
         }

         return Flow::Continue;
      });

      if (result)
         return Abandon(result);

      // If reached, then no trait was found in the descriptor          
      // Let's delve into the hierarchy                                 
      return SeekTrait<SEEK>(meta, offset);
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
   LANGULUS(INLINED)
   bool Hierarchy::SeekTraitAux(const Any& aux, TMeta meta, D& output, Index offset) const {
      if constexpr (CT::Pinnable<D>) {
         // Never touch pinned values                                   
         if (output.mLocked)
            return false;
      }

      // Scan descriptor                                                
      bool done = false;
      aux.ForEachDeep([&](const Trait& trait) {
         if (trait.TraitIs(meta)) {
            // Found match                                              
            try {
               output = trait.template AsCast<D>();

               // Didn't throw, but we're done only if offset matches   
               done = offset == 0;
               --offset;
               return not done;
            }
            catch (...) { }
         }

         return Flow::Continue;
      });

      if (done) {
         // Trait was found in the descriptor                           
         if constexpr (CT::Pinnable<D>) {
            // Make sure to pin the pinnable value                      
            output.mLocked = true;
         }

         return true;
      }

      // If reached, then no trait was found in the descriptor          
      // Let's delve into the hierarchy                                 
      return SeekTrait<SEEK, D>(meta, output, offset);
   }

   /// Seek a value from static/dynamic/unit traits in the hierarchy          
   ///   @tparam SEEK - the direction we're searching the hierarchy in        
   ///   @tparam D - the type of value we're searching for (deducible)        
   ///   @param output - [out] the found value will be written here           
   ///   @param offset - the number of the matching trait to use              
   ///   @return true if value has been found and rewritten                   
   template<Seek SEEK, CT::Data D>
   LANGULUS(INLINED)
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
   ///   @param aux - descriptor to search through first                      
   ///   @param output - [out] the found value will be written here           
   ///   @param offset - the number of the matching trait to use              
   ///   @return true if value has been found and rewritten                   
   template<Seek SEEK, CT::Data D>
   LANGULUS(INLINED)
   bool Hierarchy::SeekValueAux(const Any& aux, D& output, Index offset) const {
      if constexpr (CT::Pinnable<D>) {
         // Never touch pinned values                                   
         if (output.mLocked)
            return false;
      }

      // Scan descriptor                                                
      bool done = false;
      aux.ForEachDeep([&](const Block& group) {
         try {
            // Found match if these don't throw                         
            if constexpr (CT::Pinnable<D>)
               output = group.template AsCast<TypeOf<D>>();
            else
               output = group.template AsCast<D>();

            // Didn't throw, but we're done only if offset matches      
            done = offset == 0;
            --offset;
            return not done;
         }
         catch(...) { }

         return Flow::Continue;
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