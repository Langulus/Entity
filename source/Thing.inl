///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Thing.hpp"
#include "Thing-Gather.inl"
#include "Thing-Seek.inl"


namespace Langulus::Entity
{
   
   /// Create a root thing (convenience function)                             
   ///   @tparam CREATE_FLOW - should a temporal flow be created in the root? 
   ///   @param modules... - names of modules to load                         
   ///   @return a root thing, with a runtime, a flow, and a name "ROOT"      
   template<bool CREATE_FLOW>
   Thing Thing::Root(CT::String auto&&...modules) {
      Thing root;
      root.SetName("ROOT");
      root.CreateRuntime();
      if constexpr (CREATE_FLOW)
         root.CreateFlow();
      (root.LoadMod(modules), ...);
      return Abandon(root);
   }

   /// Create a child Thing with the provided arguments                       
   ///   @param arguments - instructions for the entity's creation            
   ///   @return the new child instance                                       
   template<class...T>
   Ref<Thing> Thing::CreateChild(T&&...arguments) {
      ENTITY_VERBOSE_SELF_TAB(
         "Producing child (at ", GetReferences(), " references): ");
      Ref<Thing> newThing;
      newThing.New(this, Many {Forward<T>(arguments)...});
      return Abandon(newThing);
   }

   /// Inner creation routine                                                 
   ///   @tparam T - type of instructions for creation                        
   ///   @param verb - original create verb to output to                      
   ///   @param stuff - creation instructions                                 
   template<class T>
   void Thing::CreateInner(Verb& verb, const T& stuff) {
      if constexpr (CT::Deep<T> or CT::Neat<T>) {
         // Nest if deep/neat                                           
         stuff.ForEachDeep(
            [this, &verb](const Trait& trait) {
               verb << AddTrait(trait);
            },
            [this, &verb](const Construct& construct) {
               CreateInner(verb, construct);
            },
            [this, &verb](const Neat& neat) {
               CreateInner(verb, neat);
            },
            [this, &verb](const DMeta& type) {
               CreateInner(verb, type);
            }
         );
      }
      else if constexpr (CT::Exact<T, DMeta>) {
         // Instantiate a type without any arguments                    
         if (stuff->template Is<Thing>()) {
            // Instantiate a child Thing                                
            verb << CreateChild();
         }
         else if (stuff->template Is<Runtime>()) {
            // Instantiate a runtime                                    
            verb << CreateRuntime();
         }
         else if (stuff->template Is<Temporal>()) {
            // Instantiate a temporal flow                              
            verb << CreateFlow();
         }
         else if (stuff->template CastsTo<A::Module>()) {
            // Instantiate a module from the runtime                    
            auto runtime = GetRuntime();
            auto dependency = runtime->GetDependency(stuff);
            verb << runtime->InstantiateModule(dependency);
         }
         else {
            // Instantiate anything else                                
            verb << CreateData(Construct {stuff});
         }
      }
      else if constexpr (CT::Construct<T>) {
         // Instantiate a type, with charge and arguments               
         const auto count = static_cast<int>(std::ceil(stuff.GetCharge().mMass));
         for (int i = 0; i < count; ++i) {
            if (count != 1) {
               ENTITY_CREATION_VERBOSE_SELF(Logger::Yellow,
                  "Charged creation - creating ", i + 1, " of ", count);
            }

            if (stuff.template Is<Thing>()) {
               // Instantiate a child Thing                             
               verb << CreateChild(stuff.GetDescriptor());
            }
            else if (stuff.template Is<Runtime>()) {
               // Instantiate a runtime                                 
               verb << CreateRuntime();
            }
            else if (stuff.template Is<Temporal>()) {
               // Instantiate a temporal flow                           
               verb << CreateFlow();
            }
            else if (stuff.template CastsTo<A::Module>()) {
               // Instantiate a module from the runtime                 
               auto runtime = GetRuntime();
               auto dependency = runtime->GetDependency(stuff.GetType());
               verb << runtime->InstantiateModule(dependency, stuff.GetDescriptor());
            }
            else {
               // Instantiate anything else                             
               verb << CreateData(stuff);
            }
         }
      }
      else static_assert(false, "Unsupported descriptor");
   }

