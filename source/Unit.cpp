///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Unit.hpp"
#include "Thing.inl"
#include "Unit-Gather.inl"
#include "Unit-Seek.inl"

using namespace Langulus::A;


/// Unit destructor decouples unit from all owners                            
Unit::~Unit() {
   for (auto entity : mOwners)
      Decouple(entity);
}

/// Default unit selection simply relays to the owner                         
///   @param verb - the selection verb                                        
void Unit::Select(Flow::Verb& verb) {
   for (auto owner : mOwners)
      owner->Select(verb);
}
   
/// Check if this unit has a given set of properties                          
///   @param descriptor - descriptor with required properties                 
///   @return true if the unit has the given properties                       
bool Unit::CompareDescriptor(const Many& descriptor) const {
   // First we compare traits only, all of them must be present         
   bool mismatch = false;
   Offset memberOffset = 0;
   descriptor.ForEachDeep([&](const Anyness::Trait& trait) {
      if (not GetMember(trait.GetTrait(), memberOffset)
      .Compare(static_cast<const Many&>(trait))) {
         mismatch = true;
         return Loop::Break;
      }

      ++memberOffset;
      return Loop::Continue;
   });

   // Then we run another check based on data types, again - all        
   // of them must be present, either as trait or in other form         
   memberOffset = 0;
   descriptor.ForEachDeep([&](const Many& anythingElse) {
      if (not GetMember(TMeta {}, memberOffset).Compare(anythingElse)) {
         mismatch = true;
         return Loop::Break;
      }

      ++memberOffset;
      return Loop::Continue;
   });

   return not mismatch;
}
   
/// Get the list of unit owners                                               
///   @return the owners                                                      
auto Unit::GetOwners() const noexcept -> const Hierarchy& {
   return mOwners;
}
     
/// Refresh unit on environment change                                        
void Unit::Refresh() {}

/// Get the runtime                                                           
///   @attention assumes units are correctly coupled and coupling to          
///              different runtimes should be explicitly disallowed           
///   @return a pointer to the runtime, if available                          
auto Unit::GetRuntime() const noexcept -> Runtime* {
   if (not mOwners)
      return nullptr;
   return &*mOwners[0]->GetRuntime();
}

/// Couple the component with an entity, extracted from a descriptor's        
/// Traits::Parent, if any was defined (always two-sided)                     
/// This will call refresh to all units in that entity on next tick           
///   @param desc - the descriptor to scan for parents                        
///   @param fallback - a fallback Thing to couple to (optional)              
///      This usually comes from the producer's context. For example, if you  
///      don't provide a parent for the renderer, it will be instantiated as  
///      a child to the graphics module owner (i.e. the runtime owner)        
void Unit::Couple(const Many& desc, const Thing* fallback) {
   const Thing* owner = nullptr;
   if (not desc.ExtractTrait<Traits::Parent>(owner))
      desc.ExtractData(owner);

   if (owner and mOwners.Merge(IndexBack, const_cast<Thing*>(owner)))
      const_cast<Thing*>(owner)->AddUnit<false>(this);
   else if (fallback)
      const_cast<Thing*>(fallback)->AddUnit<false>(this);
}

/// Decouple the component from an entity (always two-sided)                  
/// This will call refresh to all units in that entity on next frame          
///   @param entity - the entity to decouple with                             
void Unit::Decouple(const Thing* entity) {
   if (not entity)
      return;
      
   if (mOwners.Remove(entity))
      const_cast<Thing*>(entity)->RemoveUnit<false>(this);
}

/// Replace one owner instance with another (used when moving things)         
///   @attention assumes both pointers are different, and not nullptr         
///   @param replaceThis - owner to replace                                   
///   @param withThis - entity to replace it with                             
void Unit::ReplaceOwner(const Thing* replaceThis, const Thing* withThis) {
   LANGULUS_ASSUME(DevAssumes, replaceThis != withThis,
      "Pointers are the same");
   LANGULUS_ASSUME(DevAssumes, replaceThis and withThis,
      "Nullptr not allowed");

   const auto found = mOwners.Find(replaceThis);
   if (found)
      mOwners[found] = const_cast<Thing*>(withThis);
}