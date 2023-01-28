///                                                                           
/// Langulus::Entity                                                          
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Thing.hpp"

#if 0
   #define ENTITY_VERBOSE_ENABLED() 1
   #define ENTITY_VERBOSE_SELF(...)            Logger::Verbose(Self(), __VA_ARGS__)
   #define ENTITY_VERBOSE(...)                 Logger::Append(__VA_ARGS__)
#else
   #define ENTITY_VERBOSE_ENABLED() 0
   #define ENTITY_VERBOSE_SELF(...)
   #define ENTITY_VERBOSE(...)
#endif

namespace Langulus::Entity
{
   
   /// Find a specific unit, searching into the hierarchy                     
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @param meta - the unit to seek for                                   
   ///   @param offset - which of the matches to return                       
   ///   @return the found unit, or nullptr if no such unit was found         
   template<SeekStyle SEEK>
   Unit* Thing::SeekUnit(DMeta meta, Index offset) {
      Unit* result = nullptr;
      if constexpr (SEEK & SeekStyle::Here) {
         // Seek here if requested                                      
         result = GetUnit(meta, offset);
         if (result)
            return result;
      }

      if constexpr (SEEK & SeekStyle::Above) {
         // Seek in parents up to root, if requested                    
         if (mOwner) {
            result = mOwner->template
               SeekUnit<SeekStyle::HereAndAbove>(meta, offset);
            if (result)
               return result;
         }
      }

      if constexpr (SEEK & SeekStyle::Below) {
         // Seek children, if requested                                 
         for (auto child : mChildren) {
            result = child->template
               SeekUnit<SeekStyle::HereAndBelow>(meta, offset);
            if (result)
               return result;
         }
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
   const Unit* Thing::SeekUnit(DMeta meta, Index offset) const {
      return const_cast<Thing*>(this)->template
         SeekUnit<SEEK>(meta, offset);
   }

   /// Find a unit by type and index from the hierarchy                       
   /// Scan an auxiliary descriptor first                                     
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param aux - descriptor to search through                            
   ///   @param meta - the type of the unit to seek for                       
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Unit* Thing::SeekUnitAux(const Any& aux, DMeta meta, Index offset) {
      Unit* result {};
      aux.ForEachDeep([&](const Unit* unit) {
         if (unit->CastsTo(meta)) {
            if (offset == 0) {
               // Found match                                           
               result = const_cast<Unit*>(unit);
               return false;
            }
            else --offset;
         }

         // Just keep searching...                                      
         return true;
      });

      if (result)
         return result;

      // If reached, then no unit was found in the descriptor           
      // Let's delve into the hierarchy                                 
      return SeekUnit<SEEK>(meta, offset);
   }
   
   /// Find a unit by type and index from the hierarchy (const)               
   /// Scan an auxiliary descriptor first                                     
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param aux - descriptor to search through                            
   ///   @param meta - the type of the unit to seek for                       
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   const Unit* Thing::SeekUnitAux(const Any& aux, DMeta meta, Index offset) const {
      return const_cast<Thing*>(this)->template
         SeekUnitAux<SEEK>(aux, meta, offset);
   }

   /// Find a unit by construct and index from the hierarchy                  
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param ext - the type and properties of the unit to seek for         
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Unit* Thing::SeekUnitExt(const Construct& ext, Index offset) {
      Unit* result = nullptr;
      if constexpr (SEEK & SeekStyle::Here) {
         // Seek here if requested                                      
         result = GetUnitExt(ext, offset);
         if (result)
            return result;
      }

      if constexpr (SEEK & SeekStyle::Above) {
         // Seek in parents up to root, if requested                    
         if (mOwner) {
            result = mOwner->template
               SeekUnitExt<SeekStyle::HereAndAbove>(ext, offset);
            if (result)
               return result;
         }
      }

      if constexpr (SEEK & SeekStyle::Below) {
         // Seek children, if requested                                 
         for (auto child : mChildren) {
            result = child->template
               SeekUnitExt<SeekStyle::HereAndBelow>(ext, offset);
            if (result)
               return result;
         }
      }

      return nullptr;
   }

   /// Find a unit by construct and index from the hierarchy (const)          
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param ext - the type and properties of the unit to seek for         
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   const Unit* Thing::SeekUnitExt(const Construct& ext, Index offset) const {
      return const_cast<Thing*>(this)->template
         SeekUnitExt<SEEK>(ext, offset);
   }

   /// Seen a unit with specific properties                                   
   /// Scan an auxiliary descriptor first                                     
   ///   @tparam SEEK - the direction in which we're scanning the hierarchy   
   ///   @param aux - local descriptor to seek through first                  
   ///   @param ext - the unit type and properties to seek for                
   ///   @param offset - the Nth match to return                              
   ///   @return a pointer to the found unit, or nullptr if not found         
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Unit* Thing::SeekUnitAuxExt(const Any& aux, const Construct& ext, Index offset) {
      // Scan descriptor even if hierarchy is empty                     
      Unit* result {};
      aux.ForEachDeep([&](const Unit* unit) {
         if (unit->CastsTo(ext.GetType())) {
            //TODO check construct arguments
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
      return SeekUnitExt<SEEK>(ext, offset);
   }

   /// Seen a unit with specific properties (const)                           
   /// Scan an auxiliary descriptor first                                     
   ///   @tparam SEEK - the direction in which we're scanning the hierarchy   
   ///   @param aux - local descriptor to seek through first                  
   ///   @param ext - the unit type and properties to seek for                
   ///   @param offset - the Nth match to return                              
   ///   @return a pointer to the found unit, or nullptr if not found         
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   const Unit* Thing::SeekUnitAuxExt(const Any& aux, const Construct& ext, Index offset) const {
      return const_cast<Thing*>(this)->template
         SeekUnitAuxExt<SEEK>(aux, ext, offset);
   }

   /// Find a trait by type (and index), searching into the hierarchy         
   ///   @tparam SEEK - direction to search at                                
   ///   @param meta - the trait to search for                                
   ///   @param offset - the offset to apply                                  
   ///   @return the trait, which is not empty, if trait was found            
   template<SeekStyle SEEK>
   Trait Thing::SeekTrait(TMeta meta, Index offset) {
      if constexpr (SEEK & SeekStyle::Here) {
         // Seek here if requested                                      
         auto output = GetTrait(meta, offset);
         if (!output.IsEmpty())
            return Abandon(output);
      }

      if constexpr (SEEK & SeekStyle::Above) {
         // Seek in parents up to root, if requested                    
         if (mOwner) {
            auto output = mOwner->template
               SeekTrait<SeekStyle::HereAndAbove>(meta, offset);
            if (!output.IsEmpty())
               return Abandon(output);
         }
      }

      if constexpr (SEEK & SeekStyle::Below) {
         // Seek children, if requested                                 
         for (auto child : mChildren) {
            auto output = child->template
               SeekTrait<SeekStyle::HereAndBelow>(meta, offset);
            if (!output.IsEmpty())
               return Abandon(output);
         }
      }

      return {};
   }

   /// Find a trait, searching into the hierarchy (const)                     
   ///   @tparam SEEK - direction to search at                                
   ///   @param meta - the trait type to search for                           
   ///   @param offset - the number of the matching trait to use              
   ///   @return the trait, which is not empty, if trait was found            
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Trait Thing::SeekTrait(TMeta meta, Index offset) const {
      auto result = const_cast<Thing*>(this)->template
         SeekTrait<SEEK>(meta, offset);
      result.MakeConst();
      return result;
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
   template<SeekStyle SEEK, CT::Data D>
   LANGULUS(ALWAYSINLINE)
   bool Thing::SeekTrait(TMeta meta, D& output, Index offset) const {
      if constexpr (CT::Pinnable<D>) {
         // Never touch pinned values                                   
         if (output.mLocked)
            return false;
      }

      if constexpr (SEEK & SeekStyle::Here) {
         // Seek here if requested                                      
         auto temp = GetTrait(meta, offset);
         try {
            if constexpr (CT::Pinnable<D>)
               output = temp.template AsCast<TypeOf<D>>();
            else
               output = temp.template AsCast<D>();
            return true;
         }
         catch (...) { }
      }

      if constexpr (SEEK & SeekStyle::Above) {
         // Seek in parents up to root, if requested                    
         if (mOwner) {
            if (mOwner->template
               SeekTrait<SeekStyle::HereAndAbove, D>(meta, output, offset))
               return true;
         }
      }

      if constexpr (SEEK & SeekStyle::Below) {
         // Seek children, if requested                                 
         for (auto child : mChildren) {
            if (child->template
               SeekTrait<SeekStyle::HereAndBelow, D>(meta, output, offset))
               return true;
         }
      }

      return false;
   }
   
   /// Find a trait, searching into the hierarchy (const)                     
   /// Scan an auxiliary descriptor first                                     
   ///   @tparam SEEK - direction to search at                                
   ///   @param aux - descriptor to search through                            
   ///   @param meta - the trait type to search for                           
   ///   @param offset - the number of the matching trait to use              
   ///   @return the trait, which is not empty, if trait was found            
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Trait Thing::SeekTraitAux(const Any& aux, TMeta meta, Index offset) const {
      // Scan descriptor                                                
      Trait result;
      aux.ForEachDeep([&](const Trait& trait) {
         if (trait.TraitIs(meta)) {
            // Found match                                              
            result = trait;
            return false;
         }

         // Just keep searching...                                      
         return true;
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
   bool Thing::SeekValue(D& output, Index offset) const {
      if constexpr (CT::Pinnable<D>) {
         // Never touch pinned values                                   
         if (output.mLocked)
            return false;
      }

      if constexpr (SEEK & SeekStyle::Here) {
         // Seek here if requested                                      
         auto temp = GetTrait(nullptr, offset);
         try {
            if constexpr (CT::Pinnable<D>)
               output = temp.template AsCast<TypeOf<D>>();
            else
               output = temp.template AsCast<D>();
            return true;
         }
         catch (...) {}
      }

      if constexpr (SEEK & SeekStyle::Above) {
         // Seek in parents up to root, if requested                    
         if (mOwner) {
            if (mOwner->template
               SeekValue<SeekStyle::HereAndAbove, D>(output, offset))
               return true;
         }
      }

      if constexpr (SEEK & SeekStyle::Below) {
         // Seek in children, if requested                              
         for (auto child : mChildren) {
            if (child->template
               SeekValue<SeekStyle::HereAndBelow, D>(output, offset))
               return true;
         }
      }

      return false;
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
   template<SeekStyle SEEK, CT::Data D>
   LANGULUS(ALWAYSINLINE)
   bool Thing::SeekValueAux(const Any& aux, D& output, Index offset) const {
      if constexpr (CT::Pinnable<D>) {
         // Never touch pinned values                                   
         if (output.mLocked)
            return false;
      }

      // Scan descriptor                                                
      bool done = false;
      aux.ForEachDeep([&](const Block& group) {
         // Found match                                                 
         try {
            // Found match if these don't throw                         
            if constexpr (CT::Pinnable<D>)
               output = group.template AsCast<TypeOf<D>>();
            else
               output = group.template AsCast<D>();

            // Didn't throw, but we're done only if offset matches      
            done = offset == 0;
            --offset;
            return !done;
         }
         catch (...) { }

         return true;         // Just keep searching...                 
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

#undef ENTITY_VERBOSE_ENABLED
#undef ENTITY_VERBOSE_SELF
#undef ENTITY_VERBOSE