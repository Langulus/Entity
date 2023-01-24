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
   Unit* Thing::SeekUnit(DMeta meta, const Index& offset) {
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
            result = mOwner->SeekUnit<SeekStyle::HereAndAbove>(meta, offset);
            if (result)
               return result;
         }
      }

      if constexpr (SEEK & SeekStyle::Below) {
         // Seek children, if requested                                 
         for (auto child : mChildren) {
            result = child->SeekUnit<SeekStyle::HereAndBelow>(meta, offset);
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
   const Unit* Thing::SeekUnit(DMeta unit, const Index& offset) const {
      return const_cast<Thing*>(this)->SeekUnit<SEEK>(unit, offset);
   }

   /// Find a unit by index and static type from the hierarchy                
   ///   @tparam T - static unit type to search for                           
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<CT::Unit T, SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Decay<T>* Thing::SeekUnit(const Index& offset) {
      using DT = Decay<T>;
      return static_cast<DT*>(SeekUnit<SEEK>(MetaData::Of<DT>(), offset));
   }

   /// Find a unit by index and static type from the hierarchy (const)        
   ///   @tparam T - static unit type to search for                           
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<CT::Unit T, SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   const Decay<T>* Thing::SeekUnit(const Index& offset) const {
      using DT = Decay<T>;
      return static_cast<const DT*>(SeekUnit<SEEK>(MetaData::Of<DT>(), offset));
   }

   /// Find a trait, searching into the hierarchy                             
   ///   @tparam SEEK - direction to search at                                
   ///   @param var - the trait to search for                                 
   ///   @param offset - the offset to apply                                  
   ///   @return the trait, which is not empty, if trait was found            
   template<SeekStyle SEEK>
   Trait Thing::SeekTrait(TMeta trait, const Index& offset) {
      if constexpr (SEEK & SeekStyle::Here) {
         // Seek here if requested                                      
         auto output = GetTrait(trait);
         if (!output.IsEmpty())
            return Abandon(output);
      }

      if constexpr (SEEK & SeekStyle::Above) {
         // Seek in parents up to root, if requested                    
         if (mOwner) {
            auto output = mOwner->SeekTrait<SeekStyle::HereAndAbove>(trait, offset);
            if (!output.IsEmpty())
               return Abandon(output);
         }
      }

      if constexpr (SEEK & SeekStyle::Below) {
         // Seek children, if requested                                 
         for (auto child : mChildren) {
            auto output = child->SeekTrait<SeekStyle::HereAndBelow>(trait, offset);
            if (!output.IsEmpty())
               return Abandon(output);
         }
      }

      return {};
   }

   /// Find a trait, searching into the hierarchy (const)                     
   ///   @tparam SEEK - direction to search at                                
   ///   @param var - the trait type to search for                            
   ///   @param offset - the number of the matching trait to use              
   ///   @return the trait, which is not empty, if trait was found            
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Trait Thing::SeekTrait(TMeta trait, const Index& offset) const {
      auto result = const_cast<Thing*>(this)
         ->template SeekTrait<SEEK>(trait, offset);
      result.MakeConst();
      return result;
   }

   /// Find a trait, searching into the hierarchy                             
   ///   @tparam T - the trait to search for                                  
   ///   @tparam SEEK - direction to search at                                
   ///   @param offset - the number of the matching trait to use              
   ///   @return the trait, which is not empty, if trait was found            
   template<CT::Trait T, SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Trait Thing::SeekTrait(const Index& offset) {
      return SeekTrait<SEEK>(MetaTrait::Of<T>(), offset);
   }

   /// Find a trait, searching into the hierarchy (const)                     
   ///   @tparam T - the trait to search for                                  
   ///   @tparam SEEK - direction to search at                                
   ///   @param offset - the number of the matching trait to use              
   ///   @return the trait, which is not empty, if trait was found            
   template<CT::Trait T, SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Trait Thing::SeekTrait(const Index& offset) const {
      return SeekTrait<SEEK>(MetaTrait::Of<T>(), offset);
   }

   /// Seek a value from static/dynamic/unit traits in the hierarchy          
   ///   @tparam SEEK - the direction we're searching the hierarchy in        
   ///   @tparam T - the type of value we're searching for (deducible)        
   ///   @param trait - the type of trait we're searching for                 
   ///   @param value - [out] the found values will be written here           
   ///   @return true if value has been found and rewritten                   
   template<SeekStyle SEEK, CT::Data D>
   LANGULUS(ALWAYSINLINE)
   bool Thing::SeekValue(TMeta trait, D& value, const Index& offset) const {
      const auto found = SeekTrait<SEEK>(trait, offset);
      if (found.IsEmpty())
         return false;

      value = found.template AsCast<D>();
      return true;
   }

   /// Seek a value from static/dynamic/unit traits in the hierarchy          
   ///   @tparam T - the type of trait we're searching for                    
   ///   @tparam SEEK - the direction we're searching the hierarchy in        
   ///   @tparam D - the type of value we're searching for (deducible)        
   ///   @param value - [out] the found values will be written here           
   ///   @return true if value has been found and rewritten                   
   template<CT::Trait T, SeekStyle SEEK, CT::Data D>
   LANGULUS(ALWAYSINLINE)
   bool Thing::SeekValue(D& value, const Index& offset) const {
      const auto found = SeekTrait<T, SEEK>(offset);
      if (found.IsEmpty())
         return false;

      value = found.template AsCast<D>();
      return true;
   }


   /// Get a trait by scanning owners and other units                         
   ///   @tparam T - trait to seek for                                        
   ///   @tparam SEEK - where in hierarchy to execute                         
   ///   @param offset - where in hierarchy to execute                        
   ///   @return the trait                                                    
   template<CT::Trait T, SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Trait Unit::SeekTrait(Offset offset) const {
      mOwners.SeekTrait<T, SEEK>();

      for (auto context : mOwners) {
         auto output = context->template SeekTrait<T, SEEK>(offset);
         if (output.IsAllocated())
            return output;
      }

      return {};
   }

   /// Get a value from inside a trait by scanning owners and other units     
   ///   @tparam SEEK - where in hierarchy to execute                         
   ///   @tparam D - data to seek (deducible)                                 
   ///   @param trait - the trait to seek for                                 
   ///   @param value - [out] value to set if trait was found                 
   ///   @param offset - where in hierarchy to execute                        
   ///   @return true if anything was written to value                        
   template<SeekStyle SEEK, CT::Data D>
   LANGULUS(ALWAYSINLINE)
   bool Unit::SeekValue(TMeta trait, D& value, Offset offset) const {
      return mOwners.template SeekTrait<SEEK>(trait, value);
   }

} // namespace Langulus::Entity

#undef ENTITY_VERBOSE_ENABLED
#undef ENTITY_VERBOSE_SELF
#undef ENTITY_VERBOSE