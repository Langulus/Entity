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

#define TEMPLATE() template<class THIS>
#define TME() SeekInterface<THIS>


namespace Langulus::Entity
{

   /// Find a unit by type and optional offset                                
   ///   @tparam SEEK - the direction to seek in                              
   ///   @param type - the type of unit to search for                         
   ///   @param offset - the match to return                                  
   ///   @return a pointer to the found unit, or nullptr if not found         
   TEMPLATE() template<Seek SEEK> LANGULUS(INLINED)
   const Unit* TME()::SeekUnit(DMeta type, Index offset) const {
      return const_cast<THIS*>(static_cast<const THIS*>(this))
         ->template SeekUnit<SEEK>(type, offset);
   }

   TEMPLATE() template<CT::Data T, Seek SEEK> LANGULUS(INLINED)
   Decay<T>* TME()::SeekUnit(Index offset) {
      return static_cast<Decay<T>*>(static_cast<THIS*>(this)
         ->template SeekUnit<SEEK>(MetaData::Of<Decay<T>>(), offset));
   }

   TEMPLATE() template<CT::Data T, Seek SEEK> LANGULUS(INLINED)
   const Decay<T>* TME()::SeekUnit(Index offset) const {
      return const_cast<TME()*>(this)
         ->template SeekUnit<T, SEEK>(offset);
   }

   /// Find a unit by type and optional offset, but search first in an        
   /// auxiliary descriptor                                                   
   ///   @tparam SEEK - the direction to seek in                              
   ///   @param aux - the auxiliary descriptor to scan first                  
   ///   @param type - the type of unit to search for                         
   ///   @param offset - the match to return                                  
   ///   @return a pointer to the found unit, or nullptr if not found         
   TEMPLATE() template<Seek SEEK> LANGULUS(INLINED)
   const Unit* TME()::SeekUnitAux(const Neat& aux, DMeta type, Index offset) const {
      return const_cast<THIS*>(static_cast<const THIS*>(this))
         ->template SeekUnitAux<SEEK>(aux, type, offset);
   }

   TEMPLATE() template<CT::Data T, Seek SEEK> LANGULUS(INLINED)
   Decay<T>* TME()::SeekUnitAux(const Neat& aux, Index offset) {
      return static_cast<Decay<T>*>(static_cast<THIS*>(this)
         ->template SeekUnitAux<SEEK>(aux, MetaData::Of<Decay<T>>(), offset));
   }

