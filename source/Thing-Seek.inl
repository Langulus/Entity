///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Thing.hpp"
#include "Hierarchy-Seek.inl"

#if 0
   #define ENTITY_VERBOSE_ENABLED() 1
   #define ENTITY_VERBOSE_SELF(...)    Logger::Verbose(Self(), __VA_ARGS__)
   #define ENTITY_VERBOSE(...)         Logger::Append(__VA_ARGS__)
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
   template<Seek SEEK>
   A::Unit* Thing::SeekUnit(DMeta meta, Index offset) {
      A::Unit* result = nullptr;
      if constexpr (SEEK & Seek::Here) {
         // Seek here if requested                                      
         result = GetUnitMeta(meta, offset);
         if (result)
            return result;
      }

      if constexpr (SEEK & Seek::Above) {
         // Seek in parents up to root, if requested                    
         if (mOwner) {
            result = mOwner->template
               SeekUnit<Seek::HereAndAbove>(meta, offset);
            if (result)
               return result;
         }
      }

      if constexpr (SEEK & Seek::Below) {
         // Seek children, if requested                                 
         for (auto child : mChildren) {
            result = child->template
               SeekUnit<Seek::HereAndBelow>(meta, offset);
            if (result)
               return result;
         }
      }

      return nullptr;
   }

   /// Find a unit by type and index from the hierarchy                       
   /// Scan an auxiliary descriptor first                                     
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param aux - descriptor to search through                            
   ///   @param meta - the type of the unit to seek for                       
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<Seek SEEK> LANGULUS(INLINED)
   A::Unit* Thing::SeekUnitAux(const Neat& aux, DMeta meta, Index offset) {
      A::Unit* result {};
      aux.ForEachDeep([&](const A::Unit* unit) {
         if (unit->CastsTo(meta)) {
            if (offset == 0) {
               // Found match                                           
               result = const_cast<A::Unit*>(unit);
               return Loop::Break;
            }
            else --offset;
         }

         // Just keep searching...                                      
         return Loop::Continue;
      });

      if (result)
         return result;

      // If reached, then no unit was found in the descriptor           
      // Let's delve into the hierarchy                                 
      return SeekUnit<SEEK>(meta, offset);
   }

   /// Find a unit by construct and index from the hierarchy                  
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param ext - the type and properties of the unit to seek for         
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<Seek SEEK> LANGULUS(INLINED)
   A::Unit* Thing::SeekUnitExt(DMeta type, const Neat& ext, Index offset) {
      A::Unit* result = nullptr;
      if constexpr (SEEK & Seek::Here) {
         // Seek here if requested                                      
         result = GetUnitExt(type, ext, offset);
         if (result)
            return result;
      }

      if constexpr (SEEK & Seek::Above) {
         // Seek in parents up to root, if requested                    
         if (mOwner) {
            result = mOwner->template
               SeekUnitExt<Seek::HereAndAbove>(type, ext, offset);
            if (result)
               return result;
         }
      }

      if constexpr (SEEK & Seek::Below) {
         // Seek children, if requested                                 
         for (auto child : mChildren) {
            result = child->template
               SeekUnitExt<Seek::HereAndBelow>(type, ext, offset);
            if (result)
               return result;
         }
      }

      return nullptr;
   }

   /// Seen a unit with specific properties                                   
   /// Scan an auxiliary descriptor first                                     
   ///   @tparam SEEK - the direction in which we're scanning the hierarchy   
   ///   @param aux - local descriptor to seek through first                  
   ///   @param ext - the unit type and properties to seek for                
   ///   @param offset - the Nth match to return                              
   ///   @return a pointer to the found unit, or nullptr if not found         
   template<Seek SEEK> LANGULUS(INLINED)
   A::Unit* Thing::SeekUnitAuxExt(DMeta type, const Neat& aux, const Neat& ext, Index offset) {
      // Scan descriptor even if hierarchy is empty                     
      A::Unit* result {};
      aux.ForEachDeep([&](const A::Unit* unit) {
         if (unit->CastsTo(type)) {
            //TODO check construct arguments
            // Found match                                              
            if (offset == 0) {
               // We're done                                            
               result = const_cast<A::Unit*>(unit);
               return Loop::Break;
            }
            else --offset;
         }
         return Loop::Continue;
      });

      if (result)
         return result;

      // If reached, then no unit was found in the descriptor           
      // Let's delve into the hierarchy                                 
      return SeekUnitExt<SEEK>(type, ext, offset);
   }

   /// Find a trait by type (and index), searching into the hierarchy         
   ///   @tparam SEEK - direction to search at                                
   ///   @param meta - the trait to search for                                
   ///   @param offset - the offset to apply                                  
   ///   @return the trait, which is not empty, if trait was found            
   template<Seek SEEK>
   Trait Thing::SeekTrait(TMeta meta, Index offset) {
      if constexpr (SEEK & Seek::Here) {
         // Seek here if requested                                      
         auto output = GetTrait(meta, offset);
         if (output)
            return Abandon(output);
      }

      if constexpr (SEEK & Seek::Above) {
         // Seek in parents up to root, if requested                    
         if (mOwner) {
            auto output = mOwner->template
               SeekTrait<Seek::HereAndAbove>(meta, offset);
            if (output)
               return Abandon(output);
         }
      }

      if constexpr (SEEK & Seek::Below) {
         // Seek children, if requested                                 
         for (auto child : mChildren) {
            auto output = child->template
               SeekTrait<Seek::HereAndBelow>(meta, offset);
            if (output)
               return Abandon(output);
         }
      }

      return {};
   }

   /// Find a trait, searching into the hierarchy (const)                     
   /// Scan an auxiliary descriptor first                                     
   ///   @tparam SEEK - direction to search at                                
   ///   @param aux - descriptor to search through                            
   ///   @param meta - the trait type to search for                           
   ///   @param offset - the number of the matching trait to use              
   ///   @return the trait, which is not empty, if trait was found            
   template<Seek SEEK> LANGULUS(INLINED)
   Trait Thing::SeekTraitAux(const Neat& aux, TMeta meta, Index offset) {
      // Scan descriptor                                                
      Trait result;
      aux.ForEachDeep([&](const Trait& trait) {
         if (trait.IsTrait(meta)) {
            // Found match                                              
            result = trait;
            return Loop::Break;
         }

         // Just keep searching...                                      
         return Loop::Continue;
      });

      if (result)
         return Abandon(result);

      // If reached, then no trait was found in the descriptor          
      // Let's delve into the hierarchy                                 
      return SeekTrait<SEEK>(meta, offset);
   }
   
   /// Find a trait by type (and index) from the hierarchy, and attempt       
   /// converting it to a desired output type                                 
   /// Supports pinnable outputs                                              
   ///   @tparam SEEK - direction to search at                                
   ///   @param meta - the trait type to search for                           
   ///   @param output - [out] the output                                     
   ///   @param offset - the number of the matching trait to use              
   ///   @return true if output was rewritten                                 
   template<Seek SEEK>
   bool Thing::SeekValue(TMeta meta, CT::Data auto& output, Index offset) const {
      using D = Deref<decltype(output)>;

      if constexpr (CT::Pinnable<D>) {
         // Never touch pinned values                                   
         if (output.mLocked)
            return false;
      }

      if constexpr (SEEK & Seek::Here) {
         // Seek here if requested                                      
         auto temp = GetTrait(meta, offset);
         try {
            if (CT::Pinnable<D> and temp.Is<TypeOf<D>>())
               output = temp.As<TypeOf<D>>();
            else if (not CT::Pinnable<D> and temp.Is<D>())
               output = temp.As<D>();
            else if constexpr (CT::DescriptorMakable<D>)
               output = D {Describe(static_cast<const Many&>(temp))};
            else if constexpr (CT::Pinnable<D>)
               output = temp.template AsCast<TypeOf<D>>();
            else
               output = temp.template AsCast<D>();

            /*if constexpr (CT::Pinnable<D>)
               output = temp.template AsCast<TypeOf<D>>();
            else
               output = temp.template AsCast<D>();*/
            return true;
         }
         catch (...) { }
      }

      if constexpr (SEEK & Seek::Above) {
         // Seek in parents up to root, if requested                    
         if (mOwner) {
            if (mOwner->template
               SeekValue<Seek::HereAndAbove>(meta, output, offset))
               return true;
         }
      }

      if constexpr (SEEK & Seek::Below) {
         // Seek children, if requested                                 
         for (auto child : mChildren) {
            if (child->template
               SeekValue<Seek::HereAndBelow>(meta, output, offset))
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
   ///   @param aux - descriptor to search through first                      
   ///   @param output - [out] the found value will be written here           
   ///   @param offset - the number of the matching trait to use              
   ///   @return true if value has been found and rewritten                   
   template<Seek SEEK> LANGULUS(INLINED)
   bool Thing::SeekValueAux(
      TMeta meta, const Neat& aux, CT::Data auto& output, Index offset
   ) const {
      using D = Deref<decltype(output)>;

      if constexpr (CT::Pinnable<D>) {
         // Never touch pinned values                                   
         if (output.mLocked)
            return false;
      }

      // Scan descriptor                                                
      bool done = false;
      if (meta) {
         aux.ForEachDeep([&](const Trait& trait) {
            if (trait.IsTrait(meta)) {
               // Found match                                           
               try {
                  if constexpr (CT::Pinnable<D>)
                     output = trait.template AsCast<TypeOf<D>>();
                  else
                     output = trait.template AsCast<D>();

                  // Didn't throw, but we're done only if offset matches
                  done = offset == 0;
                  --offset;
                  return not done;
               }
               catch (...) {}
            }

            return Loop::Continue;
         });
      }
      else {
         aux.ForEachDeep([&](const Block<>& group) {
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

            return Loop::Continue;
         });
      }

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
      return SeekValue<SEEK>(meta, output, offset);
   }

} // namespace Langulus::Entity

#undef ENTITY_VERBOSE_ENABLED
#undef ENTITY_VERBOSE_SELF
#undef ENTITY_VERBOSE