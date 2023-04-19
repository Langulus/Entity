///                                                                           
/// Langulus::Entity                                                          
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#include "Thing.inl"
#include "Unit.inl"

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
   Unit::~Unit() SAFETY_NOEXCEPT() {
      // The unit might be on the stack, make sure we decouple it from  
      // all its owners, if that's the case                             
      for (auto owner = mOwners.begin(); owner != mOwners.end() && GetReferences() > 1; ++owner) {
         owner->RemoveUnit<false>(this);
         //owner = mOwners.RemoveIndex(owner);
      }

      // Then, the unit should have exactly one reference left          
      LANGULUS_ASSUME(DevAssumes, GetReferences() < 2,
         "Unit destroyed while still in use");
      LANGULUS_ASSUME(DevAssumes, GetReferences() > 0,
         "Unit destroyed at zero reference hints at potential undefined behavior");
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
   ///   @param messyDescriptor - descriptor with required properties         
   ///   @return true if the unit has the given properties                    
   bool Unit::CompareDescriptor(const Block& messyDescriptor) const {
      bool mismatch {};
      Offset memberOffset {};
      messyDescriptor.ForEachDeep(
         [&](const Trait& trait) {
            if (!GetMember(trait.GetTrait(), memberOffset).Compare(trait)) {
               mismatch = true;
               return false;
            }

            ++memberOffset;
            return true;
         }
      );

      memberOffset = {};
      messyDescriptor.ForEachDeep(
         [&](const Block& anythingElse) {
            if (!GetMember(nullptr, memberOffset).Compare(anythingElse)) {
               mismatch = true;
               return false;
            }

            ++memberOffset;
            return true;
         }
      );

      return !mismatch;
   }

} // namespace Langulus::Entity
