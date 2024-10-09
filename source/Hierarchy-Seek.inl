///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Hierarchy.hpp"
#include "Thing.hpp"
#include "Unit.hpp"
#include "Pin.inl"

#define TEMPLATE()   template<class THIS>
#define TME()        SeekInterface<THIS>


namespace Langulus::Entity
{

   /// Find a unit by type and optional offset, but search first in an        
   /// auxiliary descriptor                                                   
   ///   @tparam SEEK - the direction to seek in                              
   ///   @param aux - the auxiliary descriptor to scan first                  
   ///   @param type - the type of unit to search for                         
   ///   @param offset - the match to return                                  
   ///   @return a pointer to the found unit, or nullptr if not found         
   TEMPLATE() template<Seek SEEK> LANGULUS(INLINED)
   auto TME()::SeekUnitAux(const Many& aux, DMeta type, Index offset) const -> const A::Unit* {
      return const_cast<THIS*>(static_cast<const THIS*>(this))
         ->template SeekUnitAux<SEEK>(aux, type, offset);
   }

   TEMPLATE() template<CT::Data T, Seek SEEK> LANGULUS(INLINED)
   auto TME()::SeekUnitAux(const Many& aux, Index offset) -> Decay<T>* {
      return dynamic_cast<Decay<T>*>(static_cast<THIS*>(this)
         ->template SeekUnitAux<SEEK>(aux, MetaDataOf<Decay<T>>(), offset));
   }

   TEMPLATE() template<CT::Data T, Seek SEEK> LANGULUS(INLINED)
   auto TME()::SeekUnitAux(const Many& aux, Index offset) const -> const Decay<T>* {
      return const_cast<TME()*>(this)
         ->template SeekUnitAux<T, SEEK>(aux, offset);
   }
      
   /// Find a unit by type and specific properties and optional offset        
   ///   @tparam SEEK - the direction to seek in                              
   ///   @param type - the type of unit to search for                         
   ///   @param ext - the properties of the unit to search for                
   ///   @param offset - the match to return                                  
   ///   @return a pointer to the found unit, or nullptr if not found         
   TEMPLATE() template<Seek SEEK> LANGULUS(INLINED)
   auto TME()::SeekUnitExt(DMeta type, const Many& ext, Index offset) const -> const A::Unit* {
      return const_cast<THIS*>(static_cast<const THIS*>(this))
         ->template SeekUnitExt<SEEK>(type, ext, offset);
   }

   TEMPLATE() template<CT::Data T, Seek SEEK> LANGULUS(INLINED)
   auto TME()::SeekUnitExt(const Many& ext, Index offset) -> Decay<T>* {
      return dynamic_cast<Decay<T>*>(static_cast<THIS*>(this)
         ->template SeekUnitExt<SEEK>(MetaDataOf<Decay<T>>(), ext, offset));
   }

   TEMPLATE() template<CT::Data T, Seek SEEK> LANGULUS(INLINED)
   auto TME()::SeekUnitExt(const Many& ext, Index offset) const -> const Decay<T>* {
      return const_cast<TME()*>(this)
         ->template SeekUnitExt<T, SEEK>(ext, offset);
   }

   /// Find a unit by type, specific properties, and optional offset, but     
   /// search first in an auxiliary descriptor                                
   ///   @tparam SEEK - the direction to seek in                              
   ///   @param aux - the auxiliary descriptor to scan first                  
   ///   @param type - the type of unit to search for                         
   ///   @param ext - the properties of the unit to search for                
   ///   @param offset - the match to return                                  
   ///   @return a pointer to the found unit, or nullptr if not found         
   TEMPLATE() template<Seek SEEK> LANGULUS(INLINED)
   auto TME()::SeekUnitAuxExt(DMeta type, const Many& aux, const Many& ext, Index offset) const -> const A::Unit* {
      return const_cast<THIS*>(static_cast<const THIS*>(this))
         ->template SeekUnitAuxExt<SEEK>(type, aux, ext, offset);
   }

   TEMPLATE() template<CT::Data T, Seek SEEK> LANGULUS(INLINED)
   auto TME()::SeekUnitAuxExt(const Many& aux, const Many& ext, Index offset) -> Decay<T>* {
      return dynamic_cast<Decay<T>*>(static_cast<THIS*>(this)
         ->template SeekUnitAuxExt<SEEK>(MetaDataOf<Decay<T>>(), aux, ext, offset));
   }

