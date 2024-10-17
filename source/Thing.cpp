///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Thing.hpp"
#include "Thing.inl"


namespace Langulus::Entity
{

   /// Default-constructor, always creates a parentless root thing            
   Thing::Thing() : Resolvable {this} {
      ENTITY_VERBOSE_SELF("Created (root, ", GetReferences(), " references)");
   }
   
   /// Descriptor-constructor                                                 
   ///   @param describe - instructions for creating the entity               
   Thing::Thing(Describe&& describe) : Resolvable {this} {
      ENTITY_VERBOSE_SELF_TAB("Created from descriptor: ", *describe);

      if (*describe) {
         Verbs::Create creator {&(*describe)};
         Create(creator);
      }

      if (mOwner) {
         ENTITY_VERBOSE_SELF(
            "Created as child to ", mOwner,
            " (", GetReferences(), " references; parent now has ",
            mOwner->GetReferences(), " references)"
         );
      }
      else {
         ENTITY_VERBOSE_SELF(
            "Created (root, ", GetReferences(), " references)"
         );
      }
   }
   
   /// Construct as a child of another thing                                  
   ///   @param parent - the thing that owns this thing                       
   ///   @param descriptor - instructions for creating the thing              
   Thing::Thing(Thing* parent, const Many& descriptor)
      : Resolvable {this}
      , mOwner     {parent}
   {
      ENTITY_VERBOSE_SELF_TAB("Created manually");

      if (parent) {
         parent->AddChild<false>(this);
         mRuntime = parent->GetRuntime();
         mFlow = parent->GetFlow();
      }

      if (descriptor) {
         // Create any subthings/traits/unit in this thing              
         Verbs::Create creator {descriptor};
         Create(creator);
      }

      if (mOwner) {
         ENTITY_VERBOSE_SELF(
            "Created as child to ", mOwner,
            " (", GetReferences(), " references; parent now has ",
            mOwner->GetReferences(), " references)"
         );
      }
      else {
         ENTITY_VERBOSE_SELF(
            "Created (root, ", GetReferences(), " references)"
         );
      }
   }

   /// Move constructor                                                       
   ///   @attention owner is never moved, you're moving only the hierarchy    
   ///      below the parent, however other's parent is notified of the move, 
   ///      because 'other' is removed from its children                      
   ///   @param other - move that entity                                      
   Thing::Thing(Thing&& other) noexcept
      : Resolvable      {this}
      , mRuntime        {Move(other.mRuntime)}
      , mFlow           {Move(other.mFlow)}
      , mChildren       {Move(other.mChildren)}
      , mUnitsList      {Move(other.mUnitsList)}
      , mUnitsAmbiguous {Move(other.mUnitsAmbiguous)}
      , mTraits         {Move(other.mTraits)}
      , mRefreshRequired{true}
   {
      // Remap children                                                 
      for (auto& child : mChildren)
         child->mOwner = this;

      // Remap units                                                    
      for (auto& unit : mUnitsList)
         unit->ReplaceOwner(&other, this);

      // Make sure the losing parent is notified of the change          
      if (other.mOwner)
         other.mOwner->RemoveChild(&other);

      ENTITY_VERBOSE_SELF("Moved from ", other);
   }
   
   /// Abandon constructor                                                    
   ///   @attention owner is never cloned, you're moving only the hierarchy   
   ///      below the parent, however other's parent is notified of the clone,
   ///      because 'other' is duplicated in its children                     
   ///   @param other - clone that entity                                     
   Thing::Thing(Abandoned<Thing>&& other)
      : Resolvable      {this}
      , mRuntime        {Abandon(other->mRuntime)}
      , mFlow           {Abandon(other->mFlow)}
      , mChildren       {Abandon(other->mChildren)}
      , mUnitsList      {Abandon(other->mUnitsList)}
      , mUnitsAmbiguous {Abandon(other->mUnitsAmbiguous)}
      , mTraits         {Abandon(other->mTraits)}
      , mRefreshRequired{true}
   {
      // Remap children                                                 
      for (auto& child : mChildren)
         child->mOwner = this;

      // Remap units                                                    
      for (auto& unit : mUnitsList)
         unit->ReplaceOwner(&*other, this);

      // Make sure the losing parent is notified of the change          
      if (other->mOwner)
         other->mOwner->RemoveChild(&*other);

      ENTITY_VERBOSE_SELF("Abandoned from ", *other);
   }

   /// Clone constructor                                                      
   ///   @attention owner is never cloned, you're moving only the hierarchy   
   ///      below the parent, however other's parent is notified of the clone,
   ///      because 'other' is duplicated in its children                     
   ///   @param other - clone that entity                                     
   Thing::Thing(Cloned<Thing>&& other)
      : Resolvable      {this}
      , mChildren       {Clone(other->mChildren)}
      , mRefreshRequired{true}
   {
      TODO();
      //TODO clone flow and runtime if pinned, recreate modules if new runtime, 
      // recreate units and traits, then recreate children
      ENTITY_VERBOSE_SELF("Cloned from ", *other);
   }

