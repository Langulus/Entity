///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Thing.hpp"
#include "Thing.inl"

#if 0
   #define ENTITY_VERBOSE_ENABLED()       1
   #define ENTITY_VERBOSE_SELF(...)       Logger::Verbose(this, ": ", __VA_ARGS__)
   #define ENTITY_VERBOSE_SELF_TAB(...)   const auto scoped = Logger::Verbose(this, ": ", __VA_ARGS__, Logger::Tabs {})
   #define ENTITY_VERBOSE(...)            Logger::Append(__VA_ARGS__)
#else
   #define ENTITY_VERBOSE_ENABLED()       0
   #define ENTITY_VERBOSE_SELF(...)       LANGULUS(NOOP)
   #define ENTITY_VERBOSE_SELF_TAB(...)   LANGULUS(NOOP)
   #define ENTITY_VERBOSE(...)            LANGULUS(NOOP)
#endif


namespace Langulus::Entity
{

   /// Default-constructor, always creates a parentless root thing            
   Thing::Thing() : Resolvable {this} {
      ENTITY_VERBOSE_SELF("Created (root, ", Reference(0), " references)");
   }
   
   /// Descriptor-constructor                                                 
   ///   @param describe - instructions for creating the entity               
   Thing::Thing(Describe&& describe) : Resolvable {this} {
      if (*describe) {
         Verbs::Create creator {&(*describe)};
         Create(creator);
      }

      if (mOwner) {
         ENTITY_VERBOSE_SELF(
            "Created as child to ", mOwner,
            " (", Reference(0), " references; parent now has ",
            mOwner->Reference(0), " references)"
         );
      }
      else {
         ENTITY_VERBOSE_SELF(
            "Created (root, ", Reference(0), " references)"
         );
      }
   }
   
   /// Construct as a child of another thing                                  
   ///   @param parent - the thing that owns this thing                       
   ///   @param descriptor - instructions for creating the thing              
   Thing::Thing(Thing* parent, const Neat& descriptor)
      : Resolvable {this}
      , mOwner {parent} {
      if (parent) {
         parent->AddChild<false>(this);
         mRuntime = parent->GetRuntime();
         mFlow = parent->GetFlow();
      }

      if (descriptor) {
         // Create any subthings/traits/unit in this thing              
         Verbs::Create creator {&descriptor};
         Create(creator);
      }

      if (mOwner) {
         ENTITY_VERBOSE_SELF(
            "Created as child to ", mOwner,
            " (", Reference(0), " references; parent now has ",
            mOwner->Reference(0), " references)"
         );
      }
      else {
         ENTITY_VERBOSE_SELF(
            "Created (root, ", Reference(0), " references)"
         );
      }
   }

   /// Move constructor                                                       
   ///   @attention owner is never moved, you're moving only the hierarchy    
   ///      below the parent, however other's parent is notified of the move, 
   ///      because 'other' is removed from its children                      
   ///   @param other - move that entity                                      
   Thing::Thing(Thing&& other) noexcept
      : Resolvable {this}
      , mChildren {Move(other.mChildren)}
      , mRuntime {Move(other.mRuntime)}
      , mFlow {Move(other.mFlow)}
      , mUnitsAmbiguous {Move(other.mUnitsAmbiguous)}
      , mUnitsList {Move(other.mUnitsList)}
      , mTraits {Move(other.mTraits)}
      , mRefreshRequired {true} {
      // Remap children                                                 
      for (auto& child : mChildren)
         child->mOwner = this;

      // Remap units                                                    
      for (auto& unit : mUnitsList)
         unit->ReplaceOwner(&other, this);

      // Make sure the losing parent is notified of the change          
      if (other.mOwner)
         other.mOwner->RemoveChild(&other);

      ENTITY_VERBOSE_SELF("moved from ", other);
   }
   
   /// Abandon constructor                                                    
   ///   @attention owner is never cloned, you're moving only the hierarchy   
   ///      below the parent, however other's parent is notified of the clone,
   ///      because 'other' is duplicated in its children                     
   ///   @param other - clone that entity                                     
   Thing::Thing(Abandoned<Thing>&& other)
      : Resolvable {this}
      , mChildren {Abandon(other->mChildren)}
      , mRuntime {Abandon(other->mRuntime)}
      , mFlow {Abandon(other->mFlow)}
      , mUnitsAmbiguous {Abandon(other->mUnitsAmbiguous)}
      , mUnitsList {Abandon(other->mUnitsList)}
      , mTraits {Abandon(other->mTraits)}
      , mRefreshRequired {true} {
      // Remap children                                                 
      for (auto& child : mChildren)
         child->mOwner = this;

      // Remap units                                                    
      for (auto& unit : mUnitsList)
         unit->ReplaceOwner(&*other, this);

      // Make sure the losing parent is notified of the change          
      if (other->mOwner)
         other->mOwner->RemoveChild(&*other);

      ENTITY_VERBOSE_SELF("abandoned from ", *other);
   }