   TEMPLATE() template<CT::Data T, Seek SEEK> LANGULUS(INLINED)
   auto TME()::SeekUnitAuxExt(const Many& aux, const Many& ext, Index offset) const -> const Decay<T>* {
      return const_cast<TME()*>(this)
         ->template SeekUnitAuxExt<T, SEEK>(aux, ext, offset);
   }

   /// Find a trait by type and specific properties and optional offset       
   ///   @tparam SEEK - the direction to seek in                              
   ///   @param ext - the properties of the unit to search for                
   ///   @param type - the type of trait to search for                        
   ///   @param offset - the match to return                                  
   ///   @return a pointer to the found unit, or nullptr if not found         
   TEMPLATE() template<Seek SEEK> LANGULUS(INLINED)
   auto TME()::SeekTraitAux(const Many& aux, TMeta type, Index offset) const -> Trait {
      return const_cast<THIS*>(static_cast<const THIS*>(this))
         ->template SeekTraitAux<SEEK>(aux, type, offset);
   }

   TEMPLATE() template<CT::Trait T, Seek SEEK> LANGULUS(INLINED)
   auto TME()::SeekTraitAux(const Many& aux, Index offset) -> Trait {
      return static_cast<THIS*>(this)
         ->template SeekTraitAux<SEEK>(aux, MetaTraitOf<T>(), offset);
   }

   TEMPLATE() template<CT::Trait T, Seek SEEK> LANGULUS(INLINED)
   auto TME()::SeekTraitAux(const Many& aux, Index offset) const -> Trait {
      return const_cast<TME()*>(this)
         ->template SeekTraitAux<T, SEEK>(aux, offset);
   }



   TEMPLATE() template<CT::Trait T, Seek SEEK> LANGULUS(INLINED)
   bool TME()::SeekValueAux(const Many& aux, CT::NotTagged auto& output, Index offset) const {
      return static_cast<const THIS*>(this)
         ->template SeekValueAux<SEEK>(MetaTraitOf<T>(), aux, output, offset);
   }
   


   TEMPLATE() template<Seek SEEK> LANGULUS(INLINED)
   bool TME()::SeekValueAux(const Many& aux, CT::Tagged auto& output, Index offset) const {
      using T = Deref<decltype(output)>;
      auto lambda = [&]<class T>() {
         return static_cast<const THIS*>(this)->template
            SeekValueAux<SEEK>(MetaTraitOf<T>(), aux, output.mData, offset);
      };
      return T::Tags::ForEachOr(lambda);
   }
   


   #if LANGULUS_FEATURE(MANAGED_REFLECTION)
      ///                                                                     
      /// Token based interface                                               
      /// Available only when managed reflection is enabled                   
      ///                                                                     
      TEMPLATE() template<Seek SEEK> LANGULUS(INLINED)
      auto TME()::SeekUnitAux(const Many& aux, const Token& dataToken, Index offset) -> A::Unit* {
         return static_cast<THIS*>(this)
            ->template SeekUnitAux<SEEK>(aux, RTTI::GetMetaData(dataToken), offset);
      }

      TEMPLATE() template<Seek SEEK> LANGULUS(INLINED)
      auto TME()::SeekUnitAux(const Many& aux, const Token& dataToken, Index offset) const -> const A::Unit* {
         return static_cast<THIS*>(this)
            ->template SeekUnitAux<SEEK>(aux, RTTI::GetMetaData(dataToken), offset);
      }
      
      TEMPLATE() template<Seek SEEK> LANGULUS(INLINED)
      auto TME()::SeekTraitAux(const Many& aux, const Token& traitToken, Index offset) const -> Trait {
         return static_cast<THIS*>(this)
            ->template SeekTraitAux<SEEK>(aux, RTTI::GetMetaTrait(traitToken), offset);
      }
     