   TEMPLATE() template<CT::Data T, Seek SEEK> LANGULUS(INLINED)
   const Decay<T>* TME()::SeekUnitAux(const Neat& aux, Index offset) const {
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
   const Unit* TME()::SeekUnitExt(DMeta type, const Neat& ext, Index offset) const {
      return const_cast<THIS*>(static_cast<const THIS*>(this))
         ->template SeekUnitExt<SEEK>(type, ext, offset);
   }

   TEMPLATE() template<CT::Data T, Seek SEEK> LANGULUS(INLINED)
   Decay<T>* TME()::SeekUnitExt(const Neat& ext, Index offset) {
      return static_cast<Decay<T>*>(static_cast<THIS*>(this)
         ->template SeekUnitExt<SEEK>(MetaData::Of<Decay<T>>(), ext, offset));
   }

   TEMPLATE() template<CT::Data T, Seek SEEK> LANGULUS(INLINED)
   const Decay<T>* TME()::SeekUnitExt(const Neat& ext, Index offset) const {
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
   const Unit* TME()::SeekUnitAuxExt(DMeta type, const Neat& aux, const Neat& ext, Index offset) const {
      return const_cast<THIS*>(static_cast<const THIS*>(this))
         ->template SeekUnitAuxExt<SEEK>(type, aux, ext, offset);
   }

   TEMPLATE() template<CT::Data T, Seek SEEK> LANGULUS(INLINED)
   Decay<T>* TME()::SeekUnitAuxExt(const Neat& aux, const Neat& ext, Index offset) {
      return static_cast<Decay<T>*>(static_cast<THIS*>(this)
         ->template SeekUnitAuxExt<SEEK>(MetaData::Of<Decay<T>>(), aux, ext, offset));
   }

   TEMPLATE() template<CT::Data T, Seek SEEK> LANGULUS(INLINED)
   const Decay<T>* TME()::SeekUnitAuxExt(const Neat& aux, const Neat& ext, Index offset) const {
      return const_cast<TME()*>(this)
         ->template SeekUnitAuxExt<T, SEEK>(aux, ext, offset);
   }
      
   /// Find a trait by type and optional offset                               
   ///   @tparam SEEK - the direction to seek in                              
   ///   @param type - the type of unit to search for                         
   ///   @param offset - the match to return                                  
   ///   @return a pointer to the found unit, or nullptr if not found         
   TEMPLATE() template<Seek SEEK> LANGULUS(INLINED)
   Trait TME()::SeekTrait(TMeta type, Index offset) const {
      return const_cast<THIS*>(static_cast<const THIS*>(this))
         ->template SeekTrait<SEEK>(type, offset);
   }

   TEMPLATE() template<CT::Trait T, Seek SEEK> LANGULUS(INLINED)
   Trait TME()::SeekTrait(Index offset) {
      return static_cast<THIS*>(this)
         ->template SeekTrait<SEEK>(T::GetTrait(), offset);
   }

   TEMPLATE() template<CT::Trait T, Seek SEEK> LANGULUS(INLINED)
   Trait TME()::SeekTrait(Index offset) const {
      return const_cast<TME()*>(this)
         ->template SeekTrait<T, SEEK>(offset);
   }

   /// Find a trait by type and specific properties and optional offset       
   ///   @tparam SEEK - the direction to seek in                              
   ///   @param ext - the properties of the unit to search for                
   ///   @param type - the type of trait to search for                        
   ///   @param offset - the match to return                                  
   ///   @return a pointer to the found unit, or nullptr if not found         
   TEMPLATE() template<Seek SEEK> LANGULUS(INLINED)
   Trait TME()::SeekTraitAux(const Neat& aux, TMeta type, Index offset) const {
      return const_cast<THIS*>(static_cast<const THIS*>(this))
         ->template SeekTraitAux<SEEK>(aux, type, offset);
   }

   TEMPLATE() template<CT::Trait T, Seek SEEK> LANGULUS(INLINED)
   Trait TME()::SeekTraitAux(const Neat& aux, Index offset) {
      return static_cast<THIS*>(this)
         ->template SeekTraitAux<SEEK>(aux, T::GetTrait(), offset);
   }

   TEMPLATE() template<CT::Trait T, Seek SEEK> LANGULUS(INLINED)
   Trait TME()::SeekTraitAux(const Neat& aux, Index offset) const {
      return const_cast<TME()*>(this)
         ->template SeekTraitAux<T, SEEK>(aux, offset);
   }



   TEMPLATE() template<CT::Trait T, Seek SEEK> LANGULUS(INLINED)
   bool TME()::SeekValue(CT::Data auto& output, Index offset) const {
      return static_cast<const THIS*>(this)
         ->template SeekValue<SEEK>(T::GetTrait(), output, offset);
   }

   TEMPLATE() template<CT::Trait T, Seek SEEK> LANGULUS(INLINED)
   bool TME()::SeekValueAux(const Neat& aux, CT::Data auto& output, Index offset) const {
      return static_cast<const THIS*>(this)
         ->template SeekValueAux<SEEK>(T::GetTrait(), aux, output, offset);
   }
   
   #if LANGULUS_FEATURE(MANAGED_REFLECTION)
      ///                                                                     
      /// Token based interface                                               
      /// Available only when managed reflection is enabled                   
      ///                                                                     
      TEMPLATE() template<Seek SEEK> LANGULUS(INLINED)
      Unit* TME()::SeekUnit(const Token& dataToken, Index offset) {
         return static_cast<THIS*>(this)
            ->template SeekUnit<SEEK>(RTTI::GetMetaData(dataToken), offset);
      }

      TEMPLATE() template<Seek SEEK> LANGULUS(INLINED)
      const Unit* TME()::SeekUnit(const Token& dataToken, Index offset) const {
         return static_cast<THIS*>(this)
            ->template SeekUnit<SEEK>(RTTI::GetMetaData(dataToken), offset);
      }

      TEMPLATE() template<Seek SEEK> LANGULUS(INLINED)
      Unit* TME()::SeekUnitAux(const Neat& aux, const Token& dataToken, Index offset) {
         return static_cast<THIS*>(this)
            ->template SeekUnitAux<SEEK>(aux, RTTI::GetMetaData(dataToken), offset);
      }

      TEMPLATE() template<Seek SEEK> LANGULUS(INLINED)
      const Unit* TME()::SeekUnitAux(const Neat& aux, const Token& dataToken, Index offset) const {
         return static_cast<THIS*>(this)
            ->template SeekUnitAux<SEEK>(aux, RTTI::GetMetaData(dataToken), offset);
      }
      
      TEMPLATE() template<Seek SEEK> LANGULUS(INLINED)
      Trait TME()::SeekTrait(const Token& traitToken, Index offset) {
         return static_cast<THIS*>(this)
            ->template SeekTrait<SEEK>(RTTI::GetMetaTrait(traitToken), offset);
      }

      TEMPLATE() template<Seek SEEK> LANGULUS(INLINED)
      Trait TME()::SeekTrait(const Token& traitToken, Index offset) const {
         return static_cast<THIS*>(this)
            ->template SeekTrait<SEEK>(RTTI::GetMetaTrait(traitToken), offset);
      }

      TEMPLATE() template<Seek SEEK> LANGULUS(INLINED)
      Trait TME()::SeekTraitAux(const Neat& aux, const Token& traitToken, Index offset) const {
         return static_cast<THIS*>(this)
            ->template SeekTraitAux<SEEK>(aux, RTTI::GetMetaTrait(traitToken), offset);
      }

      TEMPLATE() template<Seek SEEK> LANGULUS(INLINED)
      bool TME()::SeekValue(const Token& traitToken, CT::Data auto& output, Index offset) const {
         return static_cast<THIS*>(this)
            ->template SeekValue<SEEK>(RTTI::GetMetaTrait(traitToken), output, offset);
      }
     
      TEMPLATE() template<Seek SEEK> LANGULUS(INLINED)
      bool TME()::SeekValueAux(const Token& traitToken, const Neat& aux, CT::Data auto& output, Index offset) const {
         return static_cast<THIS*>(this)
            ->template SeekValueAux<SEEK>(RTTI::GetMetaTrait(traitToken), aux, output, offset);
      }
   #endif

} // namespace Langulus::Entity

#undef TEMPLATE
#undef TME

namespace Langulus::Entity
{

   /// Find a specific unit, searching into the hierarchy                     
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @param meta - the unit to seek for                                   
   ///   @param offset - which of the matches to return                       
   ///   @return the found unit, or nullptr if no such unit was found         
   template<Seek SEEK> LANGULUS(INLINED)
   Unit* Hierarchy::SeekUnit(DMeta meta, Index offset) {
      for (auto owner : *this) {
         Unit* result = owner->template SeekUnit<SEEK>(meta, offset);
         if (result)
            return result;
      }

      return nullptr;
   }

   /// Find a unit by type and index from the hierarchy                       
   /// Scan a locally provided descriptor first                               
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param aux - descriptor to search through                            
   ///   @param meta - the type of the unit to seek for                       
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<Seek SEEK> LANGULUS(INLINED)
   Unit* Hierarchy::SeekUnitAux(const Neat& aux, DMeta meta, Index offset) {
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

   /// Seek a unit with specific properties                                   
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param type - type of unit to search for                             
   ///   @param ext - the properties of the unit to seek for                  
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<Seek SEEK> LANGULUS(INLINED)
   Unit* Hierarchy::SeekUnitExt(DMeta type, const Neat& ext, Index offset) {
      for (auto owner : *this) {
         Unit* result = owner->template SeekUnitExt<SEEK>(type, ext, offset);
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
   Unit* Hierarchy::SeekUnitAuxExt(DMeta type, const Neat& aux, const Neat& ext, Index offset) {
      // Scan descriptor even if hierarchy is empty                     
      Unit* result {};
      aux.ForEachDeep([&](const Unit* u) {
         if (u->CastsTo(type)) {
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
      return SeekUnitExt<SEEK>(type, ext, offset);
   }
   
   /// Find a trait by type (and index), searching into the hierarchy         
   ///   @tparam SEEK - direction to search at                                
   ///   @param meta - the trait to search for                                
   ///   @param offset - the offset to apply                                  
   ///   @return the trait, which is not empty, if trait was found            
   template<Seek SEEK> LANGULUS(INLINED)
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
   ///   @param aux - descriptor to search through                            
   ///   @param meta - the trait type to search for                           
   ///   @param offset - the number of the matching trait to use              
   ///   @return the trait, which is not empty, if trait was found            
   template<Seek SEEK> LANGULUS(INLINED)
   Trait Hierarchy::SeekTraitAux(const Neat& aux, TMeta meta, Index offset) {
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
   /// converting it to a desired output type                                 
   /// Supports pinnable outputs                                              
   ///   @tparam SEEK - direction to search at                                
   ///   @param meta - the trait type to search for                           
   ///   @param output - [out] the output                                     
   ///   @param offset - the number of the matching trait to use              
   ///   @return true if output was rewritten                                 
   template<Seek SEEK> LANGULUS(INLINED)
   bool Hierarchy::SeekValue(TMeta meta, CT::Data auto& output, Index offset) const {
      using D = Deref<decltype(output)>;

      if constexpr (CT::Pinnable<D>) {
         // Never touch pinned values                                   
         if (output.mLocked)
            return false;
      }

      // Let's delve into the hierarchy                                 
      for (auto owner : *this) {
         if (owner->template SeekValue<SEEK>(meta, output, offset)) {
            // Value was found                                          
            return true;
         }
      }

      // If reached, nothing was found                                  
      return false;
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
   bool Hierarchy::SeekValueAux(TMeta meta, const Neat& aux, CT::Data auto& output, Index offset) const {
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
            if (trait.TraitIs(meta)) {
               // Found match                                           
               try {
                  if constexpr (CT::DescriptorMakable<D>)
                     output = D {Describe(static_cast<const Any&>(trait))};
                  else if constexpr (CT::Pinnable<D>)
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

            return Flow::Continue;
         });
      }
      else {
         aux.ForEachDeep([&](const Block& group) {
            try {
               // Found match if these don't throw                      
               if constexpr (CT::DescriptorMakable<D>)
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
            catch(...) { }

            return Flow::Continue;
         });
      }

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
      return SeekValue<SEEK>(meta, output, offset);
   }

} // namespace Langulus::Entity