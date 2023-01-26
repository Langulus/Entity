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

   /// Get the list of unit owners                                            
   ///   return the owners                                                    
   LANGULUS(ALWAYSINLINE)
   const Hierarchy& Unit::GetOwners() const noexcept {
      return mOwners;
   }

   /// Get the runtime                                                        
   ///   @attention assumes units are correctly coupled and coupling to       
   ///              different runtimes should be explicitly disallowed        
   ///   @return a pointer to the runtime, if available                       
   LANGULUS(ALWAYSINLINE)
   Runtime* Unit::GetRuntime() const noexcept {
      if (mOwners.IsEmpty())
         return nullptr;
      return mOwners[0]->GetRuntime();
   }

   /// Couple the component with an entity (always two-sided)                 
   /// This will call refresh to all units in that entity on next tick        
   ///   @param entity - the entity to couple with                            
   LANGULUS(ALWAYSINLINE)
   void Unit::Couple(const Thing* entity) {
      if (!entity)
         return;
      mOwners <<= const_cast<Thing*>(entity);
      const_cast<Thing*>(entity)->AddUnit<false>(this);
   }

   /// Decouple the component from an entity (always two-sided)               
   /// This will call refresh to all units in that entity on next frame       
   ///   @param entity - the entity to decouple with                          
   LANGULUS(ALWAYSINLINE)
   void Unit::Decouple(const Thing* entity) {
      if (!entity)
         return;
      mOwners.RemoveValue(entity);
      const_cast<Thing*>(entity)->RemoveUnit<false>(this);
   }

   /// Replace one owner instance with another (used when moving things)      
   ///   @attention assumes both pointers are different, and not nullptr      
   ///   @param replaceThis - owner to replace                                
   ///   @param withThis - entity to replace it with                          
   LANGULUS(ALWAYSINLINE)
   void Unit::ReplaceOwner(const Thing* replaceThis, const Thing* withThis) {
      LANGULUS_ASSUME(DevAssumes, replaceThis != withThis, "Pointers are the same");
      LANGULUS_ASSUME(DevAssumes, replaceThis && withThis, "Nullptr not allowed");

      const auto found = mOwners.Find(replaceThis);
      if (found)
         mOwners[found] = const_cast<Thing*>(withThis);
   }

} // namespace Langulus::Entity