      TEMPLATE() template<Seek SEEK> LANGULUS(INLINED)
      bool TME()::SeekValueAux(const Token& traitToken, const Many& aux, CT::Data auto& output, Index offset) const {
         return static_cast<THIS*>(this)
            ->template SeekValueAux<SEEK>(RTTI::GetMetaTrait(traitToken), aux, output, offset);
      }
   #endif

} // namespace Langulus::Entity

#undef TEMPLATE
#undef TME


namespace Langulus::Entity
{

   /// Find a unit by type and index from the hierarchy                       
   /// Scan a locally provided descriptor first                               
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param aux - descriptor to search through                            
   ///   @param meta - the type of the unit to seek for                       
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<Seek SEEK> LANGULUS(INLINED)
   auto Hierarchy::SeekUnitAux(const Many& aux, DMeta meta, Index offset) -> A::Unit* {
      const A::Unit* result {};

      // Scan descriptor even if hierarchy is empty                     
      if constexpr (SEEK & Seek::Here) {
         aux.ForEachDeep([&](const A::Unit* unit) {
            if (unit->CastsTo(meta)) {
               // Found match                                           
               if (offset == 0) {
                  // We're done                                         
                  result = unit;
                  return Loop::Break;
               }
               else --offset;
            }

            return Loop::Continue;
         });

         if (result)
            return const_cast<A::Unit*>(result);
      }
      
      // If reached, then no unit was found in the descriptor           
      // Let's delve into the hierarchy, by scanning for Traits::Parent 
      // and Traits::Child inside the 'aux'                             
      //return SeekValue<SEEK>(meta, output, offset);
      if constexpr (SEEK & Seek::Above) {
         aux.ForEachTrait([&](const Traits::Parent& trait) -> LoopControl {
            trait.ForEach([&](const Thing& parent) {
               if (nullptr != (result = parent.SeekUnit<Seek::HereAndAbove>(meta, offset)))
                  // Value was found                                    
                  return Loop::Break;
               return Loop::Continue;
            });
            return result == nullptr;
         });
      }

      if constexpr (SEEK & Seek::Below) {
         aux.ForEachTrait([&](const Traits::Child& trait) -> LoopControl {
            trait.ForEach([&](const Thing& child) {
               if (nullptr != (result = child.SeekUnit<Seek::HereAndBelow>(meta, offset)))
                  // Value was found                                    
                  return Loop::Break;
               return Loop::Continue;
            });
            return result == nullptr;
         });
      }

      return const_cast<A::Unit*>(result);
   }

   /// Seek a unit with specific properties                                   
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param type - type of unit to search for                             
   ///   @param ext - the properties of the unit to seek for                  
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<Seek SEEK> LANGULUS(INLINED)
   auto Hierarchy::SeekUnitExt(DMeta type, const Many& ext, Index offset) -> A::Unit* {
      for (auto owner : *this) {
         A::Unit* result = owner->template SeekUnitExt<SEEK>(type, ext, offset);
         if (result)
            return result;
      }

      return nullptr;
   }

