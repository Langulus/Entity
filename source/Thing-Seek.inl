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
   ///   @param unit - the type of the unit to seek for                       
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   const Unit* Thing::SeekUnit(DMeta unit, Index offset) const {
      return const_cast<Thing*>(this)->template SeekUnit<SEEK>(unit, offset);
   }

   /// Find a unit by type and index from the hierarchy                       
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param descriptor - descriptor to search through                     
   ///   @param unit - the type of the unit to seek for                       
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Unit* Thing::SeekUnit(const Any& descriptor, DMeta unit, Index offset) {
      Unit* result {};
      descriptor.ForEachDeep([&](const Unit* u) {
         if (u->CastsTo(unit)) {
            if (offset == 0) {
               // Found match                                           
               result = const_cast<Unit*>(u);
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
      return SeekUnit<SEEK>(unit, offset);
   }
   
   /// Find a unit by type and index from the hierarchy (const)               
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param descriptor - descriptor to search through                     
   ///   @param unit - the type of the unit to seek for                       
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   const Unit* Thing::SeekUnit(const Any& descriptor, DMeta unit, Index offset) const {
      return const_cast<Thing*>(this)->SeekUnit(descriptor, unit, offset);
   }

   /// Find a unit by construct and index from the hierarchy                  
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param construct - the type and properties of the unit to seek for   
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Unit* Thing::SeekUnitExt(const Construct& construct, Index offset) {
      Unit* result = nullptr;
      if constexpr (SEEK & SeekStyle::Here) {
         // Seek here if requested                                      
         result = GetUnitExt(construct, offset);
         if (result)
            return result;
      }

      if constexpr (SEEK & SeekStyle::Above) {
         // Seek in parents up to root, if requested                    
         if (mOwner) {
            result = mOwner->template
               SeekUnitExt<SeekStyle::HereAndAbove>(construct, offset);
            if (result)
               return result;
         }
      }

      if constexpr (SEEK & SeekStyle::Below) {
         // Seek children, if requested                                 
         for (auto child : mChildren) {
            result = child->template
               SeekUnitExt<SeekStyle::HereAndBelow>(construct, offset);
            if (result)
               return result;
         }
      }

      return nullptr;
   }

   /// Find a unit by construct and index from the hierarchy (const)          
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param construct - the type and properties of the unit to seek for   
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   const Unit* Thing::SeekUnitExt(const Construct& construct, Index offset) const {
      return const_cast<Thing*>(this)->template
         SeekUnitExt<SEEK>(construct, offset);
   }

   /// Seen a unit with specific properties                                   
   ///   @tparam SEEK - the direction in which we're scanning the hierarchy   
   ///   @param descriptor - local descriptor to seek through first           
   ///   @param construct - the unit type and properties to seek for          
   ///   @param offset - the Nth match to return                              
   ///   @return a pointer to the found unit, or nullptr if not found         
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Unit* Thing::SeekUnitExt(const Any& descriptor, const Construct& construct, Index offset) {
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

   /// Seen a unit with specific properties (const)                           
   ///   @tparam SEEK - the direction in which we're scanning the hierarchy   
   ///   @param descriptor - local descriptor to seek through first           
   ///   @param construct - the unit type and properties to seek for          
   ///   @param offset - the Nth match to return                              
   ///   @return a pointer to the found unit, or nullptr if not found         
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   const Unit* Thing::SeekUnitExt(const Any& descriptor, const Construct& construct, Index offset) const {
      return const_cast<Thing*>(this)->template
         SeekUnitExt<SEEK>(descriptor, construct, offset);
   }

   /// Find a trait by type (and index), searching into the hierarchy         
   ///   @tparam SEEK - direction to search at                                
   ///   @param trait - the trait to search for                               
   ///   @param offset - the offset to apply                                  
   ///   @return the trait, which is not empty, if trait was found            
   template<SeekStyle SEEK>
   Trait Thing::SeekTrait(TMeta trait, Index offset) {
      if constexpr (SEEK & SeekStyle::Here) {
         // Seek here if requested                                      
         auto output = GetTrait(trait, offset);
         if (!output.IsEmpty())
            return Abandon(output);
      }

      if constexpr (SEEK & SeekStyle::Above) {
         // Seek in parents up to root, if requested                    
         if (mOwner) {
            auto output = mOwner->template
               SeekTrait<SeekStyle::HereAndAbove>(trait, offset);
            if (!output.IsEmpty())
               return Abandon(output);
         }
      }

      if constexpr (SEEK & SeekStyle::Below) {
         // Seek children, if requested                                 
         for (auto child : mChildren) {
            auto output = child->template
               SeekTrait<SeekStyle::HereAndBelow>(trait, offset);
            if (!output.IsEmpty())
               return Abandon(output);
         }
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
   Trait Thing::SeekTrait(TMeta trait, Index offset) const {
      auto result = const_cast<Thing*>(this)->template
         SeekTrait<SEEK>(trait, offset);
      result.MakeConst();
      return result;
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
   bool Thing::SeekTrait(TMeta trait, D& output, Index offset) const {
      if constexpr (CT::Pinnable<D>) {
         // Never touch pinned values                                   
         if (output.mLocked)
            return false;
      }

      if constexpr (SEEK & SeekStyle::Here) {
         // Seek here if requested                                      
         auto temp = GetTrait(trait, offset);
         try {
            output = temp.template AsCast<D>();
            return true;
         }
         catch (...) { }
      }

      if constexpr (SEEK & SeekStyle::Above) {
         // Seek in parents up to root, if requested                    
         if (mOwner) {
            if (mOwner->template
               SeekTrait<SeekStyle::HereAndAbove, D>(trait, output, offset))
               return true;
         }
      }

      if constexpr (SEEK & SeekStyle::Below) {
         // Seek children, if requested                                 
         for (auto child : mChildren) {
            if (child->template
               SeekTrait<SeekStyle::HereAndBelow, D>(trait, output, offset))
               return true;
         }
      }

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
   Trait Thing::SeekTrait(const Any& descriptor, TMeta trait, Index offset) const {
      // Scan descriptor                                                
      Trait result;
      descriptor.ForEachDeep([&](const Trait& t) {
         if (t.TraitIs(trait)) {
            // Found match                                              
            result = t;
            return false;
         }

         // Just keep searching...                                      
         return true;
      });

      if (!result.IsEmpty())
         return Abandon(result);

      // If reached, then no trait was found in the descriptor          
      // Let's delve into the hierarchy                                 
      return SeekTrait<SEEK>(trait, offset);
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
   bool Thing::SeekValue(const Any& descriptor, D& value, Index offset) const {
      if constexpr (CT::Pinnable<D>) {
         // Never touch pinned values                                   
         if (value.mLocked)
            return false;
      }

      // Scan descriptor                                                
      bool done = false;
      descriptor.ForEachDeep([&](const Block& group) {
         // Found match                                                 
         try {
            value = group.template AsCast<D>();
            done = true;
            return false;
         }
         catch (...) { }
         // Data is not convertible, just keep searching...             
         return true;
      });

      if (done) {
         // Data was found in the descriptor                            
         if constexpr (CT::Pinnable<D>) {
            // Make sure to pin the pinnable value                      
            value.mLocked = true;
         }

         return true;
      }

      // If reached, then no data was found in the descriptor           
      // Let's delve into the hierarchy                                 
      return SeekValue<SEEK, D>(value, offset);
   }

} // namespace Langulus::Entity

#undef ENTITY_VERBOSE_ENABLED
#undef ENTITY_VERBOSE_SELF
#undef ENTITY_VERBOSE