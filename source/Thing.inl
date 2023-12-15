///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Thing.hpp"
#include "Runtime.hpp"
#include "Thing-Gather.inl"
#include <Flow/Verbs/Interpret.hpp>
#include <Flow/Verbs/Create.hpp>

#if 0
   #define ENTITY_VERBOSE_ENABLED() 1
   #define ENTITY_VERBOSE_SELF(...)            Logger::Verbose(Self(), __VA_ARGS__)
   #define ENTITY_VERBOSE(...)                 Logger::Append(__VA_ARGS__)
   #define ENTITY_CREATION_VERBOSE_SELF(...)   Logger::Verbose(Self(), __VA_ARGS__)
   #define ENTITY_SELECTION_VERBOSE_SELF(...)  Logger::Verbose(Self(), __VA_ARGS__)
#else
   #define ENTITY_VERBOSE_ENABLED() 0
   #define ENTITY_VERBOSE_SELF(...)
   #define ENTITY_VERBOSE(...)
   #define ENTITY_CREATION_VERBOSE_SELF(...)
   #define ENTITY_SELECTION_VERBOSE_SELF(...)
#endif


namespace Langulus::Entity
{
   
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
         else if (stuff->template CastsTo<Module>()) {
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
            else if (stuff.template CastsTo<Module>()) {
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
      else LANGULUS_ERROR("Unsupported descriptor");
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

      const auto added = mChildren.Merge(entity);
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
   template<Seek SEEK>
   Any Thing::RunIn(Verb& verb) {
      if constexpr (SEEK & Seek::Here) {
         // Execute here                                                
         Do(verb);
         if (verb.IsDone())
            return verb.GetOutput();
      }

      if constexpr (SEEK & Seek::Above) {
         // Execute in parents up to root, if requested                 
         if (mOwner) {
            mOwner->template RunIn<Seek::HereAndAbove>(verb);
            if (verb.IsDone())
               return verb.GetOutput();
         }
      }

      if constexpr (SEEK & Seek::Below) {
         // Execute in children, if requested                              
         for (auto& child : mChildren) {
            Verb local {verb};
            local.ShortCircuit(false);
            verb << Abandon(child->template RunIn<Seek::HereAndBelow>(local));
         }

         if (verb.IsDone())
            return verb.GetOutput();
      }

      return {};
   }
   
   /// Register unit by all its bases in mUnitsAmbiguous                      
   ///   @param unit - the unit instance to register                          
   ///   @param type - the type to register the unit as                       
   inline void Thing::AddUnitBases(Unit* unit, DMeta type) {
      const auto found = mUnitsAmbiguous.FindIt(type);
      if (found)
         found->mValue << unit;
      else
         mUnitsAmbiguous.Insert(type, {unit});

      for (auto& base : type->mBases) {
         if (base.mType->IsExact<Unit>())
            break;
         AddUnitBases(unit, base.mType);
      }
   }

   /// Unregister unit by all its bases in mUnitsAmbiguous                    
   ///   @param unit - the unit instance to unregister                        
   ///   @param type - the type to unregister the unit as                     
   inline void Thing::RemoveUnitBases(Unit* unit, DMeta type) {
      const auto found = mUnitsAmbiguous.FindIt(type);
      if (found) {
         auto& set = found->mValue;
         if (set.Remove(unit) and not set)
            mUnitsAmbiguous.RemoveIt(found);
      }

      for (auto& base : type->mBases) {
         if (base.mType->IsExact<Unit>())
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
   Count Thing::AddUnit(Unit* unit) {
      // Check if the unit instance is already registered here          
      const auto meta = unit->GetType();
      const auto found = mUnitsAmbiguous.FindIt(meta);
      if (found and found->mValue.Find(unit))
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
      ENTITY_VERBOSE(unit, " added as unit");
      return 1;
   }

   /// Remove an unit instance from the thing                                 
   ///   @tparam TWOSIDED - if true, will both remove unit from thing, and    
   ///                      then remove the thing from unit's owners;         
   ///                      used mainly internally to avoid endless loops     
   ///   @param unit - unit to remove from the entity                         
   ///   @return 1 if unit has been removed                                   
   template<bool TWOSIDED>
   Count Thing::RemoveUnit(Unit* unit) {
      const auto meta = unit->GetType();
      const auto foundType = mUnitsAmbiguous.FindIt(meta);
      if (not foundType)
         return 0;

      const auto unitIndex = foundType->mValue.Find(unit);
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
      if constexpr (CT::Same<T, Unit>) {
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
   template<CT::Unit T>
   LANGULUS(INLINED)
   Count Thing::HasUnits() const {
      return HasUnits(MetaOf<Decay<T>>());
   }

   /// Get the list of units, in order of addition                            
   ///   @return a reference to the list of units                             
   LANGULUS(INLINED)
   const UnitList& Thing::GetUnits() const noexcept {
      return mUnitsList;
   }

   /// Create a unit by static type and arguments, relying on producers       
   /// in the hierarchy                                                       
   ///   @tparam T - the unit to create                                       
   ///   @tparam A... - arguments for the unit's creation                     
   ///   @param arguments... - the arguments to provide for construct         
   ///   @return the created unit(s)                                          
   template<CT::Unit T, class... A>
   LANGULUS(INLINED)
   Any Thing::CreateUnit(A&&...arguments) {
      return CreateData(
         Construct::From<Decay<T>>(Forward<A>(arguments)...)
      );
   }

#if LANGULUS_FEATURE(MANAGED_REFLECTION)
   /// Create a unit by type token and arguments, relying on producers        
   /// in the hierarchy                                                       
   ///   @tparam A... - arguments for the unit's creation                     
   ///   @param token - the unit type token                                   
   ///   @param arguments... - the arguments to provide for construct         
   ///   @return the created unit(s)                                          
   template<class... A>
   LANGULUS(INLINED)
   Any Thing::CreateUnitToken(const Token& token, A&&...arguments) {
      return CreateData(
         Construct::FromToken(token, Forward<A>(arguments)...)
      );
   }
#endif

   /// Get a unit by a static type and an optional offset                     
   ///   @tparam T - the type of unit we're searching for                     
   ///   @param offset - optional offset (Nth match)                          
   ///   @return the unit if found, or nullptr if not                         
   template<CT::Unit T>
   LANGULUS(INLINED)
   Decay<T>* Thing::GetUnit(Index offset) {
      if constexpr (not CT::Same<T, Unit>) {
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
   template<CT::Unit T>
   LANGULUS(INLINED)
   const Decay<T>* Thing::GetUnit(Index offset) const {
      if constexpr (not CT::Same<T, Unit>) {
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
      const Unit* Thing::GetUnitMeta(const Token& token, Index offset) const {
         return const_cast<Thing*>(this)->GetUnitMeta(token, offset);
      }

      /// Get a unit by a token, and an optional offset, then dynamic_cast it 
      /// This is available only if managed reflection feature is enabled     
      ///   @tparam T - the type of unit we're casting to                     
      ///   @param token - unit type token                                    
      ///   @param offset - optional offset (Nth match)                       
      ///   @return the unit if found, or nullptr if not                      
      template<CT::Unit T>
      LANGULUS(INLINED)
      Decay<T>* Thing::GetUnitAs(const Token& token, Index offset) {
         return dynamic_cast<Decay<T>*>(GetUnitMeta(token, offset));
      }
   #endif

   template<CT::Trait T>
   LANGULUS(INLINED)
   Trait Thing::GetTrait(const Index& offset) {
      return GetTrait(T::GetTrait(), offset);
   }

   /// Get local trait by static type and offset                              
   ///   @tparam T - the trait to search for                                  
   ///   @param offset - the offset of the trait to return (optional)         
   ///   @return the trait or nullptr if none found                           
   template<CT::Trait T>
   LANGULUS(INLINED)
   Trait* Thing::GetLocalTrait(const Index& offset) {
      return GetLocalTrait(T::GetTrait(), offset);
   }

   /// Get local trait by static type and offset                              
   ///   @tparam T - the trait to search for                                  
   ///   @param offset - the offset of the trait to return                    
   ///   @return the trait or nullptr if none found                           
   template<CT::Trait T>
   LANGULUS(INLINED)
   const Trait* Thing::GetLocalTrait(const Index& offset) const {
      return const_cast<Thing&>(*this).template GetLocalTrait<T>(offset);
   }

   /// Produce constructs (including units) from the hierarchy                
   ///   @attention assumes construct has a valid type                        
   ///   @tparam SEEK - what part of the hierarchy to use for the creation    
   ///   @param construct - instructions for the creation of the data         
   ///   @return created data                                                 
   template<Seek SEEK>
   Any Thing::CreateData(const Construct& construct) {
      LANGULUS_ASSUME(UserAssumes, construct.GetType(),
         "Invalid construct type");
      const auto type = construct.GetType();
      const auto producer = type and type->mProducerRetriever
         ? type->mProducerRetriever() : nullptr;
      Construct descriptor {construct};

      // Implicitly add a parent trait to descriptor, if one isn't      
      // already added - it will be stripped later, when normalizing    
      // the descriptor when producing the item from a factory          
      if (not descriptor.GetDescriptor().Get<Traits::Parent>())
         descriptor << Traits::Parent {this};

      if (producer) {
         // Data has a specific producer, we can narrow the required    
         // contexts for creation a lot                                 
         if (producer->template CastsTo<Unit>()) {
            // Data is producible from a unit                           
            auto producers = GatherUnits<SEEK>(producer);
            LANGULUS_ASSERT(producers, Construct, 
               "No producers", " (of unit type `", producer, "`) available "
               "in hierarchy for construct: ", construct
            );

            // Potential unit producers found, attempt creation         
            producers.MakeOr();
            Verbs::Create creator {&descriptor};
            if (Flow::DispatchFlat(producers, creator))
               return Abandon(creator.GetOutput());

            Logger::Error(
               "Failed to create `", Logger::Push, Logger::DarkYellow, 
               type, Logger::Pop, "` in unit(s) ", Logger::Push, 
               Logger::DarkYellow, producers, Logger::Pop,
               ": ", descriptor
            );
         }
         else if (producer->template CastsTo<Module>()) {
            // Data is producible from a module                         
            auto producers = GetRuntime()->GetModules(producer);
            LANGULUS_ASSERT(producers, Construct,
               "No producers", " (of module type `", producer, "`) available "
               "in hierarchy for construct: ", construct
            );

            // Potential module producers found, attempt creation       
            producers.MakeOr();
            Verbs::Create creator {&descriptor};
            if (Flow::DispatchFlat(producers, creator))
               return Abandon(creator.GetOutput());

            Logger::Error(
               "Failed to create `", Logger::Push, Logger::DarkYellow,
               type, Logger::Pop, "` in module(s) ", Logger::Push,
               Logger::DarkYellow, producers, Logger::Pop,
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
         auto producers = GatherUnits<Unit, SEEK>();
         if (producers) {
            // Potential unit producers found, attempt creation there   
            producers.MakeOr();
            Verbs::Create creator {&descriptor};
            if (Flow::DispatchFlat(producers, creator))
               return Abandon(creator.GetOutput());

            Logger::Error(
               "Failed to create abstract `", Logger::Push, Logger::DarkYellow,
               type, Logger::Pop, "` in unit(s) ", Logger::Push,
               Logger::DarkYellow, producers, Logger::Pop,
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
         Verbs::Create creator {&descriptor};
         if (Verbs::Create::ExecuteStateless(creator))
            return Abandon(creator.GetOutput());
         
         Logger::Error(
            "Failed to create `", Logger::Push, Logger::DarkYellow,
            type, Logger::Pop, "` statelessly: ", descriptor
         );
      }

      LANGULUS_THROW(Construct, "Unable to create data");
      return {};
   }

   /// Execute verb in all owners, seeking valid context in the specified     
   /// seek direction                                                         
   ///   @tparam SEEK - direction to seek valid execution context in          
   ///   @param verb - the verb to execute                                    
   ///   @return the verb output                                              
   template<Seek SEEK>
   Any Unit::RunIn(Verb& verb) {
      if (not mOwners) {
         Logger::Warning(Self(), "No owners available for executing: ", verb);
         return false;
      }

      // Dispatch to all owner, accumulate outputs                      
      for (auto& owner : mOwners) {
         Verb local {verb};
         local.ShortCircuit(false);
         verb << Abandon(owner->template RunIn<SEEK>(local));
      }

      return verb.GetOutput();
   }

} // namespace Langulus::Entity

#undef ENTITY_VERBOSE_ENABLED
#undef ENTITY_VERBOSE_SELF
#undef ENTITY_VERBOSE
#undef ENTITY_CREATION_VERBOSE_SELF
#undef ENTITY_SELECTION_VERBOSE_SELF
