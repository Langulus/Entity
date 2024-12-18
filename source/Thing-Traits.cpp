///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Thing.hpp"
#include "Thing.inl"
#include "Thing-Seek.inl"


namespace Langulus::Entity
{

   /// Get a trait by type (const)                                            
   ///   @param trait - trait id                                              
   ///   @param offset - offset of result to use                              
   ///   @return a filled trait if fount, empty if not                        
   auto Thing::GetTrait(TMeta trait, Index offset) const -> Trait {
      return GetTrait(Trait::FromMeta(trait, nullptr), offset);
   }

   /// Get a trait by type                                                    
   ///   @param trait - trait id                                              
   ///   @param offset - offset of result to use                              
   ///   @return a filled trait if fount, empty if not                        
   auto Thing::GetTrait(TMeta trait, Index offset) -> Trait {
      return GetTrait(Trait::FromMeta(trait, nullptr), offset);
   }

   /// Get a trait from this entity's trait list                              
   ///   @param id - the trait to search for                                  
   ///   @param index - the index of the trait we seek                        
   ///   @return a pointer to the trait, or nullptr if not found              
   auto Thing::GetLocalTrait(TMeta id, Index index) -> Trait* {
      if (id) {
         // Search a typed trait                                        
         const auto found = mTraits.FindIt(id);
         if (found)
            return &(found.GetValue()[index]);
         return nullptr;
      }

      // Search trait by index                                          
      Trait* found {};
      if (index.IsArithmetic()) {
         auto offset = index.GetOffsetUnsafe();
         mTraits.ForEachValue([&](TMany<Trait>& list) noexcept {
            if (offset < list.GetCount()) {
               found = &list[offset];
               return Loop::Break;
            }

            offset -= list.GetCount();
            return Loop::Continue;
         });
      }

      return found;
   }

   /// Get a trait from the entity's trait list   (const)                     
   ///   @param id - the trait to search for                                  
   ///   @param offset - the index of the trait we seek                       
   ///   @return a pointer to the trait, or nullptr if not found              
   auto Thing::GetLocalTrait(TMeta id, Index offset) const -> const Trait* {
      return const_cast<Thing&>(*this).GetLocalTrait(id, offset);
   }

   /// Get a trait by type and contents                                       
   ///   @param id - trait to match                                           
   ///   @param index - offset of result to use                               
   ///   @return a non-empty trait, if found                                  
   auto Thing::GetTrait(const Trait& id, Index index) -> Trait {
      if (id.GetTrait()) {
         // Handle some predefined traits here                          
         if (id.template IsTrait<Traits::Unit>()) {
            // Get a component                                          
            auto unit = GetUnitMeta(DMeta {}, index);
            if (unit)
               return Traits::Unit {unit};
            return {};
         }
         else if (id.template IsTrait<Traits::Child>()) {
            // Get a child entity                                       
            auto child = GetChild(index);
            if (child)
               return Traits::Child {child};
            return {};
         }
         else if (id.template IsTrait<Traits::Runtime>()) {
            // Get the nearest runtime                                  
            return Traits::Runtime {&(*mRuntime)};
         }
         else if (id.template IsTrait<Traits::Parent>()) {
            // Get the parent                                           
            return Traits::Parent {&(*mOwner)};
         }
      }

      // Check dynamic traits in the entity                             
      auto found = GetLocalTrait(id.GetTrait(), index);
      if (found)
         return *found;

      // Nothing was found                                              
      return {};
   }

   /// Get a trait by type and contents (const)                               
   ///   @param id - trait to match                                           
   ///   @param index - offset of result to use                               
   ///   @return a non-empty trait, if found                                  
   auto Thing::GetTrait(const Trait& id, Index index) const -> Trait {
      return const_cast<Thing*>(this)->GetTrait(id, index);
   }

   /// Add a new trait to the thing                                           
   ///   @param trait - trait to shallow copy                                 
   ///   @return the new trait instance                                       
   auto Thing::AddTrait(Trait trait) -> Trait* {
      const auto tmeta = trait.GetTrait();
      auto found = mTraits.FindIt(tmeta);
      if (found) {
         found.GetValue() << trait;
         return &found.GetValue().Last();
      }

      mTraits.Insert(tmeta, trait);
      mRefreshRequired = true;
      ENTITY_VERBOSE_SELF(trait, " added");
      return &mTraits[tmeta].Last();
   }

   /// Remove a trait from the universal entity                               
   ///   @param trait - type of trait to remove                               
   ///   @return the number of removed traits                                 
   auto Thing::RemoveTrait(TMeta trait) -> Count {
      const auto found = mTraits.FindIt(trait);
      if (found) {
         const auto removed = found.GetValue().GetCount();
         mTraits.RemoveIt(found);
         ENTITY_VERBOSE_SELF(trait, " removed");
         mRefreshRequired = true;
         return removed;
      }

      return 0;
   }

   /// Remove an exact-matching trait from this entity                        
   ///   @param trait - type and value to remove                              
   ///   @return the number of removed traits                                 
   auto Thing::RemoveTrait(Trait trait) -> Count {
      const auto found = mTraits.FindIt(trait.GetTrait());
      if (found) {
         const auto removed = found.GetValue().Remove(trait);
         if (removed) {
            ENTITY_VERBOSE_SELF(trait, " removed");
            mRefreshRequired = true;
            return removed;
         }
      }

      return 0;
   }

   /// A fast check whether traits of the given type are inside this entity   
   ///   @param trait - type of trait to check                                
   ///   @return the number of matching traits                                
   auto Thing::HasTraits(TMeta trait) const -> Count {
      const auto found = mTraits.FindIt(trait);
      return found ? found.GetValue().GetCount() : 0;
   }

   /// A fast check whether traits of the given type and value are inside     
   ///   @param trait - trait to search for                                   
   ///   @return the number of matching traits                                
   auto Thing::HasTraits(const Trait& trait) const -> Count {
      const auto found = mTraits.FindIt(trait.GetTrait());
      if (not found)
         return 0;

      Count counter = 0;
      for (auto& trait : found.GetValue()) {
         if (trait == trait)
            ++counter;
      }
      return counter;
   }
   
   /// Get traits                                                             
   ///   @return the map of traits                                            
   LANGULUS_API(ENTITY)
   auto Thing::GetTraits() const noexcept -> const TraitMap& {
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
