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
   
#if LANGULUS_FEATURE(MANAGED_MEMORY)
   /// Find a specific unit, searching into the hierarchy                     
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @param token - the unit token to seek for                            
   ///   @param offset - which of the matches to return                       
   ///   @return the found unit, or nullptr if no such unit was found         
   template<SeekStyle SEEK>
   Unit* Thing::SeekUnit(const Token& token, const Index& offset) {
      return SeekUnit<SEEK>(RTTI::Database.GetMetaData(token), offset);
   }

   /// Find a unit by type and index from the hierarchy                       
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param token - the unit token to seek for                            
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   const Unit* Thing::SeekUnit(const Token& token, const Index& offset) const {
      return SeekUnit<SEEK>(RTTI::Database.GetMetaData(token), offset);
   }

   /// Seek a unit inside descriptor first, and then the hierarchy            
   ///   @tparam SEEK - the direction we're seeking in the hierarchy          
   ///   @param descriptor - descriptor to search through                     
   ///   @param token - the unit token to seek for                            
   ///   @param offset - the index of the unit to return                      
   ///   @return a valid unit pointer, if unit was found                      
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   const Unit* Thing::SeekUnit(const Any& descriptor, const Token& token, Index offset) const {
      return SeekUnit<SEEK>(descriptor, RTTI::Database.GetMetaData(token), offset);
   }
