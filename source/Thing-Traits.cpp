///                                                                           
/// Langulus::Entity                                                          
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#include "Thing.inl"
#include "Thing-Seek.inl"

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
         const auto found = mTraits.Find(id);
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
            return Traits::Runtime {mRuntime.Get()};
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
      Trait output;
      for (auto& unit : mUnitsList) {
         auto t = unit->GetMember(id.GetTrait(), index);
         if (!t.IsEmpty()) {
            output = Trait::From(id.GetTrait(), t);
            break;
         }
      }

      if (!output.IsEmpty())
         return Abandon(output);

      // Finally, check this entity's static traits                     
      auto t = GetMember(id.GetTrait(), index);
      if (!t.IsEmpty())
         return Trait::From(id.GetTrait(), t);

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
   ///   @return the number of removed traits                                 
   Count Thing::RemoveTrait(TMeta id) {
      const auto found = mTraits.Find(id);
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
   ///   @return the number of removed traits                                 
   Count Thing::RemoveTrait(const Trait& prototype) {
      const auto found = mTraits.Find(prototype.GetTrait());
      if (found) {
         const auto removed = mTraits.GetValue(found).Remove(prototype);
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
      const auto found = mTraits.Find(id);
      if (found)
         return mTraits.GetValue(found).GetCount();
      return 0;
   }

   /// A fast check whether traits of the given type and value are inside     
   ///   @param prototype - trait to search for                               
   ///   @return the number of matching traits                                
   Count Thing::HasTraits(const Trait& prototype) const {
      const auto found = mTraits.Find(prototype.GetTrait());
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
      if (!SeekTrait<Traits::Name, Seek::Here>(name))
         return {};
      return name;
   }
   
} // namespace Langulus::Entry