   /// Add a child                                                            
   ///   @attention assumes entity is a valid pointer                         
   ///   @tparam TWOSIDED - true to also set the entity's owner;              
   ///                      used mainly internally to avoid endless loops     
   ///   @param entity - entity instance to add as child                      
   ///   @return the number of added children                                 
   template<bool TWOSIDED>
   Count Thing::AddChild(Thing* entity) {
      LANGULUS_ASSUME(UserAssumes, entity, "Bad entity pointer");

      const auto added = mChildren.Merge(IndexBack, entity);
      if constexpr (TWOSIDED) {
         if (added) {
            if (entity->mOwner != this) {
               if (entity->mOwner)
                  entity->mOwner->RemoveChild<false>(entity);

               entity->mOwner = this;
               entity->mRefreshRequired = true;
               ENTITY_VERBOSE_SELF(entity, "'s owner overwritten");
            }

            ENTITY_VERBOSE_SELF(entity, " added as child");
         }
      }

      return added;
   }
      
   /// Remove a child that matches pointer                                    
   ///   @attention assumes entity is a valid pointer                         
   ///   @tparam TWOSIDED - true to also remove entity's owner;               
   ///                      used mainly internally to avoid endless loops     
   ///   @param entity - entity instance to remove from children              
   ///   @return the number of removed children                               
   template<bool TWOSIDED>
   Count Thing::RemoveChild(Thing* entity) {
      LANGULUS_ASSUME(UserAssumes, entity, "Bad entity pointer");
      
      const auto removed = mChildren.Remove(entity);
      if constexpr (TWOSIDED) {
         if (removed) {
            if (entity->mOwner == this) {
               entity->mOwner = nullptr;
               entity->mRefreshRequired = true;
               ENTITY_VERBOSE_SELF(entity, "'s owner overwritten");
            }

            ENTITY_VERBOSE_SELF(entity, " removed from children");
         }
      }

      return removed;
   }

   /// Execute verb in the hierarchy, searching for valid context in the      
   /// given direction                                                        
   ///   @tparam SEEK - the direction in which to seek a valid context        
   ///   @param verb - the verb to execute                                    
   ///   @return verb output                                                  
   template<Seek SEEK, CT::VerbBased V>
   V& Thing::RunIn(V& verb) {
      if constexpr (SEEK & Seek::Here) {
         // Execute here                                                
         if (Run(verb).IsDone())
            return verb;
      }

      if constexpr (SEEK & Seek::Above) {
         // Execute in parents up to root, if requested                 
         if (mOwner and mOwner->template
         RunIn<Seek::HereAndAbove>(verb).IsDone())
            return verb;
      }

      if constexpr (SEEK & Seek::Below) {
         // Execute in children, if requested                           
         for (auto& child : mChildren) {
            V local = verb;
            local.ShortCircuit(false);
            verb << Abandon(child->template
               RunIn<Seek::HereAndBelow>(local).GetOutput());
         }
      }

      return verb;
   }

   /// Execute verb in this thing only, scanning units for required verbs     
   ///   @param verb - the verb to execute                                    
   ///   @return verb output                                                  
   template<CT::VerbBased V>
   V& Thing::Run(V& verb) {
      try {
         // Dispatch to entity first, using reflected and default verbs,
         // but disallowing custom dispatch, because we're currently in 
         // it and there's a potential for infinite regress             
         Resolvable::Run<false>(verb);
      }
      catch (...) {}

      if (verb.IsDone())
         return verb;

      // If verb is still not satisfied, dispatch to ALL units          
      for (auto& unit : mUnitsList) {
         try {
            V local = verb;
            local.ShortCircuit(false);
            unit->Run(local);

            if (local.IsDone()) {
               // The local verb may or may not have an output, but a   
               // success should always be carried over                 
               verb.Done();
               verb << Abandon(local.GetOutput());
            }
         }
         catch (...) {}
      }

      return verb;
   }
   
