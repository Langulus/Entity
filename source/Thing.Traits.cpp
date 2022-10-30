///                                                                           
/// Langulus::Entity                                                          
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#include "Thing.hpp"
#include "Runtime.hpp"

#define ENTITY_VERBOSE_SELF(a)            //Logger::Verbose() << this << ": "<< a
#define ENTITY_VERBOSE(a)                 //Logger::Append() << a
#define ENTITY_CREATION_VERBOSE_SELF(a)   //Logger::Verbose() << this << ": " <<a
#define ENTITY_SELECTION_VERBOSE_SELF(a)  //Logger::Verbose() << this << ": " <<a


namespace Langulus::Entity
{

   /// Get a trait by type                                                    
   ///   @param id - trait id                                                 
   ///   @param output - [out] result of the search                           
   ///   @param offset - offset of result to use                              
   ///   @return true if anything was found                                   
   bool Thing::GetTrait(TMeta id, Trait& output, const Index& offset) {
      return GetTrait(Trait::FromMeta(id, nullptr), output, offset);
   }

   /// Get a trait from this entity's trait list                              
   ///   @param id - the trait to search for                                  
   ///   @param index - the index of the trait we seek                        
   ///   @return a pointer to the trait, or nullptr if not found              
   Trait* Thing::GetLocalTrait(TMeta id, const Index& index) {
      if (id) {
         // Search a typed trait                                        
         const auto found = mTraits.FindKeyIndex(id);
         if (found)
            return &mTraits.GetValue(found)[index];
         return nullptr;
      }

      // Search trait by index                                          
      Trait* found {};
      if (index.IsArithmetic()) {
         auto offset = index.GetOffset();
         mTraits.ForEachValue([&](TAny<Trait>& list) noexcept {
            if (offset < list.GetCount()) {
               found = &list[offset];
               return false;
            }

            offset -= list.GetCount();
            return true;
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
   ///   @param output - [out] result of the search                           
   ///   @param offset - offset of result to use                              
   ///   @return true if anything was found                                   
   bool Thing::GetTrait(const Trait& id, Trait& output, const Index& index) {
      if (id.GetTrait()) {
         if (id.TraitIs<Traits::Unit>()) {
            // Get a component                                          
            auto unit = GetUnit(nullptr, index);
            if (unit) {
               output = Traits::Unit {unit};
               return true;
            }

            return false;
         }
         else if (id.TraitIs<Traits::Child>()) {
            // Get a child entity                                       
            auto child = GetChild(index);
            if (child) {
               output = Traits::Child {child};
               return true;
            }

            return false;
         }
         else if (id.TraitIs<Traits::Runtime>()) {
            // Get the nearest runtime                                  
            output = Traits::Runtime {mRuntime};
            return true;
         }
         else if (id.TraitIs<Traits::Parent>()) {
            // Get the parent                                           
            output = Traits::Parent {mOwner.Get()};
            return true;
         }
      }

      // Check dynamic traits in the entity                             
      auto found = GetLocalTrait(id.GetTrait(), index);
      if (found) {
         output = *found;
         return true;
      }

      // Then check each unit's static traits                           
      mUnits.ForEachValue([&](Unit* unit) {
         auto t = unit->GetMember(id.GetTrait(), index);
         if (!t.IsEmpty()) {
            output = Trait {id.GetTrait(), t};
            return;
         }
      });

      // Finally, check this entity's static traits                     
      auto t = GetMember(id.GetTrait(), index);
      if (!t.IsEmpty()) {
         output = Trait {id.GetTrait(), t};
         return true;
      }

      return false;
   }

   /// Add a new trait to the universal entity                                
   ///   @param initializer - trait to shallow copy                           
   ///   @return the new trait instance                                       
   Trait* Thing::AddTrait(const Trait& initializer) {
      auto& list = mTraits[initializer.GetTrait()];
      list << initializer;
      mRefreshRequired = true;
      ENTITY_VERBOSE(initializer << " added");
      return &list.Last();
   }

   /// Remove a trait from the universal entity                               
   ///   @param id - type of trait to remove                                  
   Count Thing::RemoveTrait(TMeta id) {
      const auto found = mTraits.FindKeyIndex(id);
      if (found) {
         const auto removed = mTraits.GetValue(found).GetCount();
         mTraits.RemoveIndex(found);
         ENTITY_VERBOSE_SELF(id << " removed");
         mRefreshRequired = true;
         return removed;
      }

      return 0;
   }

   /// Remove an exact-matching trait from this entity                        
   ///   @param id - type and value to remove                                 
   Count Thing::RemoveTrait(const Trait& prototype) {
      const auto found = mTraits.FindKeyIndex(prototype.GetTrait());
      if (found) {
         const auto removed = mTraits.GetValue(found).RemoveValue(prototype);
         if (removed) {
            ENTITY_VERBOSE_SELF(prototype << " removed");
            mRefreshRequired = true;
            return removed;
         }
      }

      return 0;
   }

   /// A fast check whether traits of the given type are inside this entity   
   ///   @param id - type of trait to check                                   
   ///   @return the number of matching traits                                
   Count Thing::HasTraits(TMeta id) const {
      const auto found = mTraits.FindKeyIndex(id);
      if (found)
         return mTraits.GetValue(found).GetCount();
      return 0;
   }

   /// A fast check whether traits of the given type and value are inside     
   ///   @param id - trait to check                                           
   ///   @return the number of matching traits                                
   Count Thing::HasTraits(const Trait& prototype) const {
      const auto found = mTraits.FindKeyIndex(prototype.GetTrait());
      if (!found)
         return 0;

      Count counter {};
      const auto& list = mTraits.GetValue(found);
      for (auto& trait : list) {
         if (trait == prototype)
            ++counter;
      }
      return counter;
   }

   /// Add/overwrite entity's name trait                                      
   ///   @param name - the name to set                                        
   void Thing::SetName(const Text& name) {
      auto found = GetLocalTrait<Traits::Name>();
      if (!found)
         AddTrait(Traits::Name {name});
      else
         *found = name;
   }

   /// Get entity's name trait, if any                                        
   /// It can reside in one of the units                                      
   ///   @return the name, or empty string if no such trait was found here    
   Text Thing::GetName() const {
      Text name;
      if (!SeekValue<SeekStyle::Here, Traits::Name>(name))
         return {};
      return name;
   }
   
} // namespace Langulus::Entry
