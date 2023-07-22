///                                                                           
/// Langulus::Entity                                                          
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>                    
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
   
   /// Remove a child                                                         
   ///   @attention assumes entity is a valid pointer                         
   ///   @tparam TWOSIDED - true to also set the entity's owner;              
   ///                      used mainly internally to avoid endless loops     
   ///   @param entity - entity instance to add as child                      
   ///   @return the number of added children                                 
   template<bool TWOSIDED>
   Count Thing::AddChild(Thing* entity) {
      LANGULUS_ASSUME(UserAssumes, nullptr != entity, "Bad entity pointer");

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
      LANGULUS_ASSUME(UserAssumes, nullptr != entity, "Bad entity pointer");
      
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

   /// Execute verb in the hierarchy                                          
   ///   @tparam SEEK - where in the hierarchy to execute                     
   ///   @param verb - the verb to execute                                    
   ///   @return true if succesfully executed                                 
   template<Seek SEEK>
   bool Thing::DoInHierarchy(Verb& verb) {
      // Execute in owners directly                                     
      if constexpr (SEEK & Seek::Here) {
         Do(verb);
         if (verb.IsDone())
            return true;
      }

      // Execute in parents up to root, if requested                    
      if constexpr (SEEK & Seek::Above) {
         if (mOwner && mOwner->DoInHierarchy<Seek::HereAndAbove>(verb))
            return true;
      }

      // Execute in children, if requested                              
      if constexpr (SEEK & Seek::Below) {
         for (auto child : mChildren) {
            if (child->DoInHierarchy<Seek::HereAndBelow>(verb))
               return true;
         }
      }

      return false;
   }
   
   /// Register unit by all its bases in mUnitsAmbiguous                      
   ///   @param unit - the unit instance to register                          
   ///   @param type - the type to register the unit as                       
   inline void Thing::AddUnitBases(Unit* unit, DMeta type) {
      mUnitsAmbiguous[type] << unit;

      for (auto& base : type->mBases) {
         if (base.mType->template Is<Unit>())
            break;
         AddUnitBases(unit, base.mType);
      }
   }

   /// Unregister unit by all its bases in mUnitsAmbiguous                    
   ///   @param unit - the unit instance to unregister                        
   ///   @param type - the type to unregister the unit as                     
   inline void Thing::RemoveUnitBases(Unit* unit, DMeta type) {
      const auto found = mUnitsAmbiguous.Find(type);
      if (found) {
         auto& set = mUnitsAmbiguous.GetValue(found);
         if (set.Remove(unit) && !set)
            mUnitsAmbiguous.RemoveIndex(found);
      }

      for (auto& base : type->mBases) {
         if (base.mType->template Is<Unit>())
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
      const auto found = mUnitsAmbiguous.Find(meta);
      if (found && mUnitsAmbiguous.GetValue(found).Find(unit))
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
      const auto foundType = mUnitsAmbiguous.Find(meta);
      if (!foundType)
         return 0;

      auto& unitSet = mUnitsAmbiguous.GetValue(foundType);
      const auto unitIndex = unitSet.Find(unit);
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
         if (!found)
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
      if constexpr (!CT::Same<T, Unit>) {
         return static_cast<Decay<T>*>(
            GetUnitMeta(MetaOf<Decay<T>>(), offset)
         );
      }
      else {
         return static_cast<Decay<T>*>(
            GetUnitMeta(nullptr, offset)
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
      if constexpr (!CT::Same<T, Unit>) {
         return static_cast<const Decay<T>*>(
            GetUnitMeta(MetaOf<Decay<T>>(), offset)
         );
      }
      else {
         return static_cast<const Decay<T>*>(
            GetUnitMeta(nullptr, offset)
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

   /// Produce any data (including units) from the hierarchy                  
   ///   @tparam SEEK - what part of the hierarchy to use for the creation    
   ///   @param construct - instructions for the creation of the data         
   ///   @return created data                                                 
   template<Seek SEEK>
   Any Thing::CreateData(const Construct& construct) {
      const auto type = construct.GetType();
      if (!type)
         LANGULUS_THROW(Construct, "Invalid construct");

      const auto producer = type->mProducer;

      // Always implicitly attach a parent trait to descriptor, it      
      // will be stripped, when normalizing the descriptor              
      Construct descriptor {construct};
      descriptor << Traits::Parent {this};

      if (producer) {
         // Data has a specific producer, we can narrow the required    
         // contexts for creation a lot                                 
         if (producer->template CastsTo<Unit>()) {
            // Data is producible from a unit                           
            auto producers = GatherUnits<SEEK>(producer);
            if (!producers) {
               LANGULUS_THROW(Construct,
                  "No viable unit producers available");
            }

            // Potential unit producers found, attempt creation         
            Verbs::Create creator {&descriptor};
            if (Flow::DispatchFlat(producers, creator)) {
               // Great success                                         
               return Abandon(creator.GetOutput());
            }
         }
         else if (producer->template CastsTo<Module>()) {
            // Data is producible from a module                         
            auto producers = GetRuntime()->GetModules(producer);
            if (!producers) {
               LANGULUS_THROW(Construct,
                  "No viable module producers available");
            }

            // Potential module producers found, attempt creation       
            Verbs::Create creator {&descriptor};
            if (Flow::DispatchFlat(producers, creator)) {
               // Great success                                         
               return Abandon(creator.GetOutput());
            }
         }
         else if (producer->template CastsTo<Thing>()) {
            // Data is producible from a thing                          
            TODO();
         }
      }
      else if (type->mIsAbstract && !type->mConcrete) {
         // Data doesn't have a specific producer, but it is abstract   
         // so we know that only a module/unit can concretize it        
         // Gather all units in the desired part of the hierarchy       
         auto producers = GatherUnits<Unit, SEEK>();
         if (producers) {
            // Potential unit producers found, attempt creation there   
            Verbs::Create creator {&descriptor};
            if (Flow::DispatchFlat(producers, creator))
               return Abandon(creator.GetOutput());
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
         
         LANGULUS_THROW(Construct, 
            "Requested data is not default- nor descriptor-constructible");
      }

      Logger::Error("Unable to create data: ", construct);
      return {};
   }

   /// Execute verb in all owners                                             
   ///   @tparam SEEK - where in hierarchy to execute                         
   ///   @param verb - the verb to execute                                    
   ///   @return true if succesfully executed                                 
   template<Seek SEEK>
   bool Unit::DoInHierarchy(Verb& verb) {
      if (!mOwners) {
         Logger::Warning("No owners available for executing ", verb);
         return false;
      }

      bool success {};
      for (auto owner : mOwners)
         success |= owner->template DoInHierarchy<SEEK>(verb);
      return success;
   }

} // namespace Langulus::Entity

#undef ENTITY_VERBOSE_ENABLED
#undef ENTITY_VERBOSE_SELF
#undef ENTITY_VERBOSE
#undef ENTITY_CREATION_VERBOSE_SELF
#undef ENTITY_SELECTION_VERBOSE_SELF
