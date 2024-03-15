///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#include "Unit.hpp"
#include "Thing.inl"
#include "Unit-Gather.inl"
#include "Unit-Seek.inl"

using namespace Langulus::A;


/// Manual construction                                                       
///   @param classid - type of the unit                                       
Unit::Unit(DMeta classid) noexcept
   : Resolvable {classid} {}

/// Move unit                                                                 
///   @param other - the unit to move                                         
Unit::Unit(Unit&& other) noexcept
   : Resolvable {Forward<Resolvable>(other)}
   , mOwners {::std::move(other.mOwners)} {
   // Replace the owner's unit pointer with the new one                 
   TODO();
   /*for (auto owner : mOwners)
      owner->ReplaceUnit(&other, this);*/
}

/// Default unit selection simply relays to the owner                         
///   @param verb - the selection verb                                        
void Unit::Select(Flow::Verb& verb) {
   for (auto owner : mOwners)
      owner->Select(verb);
}

/// Move operator                                                             
///   @param other - unit to move                                             
Unit& Unit::operator = (Unit&& other) noexcept {
   Resolvable::operator = (Forward<Resolvable>(other));
   mOwners = Move(other.mOwners);

   // Update all coupled owners                                         
   TODO();
   /*for (auto owner : mOwners)
      owner->ReplaceUnit(&other, this);*/
   return *this;
}
   
/// Check if this unit has a given set of properties                          
///   @param descriptor - descriptor with required properties                 
///   @return true if the unit has the given properties                       
bool Unit::CompareDescriptor(const Neat& descriptor) const {
   // First we compare traits only, all of them must be present         
   bool mismatch {};
   Offset memberOffset {};
   descriptor.ForEach([&](const Anyness::Trait& trait) {
      if (not GetMember(trait.GetTrait(), memberOffset)
      .Compare(static_cast<const Any&>(trait))) {
         mismatch = true;
         return Loop::Break;
      }

      ++memberOffset;
      return Loop::Continue;
   });

   // Then we run another check, based on data types, again, all        
   // of them must be present, either in trait, or in other form        
   memberOffset = {};
   descriptor.ForEachTail([&](const Anyness::Block& anythingElse) {
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
const Hierarchy& Unit::GetOwners() const noexcept {
   return mOwners;
}
     
/// Refresh unit on environment change                                        
void Unit::Refresh() {}

/// Get the runtime                                                           
///   @attention assumes units are correctly coupled and coupling to          
///              different runtimes should be explicitly disallowed           
///   @return a pointer to the runtime, if available                          
Runtime* Unit::GetRuntime() const noexcept {
   if (not mOwners)
      return nullptr;
   return mOwners[0]->GetRuntime();
}

/// Couple the component with an entity, extracted from a descriptor's        
/// Traits::Parent, if any was defined (always two-sided)                     
/// This will call refresh to all units in that entity on next tick           
///   @param descriptor - the descriptor to scan for parents                  
void Unit::Couple(const Neat& descriptor) {
   // Couple any Thing provided in the descriptor                       
   const Thing* owner = nullptr;
   if (not descriptor.ExtractTrait<Traits::Parent>(owner))
      descriptor.ExtractData(owner);

   if (owner and mOwners.Merge(IndexBack, const_cast<Thing*>(owner)))
      const_cast<Thing*>(owner)->AddUnit<false>(this);
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