   /// Clone constructor                                                      
   ///   @attention owner is never cloned, you're moving only the hierarchy   
   ///      below the parent, however other's parent is notified of the clone,
   ///      because 'other' is duplicated in its children                     
   ///   @param other - clone that entity                                     
   Thing::Thing(Cloned<Thing>&& other)
      : Resolvable {this}
      , mChildren {Clone(other->mChildren)}
      , mRefreshRequired {true} {
      TODO();
      //TODO clone flow and runtime if pinned, recreate modules if new runtime, 
      // recreate units and traits, then recreate children
      ENTITY_VERBOSE_SELF("cloned from ", *other);
   }

   /// Destructor                                                             
   Thing::~Thing() IF_UNSAFE(noexcept) {
      Detach();
   }

   /// A nested call to detach all parents of all children                    
   void Thing::Detach() {
      ENTITY_VERBOSE_SELF_TAB("Destroying (", Reference(0), " uses):");

      if (not mRuntime.IsLocked())
         mRuntime->Reset();
      if (not mFlow.IsLocked())
         mFlow->Reset();

      mTraits.Reset();

      // Decouple all children from this                                
      for (auto& child : mChildren) {
         if (child->mOwner) {
            ENTITY_VERBOSE_SELF("Decoupling child: ", child);
            LANGULUS_ASSUME(DevAssumes,
               child->mOwner == this, "Parent mismatch");
            child->mOwner.Reset();
            child->Detach();
            ENTITY_VERBOSE_SELF("...", Reference(0), " uses remain");
         }
      }

      // Decouple all units from this owner                             
      for (auto& unit : mUnitsList) {
         ENTITY_VERBOSE_SELF("Decoupling unit: ", unit);
         unit->mOwners.Reset();
         ENTITY_VERBOSE_SELF("...", Reference(0), " uses remain");
      }

      mUnitsAmbiguous.Reset();
      mUnitsList.Reset();
   }

   /// Compare two entities                                                   
   ///   @param other - entity to compare with                                
   ///   @return true if entities are functionally same, that is, they have   
   ///           the exact same behavior (actual state is ignored)            
   bool Thing::operator == (const Thing& other) const {
      return mChildren == other.mChildren
         and mUnitsList == other.mUnitsList
         and mTraits == other.mTraits;
   }

   /// Convert to text, by writing a short name or address                    
   Thing::operator Text() const {
      const auto name = GetName();
      if (name)
         return Text {'#', name};
      return Resolvable::operator Text();
   }

   /// Dump the entity's hierarchy in log                                     
   void Thing::DumpHierarchy() const {
      const auto tab = Logger::Verbose("** ", *this, Logger::Tabs {});

      if (mTraits) {
         Logger::Verbose(".. contains ", mTraits.GetCount(), " traits:");
         for (auto traitpair : mTraits) {
            for (auto& trait : traitpair.mValue)
               Logger::Verbose(". ", trait);
         }
      }

      if (mUnitsList) {
         Logger::Verbose("++ contains ", mUnitsList.GetCount(), " units:");
         for (auto& unit : mUnitsList)
            Logger::Verbose("+ ", *unit);
      }

      if (mChildren) {
         Logger::Verbose("** contains ", mChildren.GetCount(), " child entities:");
         for (auto& child : mChildren)
            child->DumpHierarchy();
      }
   }

   /// Simulate the hierarchy of things for a given period of time, by        
   /// updating all runtimes and flows at and under this Thing                
   /// Also synchronizes changes between all units, if mRefreshRequired       
   ///   @param deltaTime - how much time passes for the simulation           
   ///   @return true if no exit was requested by any of the runtimes/flows   
   bool Thing::Update(Time deltaTime) {
      // Refresh the hierarchy on any changes, before updating anything 
      Refresh();

      if (mFlow.IsLocked()) {
         // This thing owns its flow, so we need to update it here      
         // This will execute any temporally based verbs and scripts    
         // Game logic basically happens in this flow                   
         if (not (*mFlow)->Update(deltaTime))
            return false;
      }

      if (mRuntime.IsLocked()) {
         // This thing owns its runtime, so we need to update it here   
         // This is where modules are updated in parallel, physical     
         // simulations happen, images get rendered, etc.               
         if (not (*mRuntime)->Update(deltaTime))
            return false;
      }

      // Cascade the update down the hierarchy                          
      for (auto& child : mChildren) {
         if (not child->Update(deltaTime))
            return false;
      }

      return true;
   }

