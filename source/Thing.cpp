///                                                                           
/// Langulus::Entity                                                          
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#include "Thing.inl"
#include "Hierarchy.inl"
#include "Runtime.hpp"
#include "Unit.inl"

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

   /// Construct as a root                                                    
   ///   @param descriptor - instructions for creating the entity             
   Thing::Thing(const Any& descriptor)
      : Resolvable {nullptr /*MetaData::Of<::Langulus::Entity::Thing>()*/} {
      if (!descriptor.IsEmpty()) {
         Verbs::Create creator {descriptor};
         Create(creator);
      }
   }
   
   /// Construct as a child of another thing                                  
   ///   @param parent - the owner of the thing                               
   ///   @param descriptor - instructions for creating the thing              
   Thing::Thing(Thing* parent, const Any& descriptor)
      : Resolvable {nullptr /*MetaData::Of<::Langulus::Entity::Thing>()*/}
      , mOwner {parent} {
      if (parent) {
         parent->AddChild<false>(this);
         mRuntime = parent->GetRuntime();
         mFlow = parent->GetFlow();
      }

      if (!descriptor.IsEmpty()) {
         // Create any subthings/traits/unit in this thing              
         Verbs::Create creator {descriptor};
         Create(creator);
      }
   }

   /// Move constructor                                                       
   ///   @attention owner is never moved, you're moving only the hierarchy    
   ///              below the parent, however other's parent is notified of   
   ///              the move, 'other' is removed from its children            
   ///   @param other - move that entity                                      
   Thing::Thing(Thing&& other) noexcept
      : Resolvable {Forward<Resolvable>(other)}
      , mChildren {::std::move(other.mChildren)}
      , mUnits {::std::move(other.mUnits)}
      , mTraits {::std::move(other.mTraits)}
      , mRuntime {::std::move(other.mRuntime)}
      , mRefreshRequired {true} {
      // Remap children                                                 
      for (auto child : mChildren)
         child->mOwner = this;

      // Remap components                                               
      for (auto unitpair : mUnits) {
         for (auto unit : unitpair.mValue)
            unit->ReplaceOwner(&other, this);
      }

      // Make sure the losing parent is notified of the change          
      if (other.mOwner)
         other.mOwner->RemoveChild(&other);
   }

   /// Compare two entities                                                   
   ///   @param other - entity to compare with                                
   ///   @return true if entities are functionally same, that is, they have   
   ///           the exact same behavior (actual state is ignored)            
   bool Thing::operator == (const Thing& other) const {
      return mChildren == other.mChildren
         && mUnits == other.mUnits
         && mTraits == other.mTraits;
   }

   /// Convert to text, by writing a short name or address                    
   Thing::operator Debug() const {
      const auto name = GetName();
      if (!name.IsEmpty()) {
         Debug result;
         result += '#';
         result += name;
         return result;
      }
      
      return Resolvable::operator Debug();
   }

   /// Dump the entity's hierarchy in log                                     
   void Thing::DumpHierarchy() const {
      const auto tab = Logger::Verbose("** ", *this, Logger::Tabs {});

      if (!mTraits.IsEmpty()) {
         Logger::Verbose(".. contains ", mTraits.GetCount(), " traits");
         for (auto traitpair : mTraits) {
            for (auto trait : traitpair.mValue)
               Logger::Verbose(". ", trait);
         }
      }

      if (!mUnits.IsEmpty()) {
         Logger::Verbose("++ contains ", mUnits.GetCount(), " units");
         for (auto unitpair : mUnits) {
            for (auto unit : unitpair.mValue)
               Logger::Verbose("+ ", *unit);
         }
      }

      if (!mChildren.IsEmpty()) {
         Logger::Verbose("** contains ", mChildren.GetCount(), " child entities");
         for (auto child : mChildren)
            child->DumpHierarchy();
      }
   }

   /// Update all children's Runtime(s) and Temporal(s)                       
   /// This is used for the purpose of a main loop, but also supports sub     
   /// systems and multiple runtimes with different environments              
   ///   @param dt - delta time that has passed between updates               
   void Thing::Update(Time) {
      // Keep a flow inside the runtime?
      /*
      auto thisBlock = GetBlock();
      for (auto unit : mUnits) {
         if (unit->IsClassIrrelevant())
            continue;

         if (unit->ClassIs<CFlow>()) {
            // Update every flow                                          
            // This will execute all verbs up to current   time, as well   
            // as verbs with specific time and frequency                  
            static_cast<CFlow*>(unit)->Update(thisBlock, dt);
         }
         else if (unit->ClassIs<CRuntime>()) {
            // Update every system                                       
            // This will also update all modules that   are instantiated   
            // by the system component                                    
            static_cast<CRuntime*>(unit)->Update(dt);
         }
      }

      // Refresh all units if requested                                 
      if (mRefreshRequired) {
         mRefreshRequired = false;
         for (auto unit : mUnits)
            unit->Refresh();
      }

      // Nest update inside all children                                 
      for (auto child : mChildren) {
         if (child->IsClassIrrelevant())
            continue;
         child->Update(dt);
      }*/
   }

   /// Reset the entity, clearing all children, units, traits                 
   void Thing::Reset() {
      mChildren.Clear();
      mUnits.Clear();
      mTraits.Clear();
   }

   /// Get a unit by type and offset                                          
   /// If type is nullptr searches only by offset                             
   /// If type is not nullptr, gets the Nth matching unit, if any             
   ///   @param type - the type of the unit                                   
   ///   @param index - the unit index to seek                                
   ///   @return the unit if found, or nullptr if not                         
   Unit* Thing::GetUnit(DMeta id, Index index) {
      if (id) {
         // Search a typed trait                                        
         const auto found = mUnits.FindKeyIndex(id);
         if (found)
            return mUnits.GetValue(found)[index];
         return nullptr;
      }

      // Search unit by index                                           
      Unit* found {};
      if (index.IsArithmetic()) {
         mUnits.ForEachValue([&](TAny<Unit*>& list) noexcept {
            if (index < list.GetCount()) {
               found = list[index];
               return false;
            }

            index -= list.GetCount();
            return true;
         });
      }

      return found;
   }

   /// Get a unit by type and offset (const)                                  
   /// If type is nullptr searches only by offset                             
   /// If type is not nullptr, gets the Nth matching unit, if any             
   ///   @param type - the type of the unit                                   
   ///   @param offset - the unit index to seek                               
   ///   @return the unit if found, or nullptr if not                         
   const Unit* Thing::GetUnit(DMeta type, Index offset) const {
      return const_cast<Thing*>(this)->GetUnit(type, offset);
   }
   
   /// Get a unit by type, properties, and offset                             
   /// If type is nullptr searches only by offset and properties              
   /// If type is not nullptr, gets the Nth matching unit, if any             
   ///   @param what - the type and properties of the unit                    
   ///   @param index - the unit index to seek                                
   ///   @return the unit if found, or nullptr if not                         
   Unit* Thing::GetUnitExt(const Construct& what, Index index) {
      if (what.GetType()) {
         // Search a typed unit                                         
         const auto found = mUnits.FindKeyIndex(what.GetType());
         if (found) {
            const auto& bucket = mUnits.GetValue(found);
            if (what.GetArgument().IsEmpty())
               return bucket[index];

            // Check all units in that bucket for required properties   
            for (auto unit : bucket) {
               bool mismatch {};

               Offset memberOffset {};
               what.ForEachDeep(
                  [&](const Trait& trait) {
                     if (!unit->GetMember(trait.GetTrait(), memberOffset).Compare(trait)) {
                        mismatch = true;
                        return false;
                     }

                     ++memberOffset;
                     return true;
                  }
               );

               memberOffset = {};
               what.ForEachDeep(
                  [&](const Block& anythingElse) {
                     if (!unit->GetMember(nullptr, memberOffset).Compare(anythingElse)) {
                        mismatch = true;
                        return false;
                     }

                     ++memberOffset;
                     return true;
                  }
               );

               if (!mismatch) {
                  // Match found                                        
                  if (index == 0)
                     return unit;
                  else
                     --index;
               }
            }
         }

         return nullptr;
      }

      // Search unit by index and properties only                       
      Unit* found {};
      if (index.IsArithmetic()) {
         auto offset = index.GetOffset();
         mUnits.ForEachValue([&](TAny<Unit*>& list) noexcept {
            if (offset < list.GetCount()) {
               found = list[offset];
               return false;
            }

            offset -= list.GetCount();
            return true;
         });
      }

      return found;
   }

   /// Get a unit by type, properties, and offset (const)                     
   ///   @param what - the type and properties of the unit                    
   ///   @param index - the unit index to seek                                
   ///   @return the unit if found, or nullptr if not                         
   const Unit* Thing::GetUnitExt(const Construct& what, Index index) const {
      return const_cast<Thing*>(this)->GetUnitExt(what, index);
   }