   /// Register unit by all its bases in mUnitsAmbiguous                      
   ///   @param unit - the unit instance to register                          
   ///   @param type - the type to register the unit as                       
   inline void Thing::AddUnitBases(A::Unit* unit, DMeta type) {
      const auto found = mUnitsAmbiguous.FindIt(type);
      if (found)
         found.GetValue() << unit;
      else
         mUnitsAmbiguous.Insert(type, unit);

      for (auto& base : type->mBases) {
         if (base.mType->IsExact<A::Unit>())
            break;

         AddUnitBases(unit, base.mType);
      }
   }

   /// Unregister unit by all its bases in mUnitsAmbiguous                    
   ///   @param unit - the unit instance to unregister                        
   ///   @param type - the type to unregister the unit as                     
   inline void Thing::RemoveUnitBases(A::Unit* unit, DMeta type) {
      const auto found = mUnitsAmbiguous.FindIt(type);
      if (found) {
         auto& set = found.GetValue();
         if (set.Remove(unit) and not set)
            mUnitsAmbiguous.RemoveIt(found);
      }

      for (auto& base : type->mBases) {
         if (base.mType->IsExact<A::Unit>())
            break;

         RemoveUnitBases(unit, base.mType);
      }
   }

   /// Add a new unit to the entity                                           
   /// Adding units coupled with different runtimes is not allowed            
   /// You can only duplicate units, if they're unique instances memorywise   
   ///   @tparam TWOSIDED - if true, will both add unit to thing, and add     
   ///                      the thing as an owner for the unit;               
   ///                      used mainly internally to avoid endless loops     
   ///   @param unit - the unit to add                                        
   ///   @return 1 if unit has been added                                     
   template<bool TWOSIDED>
   Count Thing::AddUnit(A::Unit* unit) {
      // Check if the unit instance is already registered here          
      const auto meta = unit->GetType();
      const auto found = mUnitsAmbiguous.FindIt(meta);
      if (found and found.GetValue().Find(unit))
         return 0;

      // We must guarantee, that no unit is coupled to entities with    
      // different runtimes!                                            
      for (const auto& owners : unit->mOwners) {
         LANGULUS_ASSERT(owners->GetRuntime() == GetRuntime(), Access,
            "Coupling a unit to multiple runtimes is not allowed");
      }

      // Log self before unit being added, it might change name         
      ENTITY_VERBOSE_SELF("");

      if constexpr (TWOSIDED) {
         #if LANGULUS(SAFE)
            unit->mOwners <<= this;
         #else
            unit->mOwners << this;
         #endif
      }

      mUnitsList << unit;
      AddUnitBases(unit, meta);
      mRefreshRequired = true;

      ENTITY_VERBOSE(
         unit, " added as unit (now at ", GetReferences(), " references)");
      return 1;
   }

   /// Remove an unit instance from the thing                                 
   ///   @tparam TWOSIDED - if true, will both remove unit from thing, and    
   ///                      then remove the thing from unit's owners;         
   ///                      used mainly internally to avoid endless loops     
   ///   @param unit - unit to remove from the entity                         
   ///   @return 1 if unit has been removed                                   
   template<bool TWOSIDED>
   Count Thing::RemoveUnit(A::Unit* unit) {
      const auto meta = unit->GetType();
      const auto foundType = mUnitsAmbiguous.FindIt(meta);
      if (not foundType)
         return 0;

      const auto unitIndex = foundType.GetValue().Find(unit);
      if (unitIndex) {
         // Decouple before unit is destroyed                           
         if constexpr (TWOSIDED)
            unit->mOwners.Remove(this);

         // Notify all other units about the environment change         
         mRefreshRequired = true;
         ENTITY_VERBOSE_SELF(unit, " removed from units");

         // Dereference (and eventually destroy) unit                   
         RemoveUnitBases(unit, meta);
         mUnitsList.Remove(unit);
         return 1;
      }

      return 0;
   }