   /// Destroy the thing                                                      
   Thing::~Thing() {
      // First stage destruction that severs all connections            
      // If Thing was on the stack, this will be the first time it's    
      // called. If it was on the managed heap, it would've been called 
      // from the Reference routine                                     
      Teardown();

      LANGULUS_ASSUME(DevAssumes, GetReferences() <= 1,
         "Can't teardown while this Thing is still used", " in ",
         GetReferences(), " places"
      );

      // Destroy the entire herarchy under this Thing                   
      ENTITY_VERBOSE_SELF_TAB("Destroying...");

      #if ENTITY_VERBOSE_ENABLED()
         for (auto& child : mChildren) {
            if (child->GetReferences() != 1) {
               Logger::Warning(
                  child, " can't be destroyed yet - has ",
                  child->GetReferences(), " uses instead of 1"
               );
            }
         }

         for (auto& unit : mUnitsList) {
            if (unit->GetReferences() > 3) {
               Logger::Warning(
                  unit, " can't be destroyed yet - has ",
                  unit->GetReferences(), " uses instead of 3 (or less)" 
               );
            }
         }
      #endif
   }

   /// This is used for first-stage destruction, severing ties to avoid       
   /// circular dependencies. After this runs, there should be only one       
   /// reference remaining for this Thing                                     
   void Thing::Teardown() {
      ENTITY_VERBOSE_SELF_TAB(
         "Teardown initiated at ", GetReferences(), " uses...");

      // Reset owner, so that only one reference to this Thing remains  
      // in the hierarchy: the owner's mChildren                        
      mOwner.Reset();

      // Traits might be exposing members in units. Make sure we        
      // dereference those first, so that units have as small number of 
      // references as possible                                         
      ENTITY_VERBOSE_SELF("Tearing off traits (name might change)");
      mTraits.Reset();

      // Decouple all units from this owner because units might get     
      // destroyed upon destroying mUnitsList and mUnitsAmbiguous, if   
      // the Units were created on the stack.                           
      // If they still have owners, they will attempt to Decouple in    
      // A::Unit::~Unit from already destroyed mUnitsList/Ambiguous     
      for (auto& unit : mUnitsList) {
         ENTITY_VERBOSE_SELF(
            "Tearing off unit ", unit, " at ", unit->GetReferences(), " uses...");
         unit->mOwners.Remove(this);
      }

      // Propagate Teardown through the hierarchy of Things             
      for (auto& child : mChildren)
         child->Teardown();

      if (not mFlow.IsLocked())
         mFlow.Reset();

      if (not mRuntime.IsLocked())
         mRuntime.Reset();

      ENTITY_VERBOSE_SELF("Teardown complete: ", GetReferences(), " uses remain");
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
      const auto tab = Logger::Verbose(
         Logger::White, Logger::Underline, *this, Logger::Tabs {});

      if (mTraits) {
         const auto tab2 = Logger::Section(Logger::White, Logger::Underline,
            "Traits (", mTraits.GetCount(), "):");
         for (auto traitpair : mTraits) {
            for (auto& trait : traitpair.mValue)
               Logger::Verbose(trait);
         }
      }

      if (mUnitsList) {
         const auto tab2 = Logger::Section(Logger::White, Logger::Underline,
            "Units (", mUnitsList.GetCount(), "):");
         for (auto& unit : mUnitsList)
            Logger::Verbose(*unit);
      }

      if (mChildren) {
         const auto tab2 = Logger::Section(Logger::White, Logger::Underline,
            "Children (", mChildren.GetCount(), "):");
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
         Many unsusedSideeffects;
         if (not mFlow->Update(deltaTime, unsusedSideeffects))
            return false;
      }

      if (mRuntime.IsLocked()) {
         // This thing owns its runtime, so we need to update it here   
         // This is where modules are updated in parallel, physical     
         // simulations happen, images get rendered, etc.               
         if (not mRuntime->Update(deltaTime))
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
   auto Thing::GetUnitMeta(DMeta id, Index index) -> A::Unit* {
      if (id) {
         // Search a typed trait                                        
         const auto found = mUnitsAmbiguous.FindIt(id);
         return found ? found.GetValue()[index] : nullptr;
      }

      // Get unit by index only                                         
      return mUnitsList[index];
   }

   auto Thing::GetUnitMeta(DMeta type, Index offset) const -> const A::Unit* {
      return const_cast<Thing*>(this)->GetUnitMeta(type, offset);
   }
   
   /// Get a unit by type, properties, and offset                             
   /// If construct's type is nullptr searches only by offset and properties  
   /// If construct's type is not nullptr, gets the Nth matching unit, if any 
   ///   @param meta - the type of the unit                                   
   ///   @param what - the desired properties of the unit                     
   ///   @param index - the unit index to seek                                
   ///   @return the unit if found, or nullptr if not                         
   auto Thing::GetUnitExt(DMeta meta, const Many& what, Index index) -> A::Unit* {
      if (meta) {
         // Search a typed unit                                         
         const auto found = mUnitsAmbiguous.FindIt(meta);
         if (found) {
            auto& bucket = found.GetValue();
            if (not what)
               return bucket[index];

            // Check all units in that bucket for required properties   
            for (auto& unit : bucket) {
               if (unit->CompareDescriptor(what)) {
                  // Match found                                        
                  if (index == 0) return unit;
                  else --index;
               }
            }
         }
      }
      else {
         // Search unit by index and properties only                    
         for (auto& unit : mUnitsList) {
            if (unit->CompareDescriptor(what)) {
               // Match found                                           
               if (index == 0) return unit;
               else --index;
            }
         }
      }

      return nullptr;
   }

   auto Thing::GetUnitExt(DMeta meta, const Many& what, Index index) const -> const A::Unit* {
      return const_cast<Thing*>(this)->GetUnitExt(meta, what, index);
   }

#if LANGULUS_FEATURE(MANAGED_REFLECTION)
   /// Get a unit by token and offset                                         
   ///   @param token - the type name of the unit                             
   ///   @param offset - the unit index                                       
   ///   @return the unit if found, or nullptr if not                         
   auto Thing::GetUnitMeta(const Token& token, Index offset) -> A::Unit* {
      return GetUnitMeta(RTTI::DisambiguateMeta(token), offset);
   }
#endif

   /// Get the owner                                                          
   ///   @return the owner                                                    
   auto Thing::GetOwner() const noexcept -> const Ref<Thing>& {
      return mOwner;
   }

   /// Get children hierarchy                                                 
   ///   @return the hierarchy                                                
   auto Thing::GetChildren() const noexcept -> const Hierarchy& {
      return mChildren;
   }

   /// Get a child by index                                                   
   ///   @param id - the index to pick                                        
   ///   @return the child entity, or nullptr of none was found               
   auto Thing::GetChild(Index offset) -> Thing* {
      return mChildren[offset];
   }

   auto Thing::GetChild(Index offset) const -> const Thing* {
      return const_cast<Thing*>(this)->GetChild(offset);
   }

   /// Get child by name and offset (searches for Traits::Name in children)   
   ///   @param name - name to seek                                           
   ///   @param offset - offset to seek                                       
   ///   @return the child entity, or nullptr of none was found               
   auto Thing::GetNamedChild(const Token& name, Index offset) -> Thing* {
      Index matches = 0;
      for (auto& child : mChildren) {
         if (child->GetName() == name) {
            if (matches == offset)
               return child;
            ++matches;
         }
      }

      return nullptr;
   }

   auto Thing::GetNamedChild(const Token& name, Index offset) const -> const Thing* {
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

   /// Count the number of matching units in this entity                      
   ///   @param type - the type of units to search for                        
   ///   @return the number of matching units                                 
   auto Thing::HasUnits(DMeta type) const -> Count {
      const auto found = mUnitsAmbiguous.FindIt(type);
      return found ? found.GetValue().GetCount() : 0;
   }

   /// Check if all units in the hierarchy require a Refresh() call           
   ///   @return true if the thing is dirty                                   
   bool Thing::RequiresRefresh() const noexcept {
      return mRefreshRequired;
   }

   /// Get the current runtime                                                
   ///   @return the pointer to the runtime                                   
   auto Thing::GetRuntime() const noexcept -> const Pin<Ref<Runtime>>& {
      return mRuntime;
   }

   /// Get the current temporal flow                                          
   ///   @return the pointer to the flow                                      
   auto Thing::GetFlow() const noexcept -> const Pin<Ref<Temporal>>& {
      return mFlow;
   }

   /// Create a local runtime for this thing                                  
   ///   @return the new runtime instance, or the old one if already created  
   auto Thing::CreateRuntime() -> Runtime* {
      if (mRuntime.IsLocked())
         return &*mRuntime;

      mRuntime.Get().New(this);
      mRuntime.Lock();

      // Dispatch the change to all children                            
      for (auto& child : mChildren)
         child->ResetRuntime(&*mRuntime);

      ENTITY_VERBOSE_SELF("New runtime: ", &*mRuntime);
      return &*mRuntime;
   }

   /// Create a local flow for this thing                                     
   ///   @return the new flow instance, or the old one, if already created    
   auto Thing::CreateFlow() -> Temporal* {
      if (mFlow.IsLocked())
         return &*mFlow;

      mFlow.Get().New();
      mFlow.Lock();

      // Dispatch the change to all children                            
      for (auto& child : mChildren)
         child->ResetFlow(&*mFlow);

      ENTITY_VERBOSE_SELF("New flow: ", &*mFlow);
      return &*mFlow;
   }

   /// Uses the current runtime to load a shared library module, and          
   /// instantiate it for use, if not yet instantiated                        
   ///   @attention assumes a runtime is available in the hierarchy           
   ///   @param module - name of the module                                   
   ///   @param descriptor - instructions for module setup                    
   ///   @return the instantiated module interface                            
   auto Thing::LoadMod(const Token& module, const Many& descriptor) -> A::Module* {
      const auto runtime = GetRuntime();
      LANGULUS_ASSUME(UserAssumes, runtime,
         "No runtime available for loading a module");
      const auto instance = runtime->InstantiateModule(module, descriptor);
      LANGULUS_ASSERT(instance, Module, "Missing module");
      return instance;
   }

} // namespace Langulus::Entry
