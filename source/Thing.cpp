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

   /// Construct as a root                                                    
   ///   @param descriptor - instructions for creating the entity             
   Thing::Thing(const Any& descriptor)
      : Resolvable {MetaOf<Thing>()} {
      if (!descriptor.IsEmpty()) {
         Verbs::Create creator {descriptor};
         Create(creator);
      }
   }
   
   /// Construct as a child                                                   
   ///   @param parent - the owner of the entity                              
   ///   @param descriptor - instructions for creating the entity             
   Thing::Thing(Thing* parent, const Any& descriptor)
      : Resolvable {MetaOf<Thing>()}
      , mOwner {parent}
      , mRuntime {parent ? parent->GetRuntime() : nullptr} {
      if (!descriptor.IsEmpty()) {
         Verbs::Create creator {descriptor};
         Create(creator);
      }
   }

   /// Move constructor                                                       
   ///   @param other - move that entity                                      
   Thing::Thing(Thing&& other) noexcept
      : Resolvable {Forward<Resolvable>(other)}
      , mOwner {Move(other.mOwner)}
      , mChildren {Move(other.mChildren)}
      , mUnits {Move(other.mUnits)}
      , mTraits {Move(other.mTraits)}
      , mRuntime {Move(other.mRuntime)} {
      // Remap children                                                 
      for (auto child : mChildren)
         child->mOwner = this;

      // Remap components                                               
      for (auto unitpair : mUnits) {
         for (auto unit : unitpair.mValue)
            unit->ReplaceOwner(&other, this);
      }
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
         result += "#";
         result += name;
         return result;
      }
      
      return Resolvable::operator Debug();
   }

   /// Dump the entity's hierarchy in log                                     
   void Thing::DumpHierarchy() const {
      const auto tab = Logger::Verbose() << "** " << *this << Logger::Tabs {};

      if (!mTraits.IsEmpty()) {
         Logger::Verbose() << ".. contains " << mTraits.GetCount() << " traits";
         for (auto traitpair : mTraits) {
            for (auto trait : traitpair.mValue)
               Logger::Verbose() << ". " << trait;
         }
      }

      if (!mUnits.IsEmpty()) {
         Logger::Verbose() << "++ contains " << mUnits.GetCount() << " units";
         for (auto unitpair : mUnits) {
            for (auto unit : unitpair.mValue)
               Logger::Verbose() << "+ " << *unit;
         }
      }

      if (!mChildren.IsEmpty()) {
         Logger::Verbose() << "** contains " << mChildren.GetCount() << " child entities";
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

   /// Check if this entity is other, or some kind of parent of it            
   ///   @param other - the entity instance to test                           
   ///   @return true if this entity is either the other, or related upwards  
   bool Thing::IsFamilyOf(const Thing& other) const {
      if (this == &other)
         return true;

      auto parent = other.mOwner.Get();
      while (parent) {
         if (parent == this)
            return true;

         parent = parent->mOwner.Get();
      }

      return false;
   }

   /// Get a unit by type and offset                                          
   /// If type is nullptr searches only by offset                             
   /// If type is not nullptr, gets the Nth matching unit, if any             
   ///   @param type - the type of the unit                                   
   ///   @param offset - the unit index to seek                               
   ///   @return the unit if found, or nullptr if not                         
   Unit* Thing::GetUnit(DMeta id, const Index& index) {
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

#if LANGULUS_FEATURE(MANAGED_REFLECTION)
   /// Get a unit by token and offset                                         
   ///   @param token - the type name of the unit                             
   ///   @param offset - the unit index                                       
   ///   @return the unit if found, or nullptr if not                         
   Unit* Thing::GetUnit(const Token& token, const Index& offset) {
      return GetUnit(RTTI::Database.GetMetaData(token), offset);
   }
#endif

   /// Get a child by index                                                   
   ///   @param id - the index to pick                                        
   ///   @return the child entity, or nullptr of none was found               
   Thing* Thing::GetChild(const Index& id) {
      return mChildren[id];
   }

   /// Get child by name and offset (searches for Traits::Name in children)   
   ///   @param name - name to seek                                           
   ///   @param offset - offset to seek                                       
   ///   @return the child entity, or nullptr of none was found               
   Thing* Thing::GetChild(const Token& name, const Index& offset) {
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
      if (mOwnRuntime)
         return;

      mRuntime = newrt;
      for (auto child : mChildren)
         child->ResetRuntime(newrt);
   }

   /// Do a cascading temporal flow reset - all children that do not have own 
   /// flow, will incorporate the provided one                                
   ///   @param newrt - the new flow to set                                   
   void Thing::ResetFlow(Temporal* newflow) {
      if (mOwnFlow)
         return;

      mFlow = newflow;
      for (auto child : mChildren)
         child->ResetFlow(newflow);
   }

   /// Add a new unit to the entity                                           
   /// Adding units, coupled with different runtimes is not allowed           
   /// You can not duplicate unit pointers                                    
   ///   @param unit - the unit to add                                        
   ///   @return 1 if unit has been added                                     
   Count Thing::AddUnit(Unit* unit) {
      // Check if pointer is already registered                         
      const auto foundType = mUnits.FindKeyIndex(unit->GetType());
      if (foundType && mUnits.GetValue(foundType).Find(unit))
         return 0;

      // We must guarantee, that no unit is coupled to entities with    
      // different runtimes!                                            
      for (auto owners : unit->mOwners) {
         LANGULUS_ASSERT(owners->GetRuntime() == GetRuntime(), Except::Access,
            "Coupling a unit to multiple runtimes is not allowed");
      }

      // Log self before unit being added, it might change name         
      ENTITY_VERBOSE_SELF("");
      unit->mOwners << this;
      mUnits[unit->GetType()] << unit;
      mRefreshRequired = true;
      ENTITY_VERBOSE(unit << " added");
      return 1;
   }

   /// Remove an exact unit from the entity                                   
   ///   @param unit - unit to remove from the entity                         
   ///   @return 1 if unit has been removed                                   
   Count Thing::RemoveUnit(Unit* unit) {
      const auto foundType = mUnits.FindKeyIndex(unit->GetType());
      if (foundType) {
         const auto removed = mUnits.GetValue(foundType).RemoveValue(unit);
         if (removed) {
            // Decouple before unit is destroyed                        
            unit->mOwners.RemoveValue<false, true>(this);

            // Notify all other units about the environment change      
            mRefreshRequired = true;
            ENTITY_VERBOSE_SELF(unit << " removed");
         }
      }

      return 0;
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
      replaceThis->mOwners.RemoveValue(this);
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
      return mRuntime.Get();
   }

   /// Get the current temporal flow                                          
   ///   @return the pointer to the flow                                      
   Temporal* Thing::GetFlow() const noexcept {
      return mFlow.Get();
   }

   /// Create a local runtime for this thing                                  
   ///   @return the new runtime instance, or the old one if already created  
   Runtime* Thing::CreateRuntime() {
      if (mOwnRuntime)
         return mRuntime.Get();
      mRuntime.New(this);
      return mRuntime.Get();
   }

   /// Create a local flow for this thing                                     
   ///   @return the new flow instance, or the old one, if already created    
   Temporal* Thing::CreateFlow() {
      if (mOwnFlow)
         return mFlow.Get();
      mFlow.New(this);
      return mFlow.Get();
   }

   /// Create a child thing                                                   
   ///   @param descriptor - instructions for the entity's creation           
   ///   @return the new child instance                                       
   Thing* Thing::CreateChild(const Any& descriptor) {
      mChildren.Emplace(this, descriptor);
      ENTITY_VERBOSE_SELF(mChildren.Last() << " added");
      return mChildren.Last();
   }

   /// Destroy a child that matched pointer                                   
   ///   @attention provided pointer is considered invalid after this call    
   ///   @param entity - entity instance to destroy                           
   ///   @return the number of destroyed children                             
   Count Thing::DestroyChild(Thing* entity) {
      const auto removed = mChildren.RemoveValue(entity);
      mRefreshRequired = removed > 0;
      return removed;
   }

   /// Uses the current runtime to load a shared library module, and          
   /// instantiate it for use                                                 
   ///   @attention assumes a runtime is available in the hierarchy           
   ///   @param module - name of the module                                   
   ///   @param descriptor - instructions for module setup                    
   ///   @return the instantiated module interface                            
   Module* Thing::LoadMod(const Token& module, const Any& descriptor) {
      auto runtime = GetRuntime();
      LANGULUS_ASSUME(UserAssumes, runtime != nullptr,
         "No runtime available for loading a module");
      return runtime->InstantiateModule(module, descriptor);
   }

   /// Create all dependencies required for the production of 'type', if such 
   /// doesn't yet exist the this entity's upward hierarchy                   
   /// They will be added either immediately below to their own producers, or 
   /// by the Runtime as a fallback                                           
   ///   @param type - the type to build dependencies for                     
   ///   @return the resulting dependencies (top level)                       
   Any Thing::CreateDependencies(DMeta type) {
      auto meta = type->mProducer;
      LANGULUS_ASSERT(meta, Except::Construct, "Missing producer");

      if (meta->HasBase<Unit>()) {
         // Unit dependencies, search for them in the hierarchy,        
         // they might already exist                                    
         auto producers = GatherUnits<SeekStyle::UpToHere>(meta);
         if (!producers.IsEmpty())
            return producers;

         // No producer available in the hierarchy, so nest-create      
         // producer's producers                                        
         ENTITY_VERBOSE_SELF(Logger::DarkYellow
            << "Required producer " << meta << " for creating " << type
            << " not found in hierarchy - attempting to create it...");

         auto nestedProducers = CreateDependencies(meta);
         nestedProducers.ForEach(
            [&](Unit* unit) {
               for (auto owner : unit->GetOwners())
                  producers += owner->CreateDataInner(meta);
            },
            [&](Module* module) {
               const auto owner = module->GetRuntime()->GetOwner();
               producers += owner->CreateDataInner(meta);
            }
         );

         return producers;
      }
      else if (meta->HasBase<Module>()) {
         // Producer is a module. Let's seek the runtime component,     
         // since it contains the instantiated modules                  
         auto runtime = GetRuntime();
         LANGULUS_ASSERT(runtime, Except::Construct, "Missing runtime");

         auto dependency = runtime->GetDependency(meta);
         return runtime->InstantiateModule(dependency);
      }

      Logger::Error()
         << "Couldn't produce " << type << " from " << meta
         << " - the producer wasn't found, and couldn't be produced";
      LANGULUS_THROW(Construct, "Couldn't create dependencies");
   }

   /// Produce unit(s) from the entity's hierarchy                            
   ///   @param construct - instructions for the creation of the unit(s)      
   ///   @return all created units                                            
   Any Thing::CreateDataInner(const Construct& construct) {
      Verbs::Create creator {&construct};
      auto producers = CreateDependencies(construct.GetType());
      if (Scope::ExecuteVerb(producers, creator))
         return Abandon(creator.GetOutput());
      return {};
   }
   
} // namespace Langulus::Entry