   /// Seek a unit with specific properties                                   
   /// Scan a locally provided descriptor first                               
   ///   @tparam SEEK - the direction in which we're scanning the hierarchy   
   ///   @param type - type of unit to search for                             
   ///   @param aux - local descriptor to seek through first                  
   ///   @param ext - the unit properties to seek for                         
   ///   @param offset - the index of the unit to return                      
   ///   @return a pointer to the found unit, or nullptr if not found         
   template<Seek SEEK> LANGULUS(INLINED)
   auto Hierarchy::SeekUnitAuxExt(DMeta type, const Many& aux, const Many& ext, Index offset) -> A::Unit* {
      // Scan descriptor even if hierarchy is empty                     
      A::Unit* result {};
      aux.ForEachDeep([&](const A::Unit* u) {
         if (u->CastsTo(type)) {
            //TODO check construct arguments
            // Found match                                              
            if (offset == 0) {
               // We're done                                            
               result = const_cast<A::Unit*>(u);
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
   
   /// Find a trait, searching into the hierarchy (const)                     
   ///   @tparam SEEK - direction to search at                                
   ///   @param aux - descriptor to search through                            
   ///   @param meta - the trait type to search for                           
   ///   @param offset - the number of the matching trait to use              
   ///   @return the trait, which is not empty, if trait was found            
   template<Seek SEEK> LANGULUS(INLINED)
   auto Hierarchy::SeekTraitAux(const Many& aux, TMeta meta, Index offset) -> Trait {
      // Scan descriptor                                                
      Trait result;
      aux.ForEachDeep([&](const Trait& trait) {
         if (trait.IsTrait(meta)) {
            if (offset == 0) {
               // Match found                                           
               result = trait;
               return Loop::Break;
            }
            
            --offset;
         }

         return Loop::Continue;
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
   ///   @param aux - descriptor to search through                            
   ///   @param meta - the trait type to search for                           
   ///   @param output - [out] the output                                     
   ///   @param offset - the number of the matching trait to use              
   ///   @return the trait, which is not empty, if trait was found            
   template<Seek SEEK> LANGULUS(INLINED)
   bool Hierarchy::SeekValueAux(TMeta meta, const Many& aux, CT::Data auto& output, Index offset) const {
      using D = Deref<decltype(output)>;

      if constexpr (CT::Pinnable<D>) {
         // Never touch pinned values                                   
         if (output.mLocked)
            return false;
      }

      // Scan descriptor                                                
      if constexpr (SEEK & Seek::Here) {
         bool done = false;
         if (meta) {
            aux.ForEachTrait([&](const Trait& trait) -> LoopControl {
               if (trait.IsTrait(meta)) {
                  // Found match                                        
                  try {
                     if (CT::Pinnable<D> and trait.Is<TypeOf<D>>())
                        output = trait.As<TypeOf<D>>();
                     else if (not CT::Pinnable<D> and trait.Is<D>())
                        output = trait.As<D>();
                     else if constexpr (CT::DescriptorMakable<D>)
                        output = D {Describe(static_cast<const Many&>(trait))};
                     else if constexpr (CT::Pinnable<D>)
                        output = trait.template AsCast<TypeOf<D>>();
                     else
                        output = trait.template AsCast<D>();

                     // Didn't throw, but we're done only if offset     
                     // matches                                         
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
            aux.ForEachDeep([&](const Many& group) -> LoopControl {
               try {
                  // Found match if these don't throw                   
                  if (CT::Pinnable<D> and group.Is<TypeOf<D>>())
                     output = group.As<TypeOf<D>>();
                  else if (not CT::Pinnable<D> and group.Is<D>())
                     output = group.As<D>();
                  else if constexpr (CT::DescriptorMakable<D>)
                     output = D {Describe(group)};
                  else if constexpr (CT::Pinnable<D>)
                     output = group.template AsCast<TypeOf<D>>();
                  else
                     output = group.template AsCast<D>();

                  // Didn't throw, but we're done only if offset matches
                  done = offset == 0;
                  --offset;
                  return not done;
               }
               catch (...) {}

               return Loop::Continue;
            });
         }

         if (done) {
            // Trait was found in the descriptor, which means our intent
            // is to set it to a custom value - pin it, so it doesn't   
            // get overwritten on update/refresh                        
            if constexpr (CT::Pinnable<D>)
               output.mLocked = true;
            return true;
         }
      }

      // If reached, then no trait was found in the descriptor          
      // Let's delve into the hierarchy, by scanning for Traits::Parent 
      // and Traits::Child inside the 'aux'                             
      //return SeekValue<SEEK>(meta, output, offset);
      bool done = false;
      if constexpr (SEEK & Seek::Above) {
         aux.ForEachTrait([&](const Traits::Parent& trait) -> LoopControl {
            trait.ForEach([&](const Thing& parent) {
               if (parent.SeekValue<Seek::HereAndAbove>(meta, output, offset)) {
                  // Value was found                                    
                  done = true;
                  return Loop::Break;
               }
               return Loop::Continue;
            });
            return not done;
         });
      }

      if constexpr (SEEK & Seek::Below) {
         aux.ForEachTrait([&](const Traits::Child& trait) -> LoopControl {
            trait.ForEach([&](const Thing& child) {
               if (child.SeekValue<Seek::HereAndBelow>(meta, output, offset)) {
                  // Value was found                                    
                  done = true;
                  return Loop::Break;
               }
               return Loop::Continue;
            });
            return not done;
         });
      }

      return done;
   }

} // namespace Langulus::Entity