   /// Refresh all units and children down the hierarchy                      
   ///   @param force - force refresh                                         
   void Thing::Refresh(bool force) {
      if (not force and not mRefreshRequired)
         return;

      mRefreshRequired = false;

      // Refresh all units                                              
      for (auto& unit : mUnitsList)
         unit->Refresh();

      // And cascade down the hierarchy                                 
      for (auto& child : mChildren)
         child->Refresh(true);
   }

   /// Reset the entity, clearing all children, units, traits                 
   void Thing::Reset() {
      // Decouple all children from this parent                         
      for (auto& child : mChildren)
         child->mOwner.Reset();

      // Decouple all units from this owner                             
      for (auto& unit : mUnitsList)
         unit->mOwners.Remove(this);

      mChildren.Reset();
      mUnitsList.Reset();
      mUnitsAmbiguous.Reset();
      mTraits.Reset();
   }

   /// Get a unit by type and offset                                          
   /// If type is nullptr searches only by offset                             
   /// If type is not nullptr, gets the Nth matching unit, if any             
   ///   @param id - the type of the unit                                     
   ///   @param index - the unit index to seek                                
   ///   @return the unit if found, or nullptr if not                         
   A::Unit* Thing::GetUnitMeta(DMeta id, Index index) {
      if (id) {
         // Search a typed trait                                        
         const auto found = mUnitsAmbiguous.FindIt(id);
         return found ? (*found.mValue)[index].Get() : nullptr;
      }

      // Get unit by index only                                         
      return mUnitsList[index].Get();
   }

   const A::Unit* Thing::GetUnitMeta(DMeta type, Index offset) const {
      return const_cast<Thing*>(this)->GetUnitMeta(type, offset);
   }
   
   /// Get a unit by type, properties, and offset                             
   /// If construct's type is nullptr searches only by offset and properties  
   /// If construct's type is not nullptr, gets the Nth matching unit, if any 
   ///   @param meta - the type of the unit                                   
   ///   @param what - the desired properties of the unit                     
   ///   @param index - the unit index to seek                                
   ///   @return the unit if found, or nullptr if not                         
   A::Unit* Thing::GetUnitExt(DMeta meta, const Neat& what, Index index) {
      if (meta) {
         // Search a typed unit                                         
         const auto found = mUnitsAmbiguous.FindIt(meta);
         if (found) {
            auto& bucket = *found.mValue;
            if (not what)
               return bucket[index].Get();

            // Check all units in that bucket for required properties   
            for (auto& unit : bucket) {
               if (unit->CompareDescriptor(what)) {
                  // Match found                                        
                  if (index == 0)
                     return unit.Get();
                  else
                     --index;
               }
            }
         }
      }
      else {
         // Search unit by index and properties only                    
         for (auto& unit : mUnitsList) {
            if (unit->CompareDescriptor(what)) {
               // Match found                                           
               if (index == 0)
                  return unit.Get();
               else
                  --index;
            }
         }
      }

      return nullptr;
   }

   const A::Unit* Thing::GetUnitExt(DMeta meta, const Neat& what, Index index) const {
      return const_cast<Thing*>(this)->GetUnitExt(meta, what, index);
   }

#if LANGULUS_FEATURE(MANAGED_REFLECTION)
   /// Get a unit by token and offset                                         
   ///   @param token - the type name of the unit                             
   ///   @param offset - the unit index                                       
   ///   @return the unit if found, or nullptr if not                         
   A::Unit* Thing::GetUnitMeta(const Token& token, Index offset) {
      return GetUnitMeta(RTTI::DisambiguateMeta(token), offset);
   }
#endif

   /// Get children hierarchy                                                 
   ///   @return the hierarchy                                                
   const Hierarchy& Thing::GetChildren() const noexcept {
      return mChildren;
   }

   /// Get a child by index                                                   
   ///   @param id - the index to pick                                        
   ///   @return the child entity, or nullptr of none was found               
   Thing* Thing::GetChild(Index offset) {
      return mChildren[offset].Get();
   }

   const Thing* Thing::GetChild(Index offset) const {
      return const_cast<Thing*>(this)->GetChild(offset);
   }