   /// Remove all units that are derived from the provided type               
   ///   @tparam T - the type of units to remove, use Unit for all            
   ///   @tparam TWOSIDED - if true, will both remove unit from thing, and    
   ///                      then remove the thing from unit's owners;         
   ///                      used mainly internally to avoid endless loops     
   ///   @return the number of removed units                                  
   template<CT::Unit T, bool TWOSIDED>
   Count Thing::RemoveUnits() {
      if constexpr (CT::Same<T, A::Unit>) {
         // Remove all units                                            
         const auto removed = mUnitsList.GetCount();
         if constexpr (TWOSIDED) {
            // Decouple before units are (potentially) destroyed        
            for (auto& unit : mUnitsList)
               unit->mOwners.Remove(this);
         }

         mUnitsList.Reset();
         mUnitsAmbiguous.Reset();
         mRefreshRequired = true;
         ENTITY_VERBOSE_SELF("All ", removed, " units were removed");
         return removed;
      }
      else {
         // Remove units of a specific type                             
         const auto meta = MetaOf<Decay<T>>();
         const auto found = mUnitsAmbiguous.Find(meta);
         if (not found)
            return 0;

         // List intentionally shallow-copied, its memory will diverge  
         // upon RemoveUnit                                             
         auto list = mUnitsAmbiguous.GetValue(found);
         Count removed {};
         for (auto& unit : list)
            removed += RemoveUnit(unit);
         return removed;
      }
   }

   /// Count the number of matching units in this entity                      
   ///   @tparam T - the type of units to seach for, use Unit for all         
   ///   @return the number of matching units                                 
   template<CT::Unit T> LANGULUS(INLINED)
   Count Thing::HasUnits() const {
      return HasUnits(MetaOf<Decay<T>>());
   }

   /// Get the list of units, in order of addition                            
   ///   @return a reference to the list of units                             
   LANGULUS(INLINED)
   auto Thing::GetUnits() const noexcept -> const UnitList& {
      return mUnitsList;
   }

   /// Get the ambiguous map of units                                         
   ///   @return a reference to the map of units                              
   LANGULUS(INLINED)
   auto Thing::GetUnitsMap() const noexcept -> const UnitMap& {
      return mUnitsAmbiguous;
   }

   /// Create a unit by static type and arguments, relying on producers       
   /// in the hierarchy                                                       
   ///   @tparam T - the unit to create                                       
   ///   @tparam A... - arguments for the unit's creation                     
   ///   @param arguments... - the arguments to provide for construct         
   ///   @return the created unit(s)                                          
   template<CT::Unit T, class...A> LANGULUS(INLINED)
   Many Thing::CreateUnit(A&&...arguments) {
      return CreateData(
         Construct::From<Decay<T>>(Forward<A>(arguments)...)
      );
   }

   /// Create default-initialized instances of each unit type                 
   ///   @tparam T... - the unit types to instantiate                         
   ///   @return the created unit(s)                                          
   template<CT::Unit...T> LANGULUS(INLINED)
   Many Thing::CreateUnits() {
      Many result;
      (result.SmartPush(IndexBack, CreateUnit<T>()), ...);
      return result;
   }

#if LANGULUS_FEATURE(MANAGED_REFLECTION)
   /// Create a unit by type token and arguments, relying on producers        
   /// in the hierarchy                                                       
   ///   @tparam A... - arguments for the unit's creation                     
   ///   @param token - the unit type token                                   
   ///   @param arguments... - the arguments to provide for construct         
   ///   @return the created unit(s)                                          
   template<class... A> LANGULUS(INLINED)
   Many Thing::CreateUnitToken(const Token& token, A&&...arguments) {
      return CreateData(
         Construct::FromToken(token, Forward<A>(arguments)...)
      );
   }
#endif

   /// Get a unit by a static type and an optional offset                     
   ///   @tparam T - the type of unit we're searching for                     
   ///   @param offset - optional offset (Nth match)                          
   ///   @return the unit if found, or nullptr if not                         
   template<CT::Unit T> LANGULUS(INLINED)
   Decay<T>* Thing::GetUnit(Index offset) {
      if constexpr (not CT::Same<T, A::Unit>) {
         return static_cast<Decay<T>*>(
            GetUnitMeta(MetaOf<Decay<T>>(), offset)
         );
      }
      else {
         return static_cast<Decay<T>*>(
            GetUnitMeta(DMeta {}, offset)
         );
      }
   }

