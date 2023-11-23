///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#include "Thing.inl"
#include "Thing-Seek.inl"
#include "Pin.inl"

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

   /// Get a trait by type (const)                                            
   ///   @param trait - trait id                                              
   ///   @param offset - offset of result to use                              
   ///   @return a filled trait if fount, empty if not                        
   Trait Thing::GetTrait(TMeta trait, const Index& offset) const {
      return GetTrait(Trait::FromMeta(trait, nullptr), offset);
   }

   /// Get a trait by type                                                    
   ///   @param trait - trait id                                              
   ///   @param offset - offset of result to use                              
   ///   @return a filled trait if fount, empty if not                        
   Trait Thing::GetTrait(TMeta trait, const Index& offset) {
      return GetTrait(Trait::FromMeta(trait, nullptr), offset);
   }

   /// Get a trait from this entity's trait list                              
   ///   @param id - the trait to search for                                  
   ///   @param index - the index of the trait we seek                        
   ///   @return a pointer to the trait, or nullptr if not found              
   Trait* Thing::GetLocalTrait(TMeta id, const Index& index) {
      if (id) {
         // Search a typed trait                                        
         const auto found = mTraits.FindIt(id);
         if (found)
            return &found->mValue[index];
         return nullptr;
      }

      // Search trait by index                                          
      Trait* found {};
      if (index.IsArithmetic()) {
         auto offset = index.GetOffsetUnsafe();
         mTraits.ForEachValue([&](TAny<Trait>& list) noexcept {
            if (offset < list.GetCount()) {
               found = &list[offset];
               return Flow::Break;
            }

            offset -= list.GetCount();
            return Flow::Continue;
         });
      }

      return found;
   }

   /// Get a trait from the entity's trait list   (const)                     
   ///   @param id - the trait to search for                                  
   ///   @param offset - the index of the trait we seek                       
   ///   @return a pointer to the trait, or nullptr if not found              
   const Trait* Thing::GetLocalTrait(TMeta id, const Index& offset) const {
      return const_cast<Thing&>(*this).GetLocalTrait(id, offset);
   }

   /// Get a trait by type and contents                                       
   ///   @param id - trait to match                                           
   ///   @param index - offset of result to use                               
   ///   @return a non-empty trait, if found                                  
   Trait Thing::GetTrait(const Trait& id, const Index& index) {
      if (id.GetTrait()) {
         // Handle some predefined traits here                          
         if (id.template TraitIs<Traits::Unit>()) {
            // Get a component                                          
            auto unit = GetUnitMeta(nullptr, index);
            if (unit)
               return Traits::Unit {unit};
            return {};
         }
         else if (id.template TraitIs<Traits::Child>()) {
            // Get a child entity                                       
            auto child = GetChild(index);
            if (child)
               return Traits::Child {child};
            return {};
         }
         else if (id.template TraitIs<Traits::Runtime>()) {
            // Get the nearest runtime                                  
            return Traits::Runtime {mRuntime->Get()};
         }
         else if (id.template TraitIs<Traits::Parent>()) {
            // Get the parent                                           
            return Traits::Parent {mOwner.Get()};
         }
      }

      // Check dynamic traits in the entity                             
      auto found = GetLocalTrait(id.GetTrait(), index);
      if (found)
         return *found;

      // Then check each unit's static traits                           
      //TODO, hm, this feeds back onto units, so we should probably expose
      //traits explicitly from the unit itself, in order for them to be reachable
      //this way we also save on this whole loop with many indirections
      /*Trait output;
      for (auto& unit : mUnitsList) {
         auto t = unit->GetMember(id.GetTrait(), index);
         if (t) {
            output = Trait::From(id.GetTrait(), t);
            break;
         }
      }

      if (output)
         return Abandon(output); */

      // Finally, check this entity's static traits                     
      //TODO shouldn't this be handled at the beginning of this func?
      /*auto t = GetMember(id.GetTrait(), index);
      if (t)
         return Trait::From(id.GetTrait(), t);*/

      // Nothing was found                                              
      return {};
   }

   /// Get a trait by type and contents (const)                               
   ///   @param id - trait to match                                           
   ///   @param index - offset of result to use                               
   ///   @return a non-empty trait, if found                                  
   Trait Thing::GetTrait(const Trait& id, const Index& index) const {
      return const_cast<Thing*>(this)->GetTrait(id, index);
   }

   /// Add a new trait to the thing                                           
   ///   @param trait - trait to shallow copy                                 
   ///   @return the new trait instance                                       
   Trait* Thing::AddTrait(const Trait& trait) {
      const auto tmeta = trait.GetTrait();
      auto found = mTraits.FindIt(tmeta);
      if (found) {
         found->mValue << trait;
         return &found->mValue.Last();
      }

      mTraits.Insert(tmeta, trait);
      mRefreshRequired = true;
      ENTITY_VERBOSE(trait << " added");
      return &mTraits[tmeta].Last();
   }

   /// Remove a trait from the universal entity                               
   ///   @param trait - type of trait to remove                               
   ///   @return the number of removed traits                                 
   Count Thing::RemoveTrait(TMeta trait) {
      const auto found = mTraits.FindIt(trait);
      if (found) {
         const auto removed = found->mValue.GetCount();
         mTraits.RemoveIt(found);
         ENTITY_VERBOSE_SELF(trait << " removed");
         mRefreshRequired = true;
         return removed;
      }

      return 0;
   }

   /// Remove an exact-matching trait from this entity                        
   ///   @param trait - type and value to remove                              
   ///   @return the number of removed traits                                 
   Count Thing::RemoveTrait(const Trait& trait) {
      const auto found = mTraits.FindIt(trait.GetTrait());
      if (found) {
         const auto removed = found->mValue.Remove(trait);
         if (removed) {
            ENTITY_VERBOSE_SELF(trait << " removed");
            mRefreshRequired = true;
            return removed;
         }
      }

      return 0;
   }

   /// A fast check whether traits of the given type are inside this entity   
   ///   @param trait - type of trait to check                                
   ///   @return the number of matching traits                                
   Count Thing::HasTraits(TMeta trait) const {
      const auto found = mTraits.FindIt(trait);
      return found ? found->mValue.GetCount() : 0;
   }

   /// A fast check whether traits of the given type and value are inside     
   ///   @param trait - trait to search for                                   
   ///   @return the number of matching traits                                
   Count Thing::HasTraits(const Trait& trait) const {
      const auto found = mTraits.FindIt(trait.GetTrait());
      if (not found)
         return 0;

      Count counter {};
      for (auto& trait : found->mValue) {
         if (trait == trait)
            ++counter;
      }
      return counter;
   }
   
   /// Get traits                                                             
   ///   @return the map of traits                                            
   LANGULUS_API(ENTITY)
   const TraitMap& Thing::GetTraits() const noexcept {
      return mTraits;
   }

   /// Add/overwrite entity's name trait                                      
   ///   @param name - the name to set                                        
   void Thing::SetName(const Text& name) {
      auto found = GetLocalTrait<Traits::Name>();
      if (not found)
         AddTrait(Traits::Name {name});
      else
         *found = name;
   }

   /// Get entity's name trait, if any                                        
   /// It can reside in one of the units                                      
   ///   @return the name, or empty string if no such trait was found here    
   Text Thing::GetName() const {
      Text name;
      SeekValue<Traits::Name, Seek::Here>(name);
      return name;
   }
   
} // namespace Langulus::Entry
