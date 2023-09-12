///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#include "Thing.inl"

namespace Langulus::Entity
{

   /// Manual construction                                                    
   ///   @param classid - type of the unit                                    
   ///   @param descriptor - the unit descriptor, used to extract unit owner  
   Unit::Unit(DMeta classid, const Neat& descriptor) noexcept
      : Resolvable {classid} {
      // Couple any Thing provided in the descriptor                    
      const Thing* owner = nullptr;
      if (not descriptor.ExtractTrait<Traits::Parent>(owner))
         descriptor.ExtractData(owner);
      Couple(owner);
   }

   /// Move unit                                                              
   ///   @param other - the unit to move                                      
   Unit::Unit(Unit&& other) noexcept
      : Resolvable {Forward<Resolvable>(other)}
      , mOwners {Move(other.mOwners)} {
      // Replace the owner's unit pointer with the new one              
      for (auto owner : mOwners)
         owner->ReplaceUnit(&other, this);
   }

   /// Unit destruction                                                       
   Unit::~Unit() {
      // The unit might be on the stack, make sure we decouple it from  
      // all its owners, if that's the case                             
      if (GetReferences() > 1) {
         for (auto owner : mOwners)
            owner->RemoveUnit<false>(this);
      }

      // Then, the unit should have exactly one reference left          
      LANGULUS_ASSUME(DevAssumes, GetReferences() < 2,
         "Unit destroyed while still in use");
      //LANGULUS_ASSUME(DevAssumes, GetReferences() > 0,
      //   "Unit destroyed at zero reference hints at potential undefined behavior");
   }

   /// Default unit selection simply relays to the owner                      
   ///   @param verb - the selection verb                                     
   void Unit::Select(Verb& verb) {
      for (auto owner : mOwners)
         owner->Select(verb);
   }

   /// Move operator                                                          
   ///   @param other - unit to move                                          
   Unit& Unit::operator = (Unit&& other) noexcept {
      Resolvable::operator = (Forward<Resolvable>(other));
      mOwners = Move(other.mOwners);

      // Update all coupled owners                                      
      for (auto owner : mOwners)
         owner->ReplaceUnit(&other, this);
      return *this;
   }
   
   /// Check if this unit has a given set of properties                       
   ///   @param descriptor - descriptor with required properties              
   ///   @return true if the unit has the given properties                    
   bool Unit::CompareDescriptor(const Neat& descriptor) const {
      // First we compare traits only, all of them must be present      
      bool mismatch {};
      Offset memberOffset {};
      descriptor.ForEach([&](const Trait& trait) {
         if (not GetMember(trait.GetTrait(), memberOffset).Compare(trait)) {
            mismatch = true;
            return Flow::Break;
         }

         ++memberOffset;
         return Flow::Continue;
      });

      // Then we run another check, based on data types, again, all     
      // of them must be present, either in trait, or in other form     
      memberOffset = {};
      descriptor.ForEachTail([&](const Block& anythingElse) {
         if (not GetMember(nullptr, memberOffset).Compare(anythingElse)) {
            mismatch = true;
            return Flow::Break;
         }

         ++memberOffset;
         return Flow::Continue;
      });

      return not mismatch;
   }
   
   /// Get the list of unit owners                                            
   ///   return the owners                                                    
   const Hierarchy& Unit::GetOwners() const noexcept {
      return mOwners;
   }

   /// Get the runtime                                                        
   ///   @attention assumes units are correctly coupled and coupling to       
   ///              different runtimes should be explicitly disallowed        
   ///   @return a pointer to the runtime, if available                       
   Runtime* Unit::GetRuntime() const noexcept {
      if (not mOwners)
         return nullptr;
      return mOwners[0]->GetRuntime();
   }

   /// Couple the component with an entity (always two-sided)                 
   /// This will call refresh to all units in that entity on next tick        
   ///   @param entity - the entity to couple with                            
   void Unit::Couple(const Thing* entity) {
      if (not entity)
         return;

      if (mOwners.Merge(const_cast<Thing*>(entity)))
         const_cast<Thing*>(entity)->AddUnit<false>(this);
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

} // namespace Langulus::Entity