#endif
   
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
   const Unit* Thing::SeekUnit(DMeta unit, const Index& offset) const {
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
   const Unit* Thing::SeekUnit(const Any& descriptor, DMeta unit, const Index& offset) const {
      const Unit* result {};
      descriptor.ForEachDeep([&result](const Unit* unit) {
         if (unit->CastsTo(unit)) {
            if (offset == 0) {
               // Found match                                           
               result = unit;
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

   /// Find a unit by index and static type from the hierarchy                
   ///   @tparam T - static unit type to search for                           
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<CT::Data T, SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Decay<T>* Thing::SeekUnit(const Index& offset) {
      static_assert(CT::Unit<T>, "T must be a Unit type");
      using DT = Decay<T>;
      return SeekUnit<SEEK>(MetaData::Of<DT>(), offset);
   }

   /// Find a unit by index and static type from the hierarchy (const)        
   ///   @tparam T - static unit type to search for                           
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<CT::Data T, SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   const Decay<T>* Thing::SeekUnit(const Index& offset) const {
      static_assert(CT::Unit<T>, "T must be a Unit type");
      using DT = Decay<T>;
      return SeekUnit<SEEK>(MetaData::Of<DT>(), offset);
   }
   
   /// Find a unit by index and static type from the hierarchy (const)        
   ///   @tparam T - static unit type to search for                           
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param descriptor - descriptor to search through                     
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<CT::Data T, SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   const Decay<T>* Thing::SeekUnit(const Any& descriptor, const Index& offset) const {
      static_assert(CT::Unit<T>, "T must be a Unit type");
      using DT = Decay<T>;
      return SeekUnit<SEEK>(descriptor, MetaData::Of<DT>(), offset);
   }
   
#if LANGULUS_FEATURE(MANAGED_MEMORY)
   /// Find a trait by token (and index) from the hierarchy                   
   ///   @tparam SEEK - where to seek for the trait                           
   ///   @param token - the trait identifier                                  
   ///   @param index - the index of the unit to return                       
   ///   @return the trait (empty if not found)                               
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Trait Thing::SeekTrait(const Token& token, const Index& index) {
      return SeekTrait<SEEK>(RTTI::Database.GetMetaTrait(token), index);
   }

   /// Find a trait by token (and index) from the hierarchy (const)           
   ///   @tparam SEEK - where to seek for the trait                           
   ///   @param token - the trait identifier                                  
   ///   @param index - the index of the unit to return                       
   ///   @return the trait (empty if not found)                               
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Trait Thing::SeekTrait(const Token& token, const Index& index) const {
      return SeekTrait<SEEK>(RTTI::Database.GetMetaTrait(token), index);
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
   bool Thing::SeekTrait(const Token& token, D& output, const Index& index) const {
      return SeekTrait<SEEK, D>(
         RTTI::Database.GetMetaTrait(token), output, index);
   }

   /// Find a trait by token (and index) from the hierarchy                   
   ///   @tparam SEEK - where to seek for the trait                           
   ///   @param descriptor - local descriptor to search through first         
   ///   @param token - the trait identifier                                  
   ///   @param index - the index of the unit to return                       
   ///   @return the trait (empty if not found)                               
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Trait Thing::SeekTrait(const Any& descriptor, const Token& token, const Index& index) const {
      return SeekTrait<SEEK>(descriptor, 
         RTTI::Database.GetMetaTrait(token), index);
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
   bool Thing::SeekTrait(const Any& descriptor, const Token& token, D& output, const Index& index) const {
      return SeekTrait<SEEK, D>(descriptor, 
         RTTI::Database.GetMetaTrait(token), output, index);
   }
#endif

   /// Find a trait by type (and index), searching into the hierarchy         
   ///   @tparam SEEK - direction to search at                                
   ///   @param trait - the trait to search for                               
   ///   @param offset - the offset to apply                                  
   ///   @return the trait, which is not empty, if trait was found            
   template<SeekStyle SEEK>
   Trait Thing::SeekTrait(TMeta trait, const Index& offset) {
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
   Trait Thing::SeekTrait(TMeta trait, const Index& offset) const {
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
   bool Thing::SeekTrait(TMeta trait, D& output, const Index& offset) const {
      if constexpr (CT::Pinnable<D>) {
         // Never touch pinned values                                   
         if (output.mLocked)
            return false;
      }

      if constexpr (SEEK & SeekStyle::Here) {
         // Seek here if requested                                      
         auto temp = GetTrait(trait, offset);
         try {
            output = temp.AsCast<D>();
            return true;
         }
         catch (...) { }
      }

      if constexpr (SEEK & SeekStyle::Above) {
         // Seek in parents up to root, if requested                    
         if (mOwner) {
            if (mOwner->template SeekTrait<SeekStyle::HereAndAbove, D>(trait, output, offset))
               return true;
         }
      }

      if constexpr (SEEK & SeekStyle::Below) {
         // Seek children, if requested                                 
         for (auto child : mChildren) {
            if (child->template SeekTrait<SeekStyle::HereAndBelow, D>(trait, output, offset))
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
   Trait Thing::SeekTrait(const Any& descriptor, TMeta trait, const Index& offset) const {
      // Scan descriptor                                                
      Trait result;
      descriptor.ForEachDeep([this](const Trait& t) {
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
   
   /// Find a trait, searching into the hierarchy (const)                     
   ///   @tparam T - the trait to search for                                  
   ///   @tparam SEEK - direction to search at                                
   ///   @param descriptor - descriptor to search through                     
   ///   @param offset - the number of the matching trait to use              
   ///   @return the trait, which is not empty, if trait was found            
   template<CT::Trait T, SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   Trait Thing::SeekTrait(const Any& descriptor, const Index& offset) const {
      return SeekTrait<SEEK>(descriptor, MetaTrait::Of<T>(), offset);
   }

   /// Seek a value from static/dynamic/unit traits in the hierarchy          
   ///   @tparam SEEK - the direction we're searching the hierarchy in        
   ///   @tparam T - the type of value we're searching for (deducible)        
   ///   @param trait - the type of trait we're searching for                 
   ///   @param value - [out] the found values will be written here           
   ///   @return true if value has been found and rewritten                   
   template<SeekStyle SEEK, CT::Data D>
   LANGULUS(ALWAYSINLINE)
   bool Thing::SeekValue(D& value, const Index& offset) const {
      const auto found = SeekTrait<Trait, SEEK>(trait, offset);
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
   template<SeekStyle SEEK, CT::Data D>
   LANGULUS(ALWAYSINLINE)
   bool Thing::SeekValue(D& value, const Index& offset) const {
      const auto found = SeekTrait<Trait, SEEK>(offset);
      if (found.IsEmpty())
         return false;

      value = found.template AsCast<D>();
      return true;
   }

} // namespace Langulus::Entity

#undef ENTITY_VERBOSE_ENABLED
#undef ENTITY_VERBOSE_SELF
#undef ENTITY_VERBOSE