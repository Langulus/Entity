///                                                                           
/// Langulus::Entity                                                          
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#include "Entity.hpp"
#include "Runtime.hpp"

#define ENTITY_VERBOSE_SELF(a)            //Logger::Verbose() << this << ": "<< a
#define ENTITY_VERBOSE(a)                 //Logger::Append() << a
#define ENTITY_CREATION_VERBOSE_SELF(a)   //Logger::Verbose() << this << ": " <<a
#define ENTITY_SELECTION_VERBOSE_SELF(a)  //Logger::Verbose() << this << ": " <<a


namespace Langulus::Entity
{

   /// Construct as a child                                                   
   ///   @param parent - the owner of the entity                              
   Entity::Entity(Entity* parent) noexcept
      : Resolvable {MetaData::Of<Entity>()}
      , mOwner {parent}
      , mRuntime {parent ? parent->GetRuntime() : nullptr} {}

   /// Move constructor                                                       
   ///   @param other - move that entity                                      
   Entity::Entity(Entity&& other) noexcept
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
   bool Entity::operator == (const Entity& other) const {
      return mChildren == other.mChildren
         && mUnits == other.mUnits
         && mTraits == other.mTraits;
   }

   /// Convert to text, by writing a short name or address                    
   Entity::operator Debug() const {
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
   void Entity::DumpHierarchy() const {
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

   /// Execute a piece of langulus Code in the closest possible temporal flow 
   ///   @param code - the code to execute                                    
   ///   @return the results of the code                                      
   Any Entity::Run(const Code& code) {
      if (code.IsEmpty())
         return {};

      // Execute code                                                   
      auto parsed = code.Parse();
      Any context {GetBlock()};
      Any output;
      if (!parsed.Execute(context, output)) {
         Logger::Error() << "RunCode failed to execute: [" << parsed << ']';
         return {};
      }

      return output;
   }

   /// Find an AIAD component that can process speech and interpret using it  
   /// This function completely relies on external modules                    
   ///   @param text - text to execute                                        
   ///   @return true if the text was successfully executed                   
   Any Entity::Run(const Lingua& text) {
      if (text.IsEmpty())
         return {};

      if (!GetFlow()) {
         Logger::Error()
            << "Message not interpreted due to lack of temporal flow: ["
            << text << ']';
         return {};
      }

      // Create message objects from the hierarchy, and try             
      // interpreting them to executable scopes                         
      auto messages = CreateData<Lingua>();
      Verbs::InterpretTo<Flow::Scope> interpreter;
      if (!Flow::DispatchFlat(messages, interpreter)) {
         Logger::Error()
            << "Messages failed to interpret: " << messages;
         return {};
      }

      // Execute the flow                                               
      return Run(interpreter.GetOutput());
   }

   /// Update all children's Runtime(s) and Temporal(s)                       
   /// This is used for the purpose of a main loop, but also supports sub     
   /// systems and multiple runtimes with different environments              
   ///   @param dt - delta time that has passed between updates               
   void Entity::Update(Time) {
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
   void Entity::Reset() {
      mChildren.Clear();
      mUnits.Clear();
      mTraits.Clear();
   }

   /// Check if this entity is other, or some kind of parent of it            
   ///   @param other - the entity instance to test                           
   ///   @return true if this entity is either the other, or related upwards  
   bool Entity::IsFamilyOf(const Entity& other) const {
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

   /// Get a trait by type                                                    
   ///   @param id - trait id                                                 
   ///   @param output - [out] result of the search                           
   ///   @param offset - offset of result to use                              
   ///   @return true if anything was found                                   
   bool Entity::GetTrait(TMeta id, Trait& output, const Index& offset) {
      return GetTrait(Trait {id}, output, offset);
   }

   /// Get a trait from this entity's trait list                              
   ///   @param id - the trait to search for                                  
   ///   @param index - the index of the trait we seek                        
   ///   @return a pointer to the trait, or nullptr if not found              
   Trait* Entity::GetLocalTrait(TMeta id, const Index& index) {
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
   const Trait* Entity::GetLocalTrait(TMeta id, const Index& offset) const {
      return const_cast<Entity&>(*this).GetLocalTrait(id, offset);
   }

   /// Get a trait by type and contents                                       
   ///   @param id - trait to match                                           
   ///   @param output - [out] result of the search                           
   ///   @param offset - offset of result to use                              
   ///   @return true if anything was found                                   
   bool Entity::GetTrait(const Trait& id, Trait& output, const Index& index) {
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

   /// Get a unit by type and offset                                          
   /// If type is nullptr searches only by offset                             
   /// If type is not nullptr, gets the Nth matching unit, if any             
   ///   @param type - the type of the unit                                   
   ///   @param offset - the unit index to seek                               
   ///   @return the unit if found, or nullptr if not                         
   Unit* Entity::GetUnit(DMeta id, const Index& index) {
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
   Unit* Entity::GetUnit(const Token& token, const Index& offset) {
      return GetUnit(RTTI::Database.GetMetaData(token), offset);
   }
#endif

   /// Get a child by index                                                   
   ///   @param id - the index to pick                                        
   ///   @return the child entity, or nullptr of none was found               
   Entity* Entity::GetChild(const Index& id) {
      return mChildren[id];
   }

   /// Get child by name and offset (searches for Traits::Name in children)   
   ///   @param name - name to seek                                           
   ///   @param offset - offset to seek                                       
   ///   @return the child entity, or nullptr of none was found               
   Entity* Entity::GetChild(const Token& name, const Index& offset) {
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
   void Entity::ResetRuntime(Runtime* newrt) {
      if (mOwnRuntime)
         return;

      mRuntime = newrt;
      for (auto child : mChildren)
         child->ResetRuntime(newrt);
   }

   /// Do a cascading temporal flow reset - all children that do not have own 
   /// flow, will incorporate the provided one                                
   ///   @param newrt - the new flow to set                                   
   void Entity::ResetFlow(Temporal* newflow) {
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
   Count Entity::AddUnit(Unit* unit) {
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
   Count Entity::RemoveUnit(Unit* unit) {
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
   void Entity::ReplaceUnit(Unit* replaceThis, Unit* withThis) {
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
   Count Entity::HasUnits(DMeta type) const {
      const auto found = mUnits.FindKeyIndex(type);
      return found ? mUnits.GetValue(found).GetCount() : 0;
   }

   /// Add a new trait to the universal entity                                
   ///   @param initializer - trait to shallow copy                           
   ///   @return the new trait instance                                       
   Trait* Entity::AddTrait(const Trait& initializer) {
      auto& list = mTraits[initializer.GetTrait()];
      list << initializer;
      mRefreshRequired = true;
      ENTITY_VERBOSE(initializer << " added");
      return &list.Last();
   }

   /// Remove a trait from the universal entity                               
   ///   @param id - type of trait to remove                                  
   Count Entity::RemoveTrait(TMeta id) {
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
   Count Entity::RemoveTrait(const Trait& prototype) {
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
   Count Entity::HasTraits(TMeta id) const {
      const auto found = mTraits.FindKeyIndex(id);
      if (found)
         return mTraits.GetValue(found).GetCount();
      return 0;
   }

   /// A fast check whether traits of the given type and value are inside     
   ///   @param id - trait to check                                           
   ///   @return the number of matching traits                                
   Count Entity::HasTraits(const Trait& prototype) const {
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

   /// Get the current runtime                                                
   ///   @return the pointer to the runtime                                   
   Runtime* Entity::GetRuntime() const noexcept {
      return mRuntime.Get();
   }

   /// Add/overwrite entity's name trait                                      
   ///   @param name - the name to set                                        
   void Entity::SetName(const Text& name) {
      auto found = GetLocalTrait<Traits::Name>();
      if (!found)
         AddTrait<Traits::Name>(name);
      else
         *found = name;
   }

   /// Get entity's name trait, if any                                        
   /// It can reside in one of the units                                      
   ///   @return the name, or empty string if no such trait was found here    
   Text Entity::GetName() const {
      Text name;
      if (!SeekValue<SeekStyle::Here, Traits::Name>(name))
         return {};
      return name;
   }

   /// Custom dispatcher, reflected via CT::Dispatcher                        
   /// Entity is a composite type, and its behavior depends on the coupled    
   /// units, so the custom dispatcher forwards verbs to these units          
   ///   @param verb - any verb                                               
   void Entity::Do(Verb& verb) {
      // Dispatch to entity first, using reflected and default verbs,   
      // but disallowing custom dispatch, because we're currently in it 
      // and there's a potential for infinite regress                   
      if (Resolvable::Run<false>(verb))
         return;

      // If verb is still not satisfied, dispatch to ALL units          
      mUnits.ForEachValue([&](Unit* unit) {
         Verb local {verb};
         if (unit->Run(local.ShortCircuit(false)))
            verb << Abandon(local.GetOutput());
      });
   }

   /// Create/Destroy stuff inside entity's context                           
   ///   @param verb - creation verb                                          
   void Entity::Create(Verb& verb) {
      if (verb.GetArgument().IsEmpty())
         return;

      TAny<Trait*> createdTraits;
      TAny<Unit*> createdUnits;
      TAny<Module*> createdModules;
      TAny<Entity*> createdChildren;

      const auto create = [&](const Construct& construct) {
         const auto count = static_cast<Count>(construct.GetCharge().mMass);
         for (Offset i = 0; i < count; ++i) {
            if (count != 1) {
               ENTITY_CREATION_VERBOSE_SELF(Logger::Yellow <<
                  "Charged creation - creating " << i + 1 << " of " << count);
            }

            if (construct.Is<Entity>()) {
               auto created = CreateChild(construct);
               if (created)
                  createdChildren << created;
            }
            else if (construct.CastsTo<Unit>())
               createdUnits += CreateDataInner(construct);
            else if (construct.CastsTo<Module>()) {
               auto created = GetRuntime()->InstantiateModule(construct);
               if (created)
                  createdModules << created;
            }
         }
      };

      // Scan the request                                               
      verb.ForEachDeep([&](const Block& group) {
         group.ForEach(
            [&](const Trait& trait) {
               ENTITY_CREATION_VERBOSE_SELF(
                  "Creating: " << Logger::Yellow << trait);
               createdTraits << AddTrait(trait);
            },
            [&](const Construct& construct) {
               if (construct.GetCharge().mMass > 0) {
                  ENTITY_CREATION_VERBOSE_SELF(
                     "Creating: " << Logger::Yellow << construct);
                  create(construct);
               }
            },
            [&](const MetaData* type) {
               ENTITY_CREATION_VERBOSE_SELF(
                  "Creating: " << Logger::Yellow << type->mToken);
               create(Construct(type));
            }
         );
      });

      verb << createdTraits;
      verb << createdUnits;
      verb << createdModules;
      verb << createdChildren;
   }

   /// Pick something from the entity - children, traits, units, modules      
   /// If this entity doesn't satisfy the query, a search will be performed   
   /// in all parents, climbing the hierarchy                                 
   ///   @param verb - the selection verb                                     
   void Entity::Select(Verb& verb) {
      // Probe every part of the argument and check if it matches       
      TAny<Trait> selectedTraits;
      TAny<Unit*> selectedUnits;
      TAny<Entity*> selectedEntities;
      bool mismatch = false;

      const auto selectTrait = [&](const MetaTrait* trait) {
         Trait found;
         if (!GetTrait(trait, found)) {
            mismatch = true;
            return false;
         }

         selectedTraits << found;
         return true;
      };

      const auto selectUnit = [&](const MetaData* type) {
         if (type->Is<Entity>())
            return true;

         auto found = GetUnit(type);
         if (!found) {
            mismatch = true;
            return false;
         }

         selectedUnits << found;
         return true;
      };

      const auto selectConstruct = [&](const Construct& construct) {
         if (construct.Is<Entity>()) {
            // Find an entity containing construct arguments            
            // Start with this one                                      
            Verbs::Select selector {construct.GetArgument()};
            Select(selector);
            if (!selector.GetOutput().IsEmpty()) {
               selectedEntities << this;
               return true;
            }
         }
         else if (construct.CastsTo<Unit>()) {
            // Find a unit containing construct arguments               
            if (!selectUnit(construct.GetType()))
               return false;

            // selectedComponents has been populated with results       
            // Filter them additionally by construct arguments          
            TAny<Unit*> filteredSelectedComponents;
            for (auto& unit : selectedUnits) {
               bool localMismatch = false;
               auto unitBlock = unit->GetBlock();
               construct.ForEachDeep([&](const Block& part) {
                  for (Offset i = 0; i < part.GetCount(); ++i) {
                     auto element = part.GetElementResolved(i);
                     Verbs::Select selector {element};
                     if (!Flow::DispatchFlat(unitBlock, selector)) {
                        // Abort on first failure                       
                        localMismatch = true;
                        return false;
                     }
                  }

                  return true;
               });

               if (!localMismatch) {
                  // The unit passes the test                           
                  filteredSelectedComponents << unit;
               }
            }

            // Substitute the selected units with the filtered ones     
            selectedUnits = Move(filteredSelectedComponents);
         }

         return false;
      };

      verb.ForEachDeep([&](const Block& part) {
         part.ForEach(
            [&](const Construct& construct) {
               return selectConstruct(construct);
            },
            [&](const Trait& trait) {
               return selectTrait(trait.GetTrait());
            },
            [&](const MetaTrait* trait) {
               return selectTrait(trait);
            },
            [&](const MetaData* type) {
               return selectUnit(type);
            }
         );

         return !mismatch;
      });

      if (!mismatch) {
         // We're not seeking an entity, but components/traits          
         if (!selectedTraits.IsEmpty()) {
            ENTITY_SELECTION_VERBOSE_SELF(Logger::Green 
               << "Trait(s) selected: " << selectedTraits);
            verb << selectedTraits;
         }

         if (!selectedUnits.IsEmpty()) {
            ENTITY_SELECTION_VERBOSE_SELF(Logger::Green
               << "Unit(s) selected: " << selectedUnits);
            verb << selectedUnits;
         }

         if (!selectedEntities.IsEmpty()) {
            ENTITY_SELECTION_VERBOSE_SELF(Logger::Green
               << "Entity(s) selected: " << selectedEntities);
            verb << selectedEntities;
         }
      }

      // Climb the hierarchy                                            
      if (!verb.IsDone() && mOwner)
         mOwner->Select(verb);
   }

   /// Create a child entity via default-construction                         
   ///   @param descriptor - instructions for the entity's creation           
   ///   @return the new child instance                                       
   Entity* Entity::CreateChild(const Any& descriptor) {
      mChildren.Emplace(this, descriptor);
      ENTITY_VERBOSE_SELF(mChildren.Last() << " added");
      return mChildren.Last();
   }

   /// Destroy a child that matched pointer                                   
   ///   @attention provided pointer is considered invalid after this call    
   ///   @param entity - entity instance to destroy                           
   ///   @return the number of destroyed children                             
   Count Entity::DestroyChild(Entity* entity) {
      const auto removed = mChildren.RemoveValue(entity);
      mRefreshRequired = removed;
      return removed;
   }

   /// Create all dependencies required for the production of 'type', if such 
   /// doesn't yet exist the this entity's upward hierarchy                   
   /// They will be added either immediately below to their own producers, or 
   /// by the Runtime as a fallback                                           
   ///   @param type - the type to build dependencies for                     
   ///   @return the resulting dependencies (top level)                       
   Any Entity::CreateDependencies(DMeta type) {
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
         return runtime->InstantiateModule(meta);
      }

      Logger::Error()
         << "Couldn't produce " << type << " from " << meta
         << " - the producer wasn't found, and couldn't be produced";
      LANGULUS_THROW(Construct, "Couldn't create dependencies");
   }

   /// Produce unit(s) from the entity's hierarchy                            
   ///   @param construct - instructions for the creation of the unit(s)      
   ///   @return all created units                                            
   Any Entity::CreateDataInner(const Construct& construct) {
      Verbs::Create creator {&construct};
      auto producers = CreateDependencies(construct.GetType());
      if (Scope::ExecuteVerb(producers, creator))
         return Abandon(creator.GetOutput());
      return {};
   }
   
} // namespace Langulus::Entry