   /// Get a unit by a static type and an optional offset (const)             
   ///   @tparam T - the type of unit we're searching for                     
   ///   @param offset - optional offset (Nth match)                          
   ///   @return the unit if found, or nullptr if not                         
   template<CT::Unit T> LANGULUS(INLINED)
   const Decay<T>* Thing::GetUnit(Index offset) const {
      if constexpr (not CT::Same<T, A::Unit>) {
         return static_cast<const Decay<T>*>(
            GetUnitMeta(MetaOf<Decay<T>>(), offset)
         );
      }
      else {
         return static_cast<const Decay<T>*>(
            GetUnitMeta(DMeta {}, offset)
         );
      }
   }

   #if LANGULUS_FEATURE(MANAGED_REFLECTION)
      /// Get a unit by a token, and an optional offset (const)               
      /// This is available only if managed reflection feature is enabled     
      ///   @param token - unit type token                                    
      ///   @param offset - optional offset (Nth match)                       
      ///   @return the unit if found, or nullptr if not                      
      LANGULUS(INLINED)
      const A::Unit* Thing::GetUnitMeta(const Token& token, Index offset) const {
         return const_cast<Thing*>(this)->GetUnitMeta(token, offset);
      }

      /// Get a unit by a token, and an optional offset, then dynamic_cast it 
      /// This is available only if managed reflection feature is enabled     
      ///   @tparam T - the type of unit we're casting to                     
      ///   @param token - unit type token                                    
      ///   @param offset - optional offset (Nth match)                       
      ///   @return the unit if found, or nullptr if not                      
      template<CT::Unit T> LANGULUS(INLINED)
      Decay<T>* Thing::GetUnitAs(const Token& token, Index offset) {
         return dynamic_cast<Decay<T>*>(GetUnitMeta(token, offset));
      }
   #endif

   template<CT::TraitBased T> LANGULUS(INLINED)
   Trait Thing::GetTrait(Index offset) {
      return GetTrait(MetaTraitOf<T>(), offset);
   }

   /// Get local trait by static type and offset                              
   ///   @tparam T - the trait to search for                                  
   ///   @param offset - the offset of the trait to return (optional)         
   ///   @return the trait or nullptr if none found                           
   template<CT::TraitBased T> LANGULUS(INLINED)
   Trait* Thing::GetLocalTrait(Index offset) {
      return GetLocalTrait(MetaTraitOf<T>(), offset);
   }

   /// Get local trait by static type and offset                              
   ///   @tparam T - the trait to search for                                  
   ///   @param offset - the offset of the trait to return                    
   ///   @return the trait or nullptr if none found                           
   template<CT::TraitBased T> LANGULUS(INLINED)
   const Trait* Thing::GetLocalTrait(Index offset) const {
      return const_cast<Thing&>(*this).template GetLocalTrait<T>(offset);
   }

