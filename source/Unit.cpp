///                                                                           
/// Langulus::Entity                                                          
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#include "Thing.hpp"

namespace Langulus::Entity
{

   /// Manual construction                                                    
   ///   @param classid - type of the unit                                    
   ///   @param descriptor - the unit descriptor, used to extract unit owner  
   Unit::Unit(DMeta classid, const Any& descriptor) noexcept
      : Resolvable {classid} {
      // Couple any Thing provided in the descriptor                    
      descriptor.ForEachDeep(
         [this](const Trait& trait) {
            if (trait.TraitIs<Traits::Parent>()) {
               trait.ForEach([this](const Thing* owner) {
                  Couple(owner);
               });
            }
         },
         [this](const Thing* owner) {
            Couple(owner);
         }
      );
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
      // Decouple from all owners                                       
      for (auto owner : mOwners)
         owner->RemoveUnit<false>(this);

      // Then, the unit should have exactly one reference left          
      LANGULUS_ASSERT(GetReferences() < 2, Destruct,
         "Unit destroyed while still in use");
      LANGULUS_ASSERT(GetReferences() > 0, Destruct,
         "Unit destroyed at zero reference hints at potential undefined behavior");
   }

   /// Default unit selection simply relays to the owner                      
   ///   @param verb - the selection verb                                     
   void Unit::Select(Verb& verb) {
      for (auto owner : mOwners)
         owner->Select(verb);
   }

   /// Get the runtime                                                        
   ///   @attention assumes units are correctly coupled and coupling to       
   ///              different runtimes is never allowed                       
   ///   @return a pointer to the runtime, if available                       
   Runtime* Unit::GetRuntime() const noexcept {
      if (mOwners.IsEmpty())
         return nullptr;
      return mOwners[0]->GetRuntime();
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
   
   /// Couple the component with an entity (always two-sided)                 
   /// This will call refresh to all units in that entity on next tick        
   ///   @param entity - the entity to couple with                            
   void Unit::Couple(const Thing* entity) {
      if (!entity)
         return;
      mOwners <<= const_cast<Thing*>(entity);
      const_cast<Thing*>(entity)->AddUnit<false>(this);
   }

   /// Decouple the component from an entity (always two-sided)               
   /// This will call refresh to all units in that entity on next frame       
   ///   @param entity - the entity to decouple with                          
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
   void Unit::ReplaceOwner(const Thing* replaceThis, const Thing* withThis) {
      LANGULUS_ASSUME(DevAssumes, replaceThis != withThis, "Pointers are the same");
      LANGULUS_ASSUME(DevAssumes, replaceThis && withThis, "Nullptr not allowed");

      const auto found = mOwners.Find(replaceThis);
      if (found)
         mOwners[found] = const_cast<Thing*>(withThis);
   }

} // namespace Langulus::Entity