   /// Get child by name and offset (searches for Traits::Name in children)   
   ///   @param name - name to seek                                           
   ///   @param offset - offset to seek                                       
   ///   @return the child entity, or nullptr of none was found               
   Thing* Thing::GetNamedChild(const Token& name, Index offset) {
      Index matches = 0;
      for (auto& child : mChildren) {
         if (child->GetName() == name) {
            if (matches == offset)
               return child.Get();
            ++matches;
         }
      }

      return nullptr;
   }

   const Thing* Thing::GetNamedChild(const Token& name, Index offset) const {
      return const_cast<Thing*>(this)->GetNamedChild(name, offset);
   }

   /// Do a cascading runtime reset - all children that do not have own       
   /// runtime, will incorporate the provided one                             
   ///   @param newrt - the new runtime to set                                
   void Thing::ResetRuntime(Runtime* newrt) {
      if (mRuntime.IsLocked())
         return;

      mRuntime = newrt;
      for (auto& child : mChildren)
         child->ResetRuntime(newrt);
   }

   /// Do a cascading temporal flow reset - all children that do not have own 
   /// flow, will incorporate the provided one                                
   ///   @param newrt - the new flow to set                                   
   void Thing::ResetFlow(Temporal* newflow) {
      if (mFlow.IsLocked())
         return;

      mFlow = newflow;
      for (auto& child : mChildren)
         child->ResetFlow(newflow);
   }

   /// Replace one unit instance with another - used when moving units        
   ///   @attention assumes both units are different and not nullptr          
   ///   @param replaceThis - the unit to replace                             
   ///   @param withThis - the unit to replace with                           
   /*Count Thing::ReplaceUnit(Unit* replaceThis, Unit* withThis) {
      RemoveUnit(replaceThis);
      return AddUnit(withThis);
   }*/

   /// Count the number of matching units in this entity                      
   ///   @param type - the type of units to search for                        
   ///   @return the number of matching units                                 
   Count Thing::HasUnits(DMeta type) const {
      const auto found = mUnitsAmbiguous.FindIt(type);
      return found ? found.mValue->GetCount() : 0;
   }

   /// Get the current runtime                                                
   ///   @return the pointer to the runtime                                   
   Runtime* Thing::GetRuntime() const noexcept {
      return const_cast<Runtime*>(mRuntime->Get());
   }

   /// Get the current temporal flow                                          
   ///   @return the pointer to the flow                                      
   Temporal* Thing::GetFlow() const noexcept {
      return const_cast<Temporal*>(mFlow->Get());
   }

   /// Create a local runtime for this thing                                  
   ///   @return the new runtime instance, or the old one if already created  
   Runtime* Thing::CreateRuntime() {
      if (mRuntime.IsLocked())
         return mRuntime->Get();

      mRuntime->New(this);
      mRuntime.Lock();

      // Dispatch the change to all children                            
      for (auto& child : mChildren)
         child->ResetRuntime(mRuntime->Get());

      ENTITY_VERBOSE_SELF("New runtime: ", *mRuntime);
      return mRuntime->Get();
   }

   /// Create a local flow for this thing                                     
   ///   @return the new flow instance, or the old one, if already created    
   Temporal* Thing::CreateFlow() {
      if (mFlow.IsLocked())
         return mFlow->Get();

      mFlow->New();//this
      mFlow.Lock();

      // Dispatch the change to all children                            
      for (auto& child : mChildren)
         child->ResetFlow(mFlow->Get());

      ENTITY_VERBOSE_SELF("New flow: ", *mFlow);
      return mFlow->Get();
   }

   /// Create a child thing                                                   
   ///   @param descriptor - instructions for the entity's creation           
   ///   @return the new child instance                                       
   Ref<Thing> Thing::CreateChild(const Neat& descriptor) {
      ENTITY_VERBOSE_SELF_TAB(
         "Producing child (at ", Reference(0), " references): ");
      Ref<Thing> newThing;
      newThing.New(this, descriptor);
      return Abandon(newThing);
   }

   /// Uses the current runtime to load a shared library module, and          
   /// instantiate it for use, if not yet instantiated                        
   ///   @attention assumes a runtime is available in the hierarchy           
   ///   @param module - name of the module                                   
   ///   @param descriptor - instructions for module setup                    
   ///   @return the instantiated module interface                            
   A::Module* Thing::LoadMod(const Token& module, const Neat& descriptor) {
      const auto runtime = GetRuntime();
      LANGULUS_ASSUME(UserAssumes, runtime,
         "No runtime available for loading a module");
      const auto instance = runtime->InstantiateModule(module, descriptor);
      LANGULUS_ASSERT(instance, Module, "Missing module");
      return instance;
   }

} // namespace Langulus::Entry