   /// Produce constructs (including units) from the hierarchy                
   ///   @attention assumes construct has a valid type                        
   ///   @tparam SEEK - what part of the hierarchy to use for the creation    
   ///   @param construct - instructions for the creation of the data         
   ///   @return created data                                                 
   template<Seek SEEK>
   Many Thing::CreateData(const Construct& construct) {
      LANGULUS_ASSUME(UserAssumes, construct.GetType(),
         "Invalid construct type");

      const auto type = construct.GetType();
      const auto producer = type and type->mProducerRetriever
         ? type->mProducerRetriever() : nullptr;
      Construct descriptor = construct;

      ENTITY_VERBOSE_SELF(
         "Acting as producer context for making `", 
         type, "` (at ", GetReferences(), " references)"
      );

      // Implicitly add a parent trait to descriptor, if one isn't      
      // already added - it will be stripped later, when normalizing    
      // the descriptor when producing the item from a factory          
      Traits::Parent parent;
      if (not descriptor->ExtractTrait<Traits::Parent>(parent)) {
         parent = this;
         parent.MakeMissing();
         descriptor << parent;
         ENTITY_VERBOSE_SELF(
            "Referenced as Traits::Parent (now at ",
            GetReferences(), " references)"
         );
      }
      else if (parent.IsMissing())
         parent << this;

      if (producer) {
         // Data has a specific producer, we can narrow the required    
         // contexts for creation a lot                                 
         if (producer->template CastsTo<A::Unit>()) {
            // Data is producible from a unit                           
            auto producers = GatherUnits<SEEK>(producer);
            LANGULUS_ASSERT(producers, Construct, 
               "No producers", " (of unit type `", producer, "`) available "
               "in hierarchy for construct: ", construct
            );

            // Potential unit producers found, attempt creation         
            producers.MakeOr();
            Verbs::Create creator {descriptor};
            if (Flow::DispatchFlat(producers, creator))
               return Abandon(creator.GetOutput());

            Logger::Error(
               "Failed to create `", Logger::PushDarkYellow, type, Logger::Pop,
               "` in unit(s) ", Logger::PushDarkYellow, producers, Logger::Pop,
               ": ", descriptor
            );
         }
         else if (producer->template CastsTo<A::Module>()) {
            // Data is producible from a module                         
            auto producers = GetRuntime()->GetModules(producer);
            LANGULUS_ASSERT(producers, Construct,
               "No producers", " (of module type `", producer, "`) available "
               "in hierarchy for construct: ", construct
            );

            // Potential module producers found, attempt creation       
            producers.MakeOr();
            Verbs::Create creator {descriptor};
            if (Flow::DispatchFlat(producers, creator))
               return Abandon(creator.GetOutput());

            Logger::Error(
               "Failed to create `", Logger::PushDarkYellow, type, Logger::Pop,
               "` in module(s) ", Logger::PushDarkYellow, producers, Logger::Pop,
               ": ", descriptor
            );
         }
         else if (producer->template CastsTo<Thing>()) {
            // Data is producible from a thing                          
            TODO();
         }
      }
      else if (type->mIsAbstract and not type->mConcreteRetriever) {
         // Data doesn't have a specific producer, but it is abstract   
         // so we know that only a module/unit can concretize it        
         // Gather all units in the desired part of the hierarchy       
         auto producers = GatherUnits<A::Unit, SEEK>();
         if (producers) {
            // Potential unit producers found, attempt creation there   
            producers.MakeOr();
            Verbs::Create creator {descriptor};
            if (Flow::DispatchFlat(producers, creator))
               return Abandon(creator.GetOutput());

            Logger::Error(
               "Failed to create abstract `", Logger::PushDarkYellow, type, Logger::Pop,
               "` in unit(s) ", Logger::PushDarkYellow, producers, Logger::Pop,
               ": ", descriptor
            );
         }

         //TODO try modules too?
         TODO();
      }
      else {
         // Data is not abstract, nor has a producer, so just make it   
         // right here if possible, passing the descriptor over, if     
         // such constructor is reflected. If it's a unit, its          
         // descriptor is resposible for registering it with the parent 
         // via the Traits::Parent trait                                
         Verbs::Create creator {descriptor};
         if (Verbs::Create::ExecuteStateless(creator))
            return Abandon(creator.GetOutput());
      }

      LANGULUS_THROW(Construct, "Unable to create data");
      return {};
   }

} // namespace Langulus::Entity

namespace Langulus::A
{

   /// Execute verb in all owners, seeking valid context in the specified     
   /// seek direction                                                         
   ///   @tparam SEEK - direction to seek valid execution context in          
   ///   @param verb - the verb to execute                                    
   ///   @return the verb output                                              
   template<Seek SEEK, CT::VerbBased V>
   V& A::Unit::RunIn(V& verb) {
      if (not mOwners) {
         Logger::Warning(Self(), "No owners available for executing: ", verb);
         return verb;
      }

      // Dispatch to all owner, accumulate outputs                      
      for (auto& owner : mOwners) {
         V local = verb;
         local.ShortCircuit(false);
         verb << Abandon(owner->template RunIn<SEEK>(local).GetOutput());
      }

      return verb;
   }

} // namespace Langulus::A