#if LANGULUS_FEATURE(MANAGED_REFLECTION)
   /// Get a unit by token and offset                                         
   ///   @param token - the type name of the unit                             
   ///   @param offset - the unit index                                       
   ///   @return the unit if found, or nullptr if not                         
   Unit* Thing::GetUnit(const Token& token, Index offset) {
      return GetUnit(RTTI::Database.GetMetaData(token), offset);
   }
#endif

   /// Get a child by index                                                   
   ///   @param id - the index to pick                                        
   ///   @return the child entity, or nullptr of none was found               
   Thing* Thing::GetChild(const Index& id) {
      return mChildren[id];
   }

   /// Get child entity by offset                                             
   ///   @param offset - the offset of the entity to return                   
   ///   @return the child or nullptr if none found                           
   const Thing* Thing::GetChild(const Index& offset) const {
      return const_cast<Thing*>(this)->GetChild(offset);
   }

   /// Get child entity by name and offset                                    
   ///   @param name - the name trait to search for                           
   ///   @param offset - the offset of the matching entity to return          
   ///   @return the child or nullptr if none found                           
   const Thing* Thing::GetNamedChild(const Token& name, const Index& offset) const {
      return const_cast<Thing*>(this)->GetNamedChild(name, offset);
   }

   /// Get child by name and offset (searches for Traits::Name in children)   
   ///   @param name - name to seek                                           
   ///   @param offset - offset to seek                                       
   ///   @return the child entity, or nullptr of none was found               
   Thing* Thing::GetNamedChild(const Token& name, const Index& offset) {
      Index matches = 0;
      for (auto child : mChildren) {
         if (child->GetName() == name) {
            if (matches == offset)
               return child;
            ++matches;
         }
      }

      return nullptr;
   }

   /// Do a cascading runtime reset - all children that do not have own       
   /// runtime, will incorporate the provided one                             
   ///   @param newrt - the new runtime to set                                
   void Thing::ResetRuntime(Runtime* newrt) {
      if (mRuntime.IsPinned())
         return;

      mRuntime = newrt;
      for (auto child : mChildren)
         child->ResetRuntime(newrt);
   }

   /// Do a cascading temporal flow reset - all children that do not have own 
   /// flow, will incorporate the provided one                                
   ///   @param newrt - the new flow to set                                   
   void Thing::ResetFlow(Temporal* newflow) {
      if (mFlow.IsPinned())
         return;

      mFlow = newflow;
      for (auto child : mChildren)
         child->ResetFlow(newflow);
   }

   /// Replace one unit instance with another - used when moving units        
   ///   @attention assumes both units are different and not nullptr          
   ///   @param replaceThis - the unit to replace                             
   ///   @param withThis - the unit to replace with                           
   void Thing::ReplaceUnit(Unit* replaceThis, Unit* withThis) {
      const auto foundType = mUnits.FindKeyIndex(replaceThis->GetType());
      if (!foundType)
         return;

      auto& list = mUnits.GetValue(foundType);
      const auto foundUnit = list.Find(replaceThis);
      if (!foundUnit)
         return;

      list[foundUnit] = withThis;
      replaceThis->mOwners.Remove(this);
      withThis->mOwners << this;
      mRefreshRequired = true;
      ENTITY_VERBOSE_SELF(replaceThis << " replaced with " << withThis);
   }

   /// Count the number of matching units in this entity                      
   ///   @param type - the type of units to seach for                         
   ///   @return the number of matching units                                 
   Count Thing::HasUnits(DMeta type) const {
      const auto found = mUnits.FindKeyIndex(type);
      return found ? mUnits.GetValue(found).GetCount() : 0;
   }

   /// Get the current runtime                                                
   ///   @return the pointer to the runtime                                   
   Runtime* Thing::GetRuntime() const noexcept {
      return mRuntime;
   }

   /// Get the current temporal flow                                          
   ///   @return the pointer to the flow                                      
   Temporal* Thing::GetFlow() const noexcept {
      return mFlow;
   }

   /// Create a local runtime for this thing                                  
   ///   @return the new runtime instance, or the old one if already created  
   Runtime* Thing::CreateRuntime() {
      if (mRuntime.IsPinned())
         return mRuntime;
      mRuntime.New(this);
      mRuntime.Pin();
      return mRuntime;
   }

   /// Create a local flow for this thing                                     
   ///   @return the new flow instance, or the old one, if already created    
   Temporal* Thing::CreateFlow() {
      if (mFlow.IsPinned())
         return mFlow;
      mFlow.New(this);
      mFlow.Pin();
      return mFlow;
   }

   /// Create a child thing                                                   
   ///   @param descriptor - instructions for the entity's creation           
   ///   @return the new child instance                                       
   Thing* Thing::CreateChild(const Any& descriptor) {
      Ptr<Thing> newThing;
      newThing.New(this, descriptor);
      return newThing;
   }

   /// Uses the current runtime to load a shared library module, and          
   /// instantiate it for use                                                 
   ///   @attention assumes a runtime is available in the hierarchy           
   ///   @param module - name of the module                                   
   ///   @param descriptor - instructions for module setup                    
   ///   @return the instantiated module interface                            
   Module* Thing::LoadMod(const Token& module, const Any& descriptor) {
      const auto runtime = GetRuntime();
      LANGULUS_ASSUME(UserAssumes, runtime != nullptr,
         "No runtime available for loading a module");
      const auto instance = runtime->InstantiateModule(module, descriptor);
      LANGULUS_ASSERT(instance, Module, "Missing module");
      return instance;
   }

} // namespace Langulus::Entry
