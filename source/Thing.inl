///                                                                           
/// Langulus::Entity                                                          
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Thing.hpp"

#if 0
   #define ENTITY_VERBOSE_ENABLED() 1
   #define ENTITY_VERBOSE_SELF(a)            Logger::Verbose() << Self() << a
   #define ENTITY_VERBOSE(a)                 Logger::Append() << a
   #define ENTITY_CREATION_VERBOSE_SELF(a)   Logger::Verbose() << Self() << a
   #define ENTITY_SELECTION_VERBOSE_SELF(a)  Logger::Verbose() << Self() << a
#else
   #define ENTITY_VERBOSE_ENABLED() 0
   #define ENTITY_VERBOSE_SELF(a)
   #define ENTITY_VERBOSE(a)
   #define ENTITY_CREATION_VERBOSE_SELF(a)
   #define ENTITY_SELECTION_VERBOSE_SELF(a)
#endif

namespace Langulus::Entity
{
   
   /// Remove a child                                                         
   ///   @attention assumes entity is a valid pointer                         
   ///   @tparam TWOSIDED - true to also set the entity's owner               
   ///   @param entity - entity instance to add as child                      
   ///   @return the number of added children                                 
   template<bool TWOSIDED>
   Count Thing::AddChild(Thing* entity) {
      LANGULUS_ASSUME(DevAssumes, nullptr != entity, "Bad entity pointer");
      const auto added = mChildren.Merge(entity);

      if constexpr (TWOSIDED) {
         if (added && entity->mOwner != this) {
            if (entity->mOwner)
               entity->mOwner->RemoveChild<false>(entity);
            entity->mOwner = this;
            entity->mRefreshRequired = true;
         }
      }

      #if ENTITY_VERBOSE_ENABLED()
         if (added)
            ENTITY_VERBOSE_SELF(entity << " added");
      #endif

      return added;
   }
      
   /// Destroy a child that matched pointer                                   
   ///   @attention assumes entity is a valid pointer                         
   ///   @attention assumes you have ownership of that entity outside call    
   ///              otherwise you risk deallocation of the memory behind it,  
   ///              if managed memory is enabled, and we have jurisdiction    
   ///   @tparam TWOSIDED - true to also remove entity's owner                
   ///   @param entity - entity instance to remove from children              
   ///   @return the number of removed children                               
   template<bool TWOSIDED>
   Count Thing::RemoveChild(Thing* entity) {
      LANGULUS_ASSUME(DevAssumes, nullptr != entity, "Bad entity pointer");
      #if LANGULUS_FEATURE(MANAGED_MEMORY)
         LANGULUS_ASSUME(DevAssumes, 
            !Anyness::Inner::Allocator::CheckAuthority(MetaOf<Thing>(), entity) ||
             Anyness::Inner::Allocator::Find(MetaOf<Thing>(), entity)->GetUses() > 1,
            "Managed entity pointer has no ownership outside this function, "
            "and will result in a bad pointer after removed from children"
         );
      #endif
      
      const auto removed = mChildren.RemoveValue<false, true>(entity);
      if constexpr (TWOSIDED) {
         if (removed && entity->mOwner == this) {
            entity->mOwner = nullptr;
            entity->mRefreshRequired = true;
         }
      }

      #if ENTITY_VERBOSE_ENABLED()
         if (removed)
            ENTITY_VERBOSE_SELF(entity << " removed");
      #endif

      return removed;
   }

   /// Execute verb in the hierarchy                                          
   ///   @tparam SEEK - where in the hierarchy to execute                     
   ///   @param verb - the verb to execute                                    
   ///   @return true if succesfully executed                                 
   template<SeekStyle SEEK>
   bool Thing::DoInHierarchy(Verb& verb) {
      // Execute in owners directly                                     
      if constexpr (SEEK & SeekStyle::Here) {
         Do(verb);
         if (verb.IsDone())
            return true;
      }

      // Execute in parents up to root, if requested                    
      if constexpr (SEEK & SeekStyle::Above) {
         if (mOwner && mOwner->DoInHierarchy<SeekStyle::UpToHere>(verb))
            return true;
      }

      // Execute in children, if requested                              
      if constexpr (SEEK & SeekStyle::Below) {
         for (auto child : mChildren) {
            if (child->DoInHierarchy<SeekStyle::DownFromHere>(verb))
               return true;
         }
      }

      return false;
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
      const auto foundType = mUnits.FindKeyIndex(unit->GetType());
      if (foundType && mUnits.GetValue(foundType).Find(unit))
         return 0;

      // We must guarantee, that no unit is coupled to entities with    
      // different runtimes!                                            
      for (auto owners : unit->mOwners) {
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

      mUnits[unit->GetType()] << unit;
      mRefreshRequired = true;
      ENTITY_VERBOSE(unit << " added");
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
      const auto foundType = mUnits.FindKeyIndex(unit->GetType());
      if (!foundType)
         return 0;

      const auto removed = mUnits.GetValue(foundType).RemoveValue(unit);
      if (removed) {
         // Decouple before unit is destroyed                           
         if constexpr (TWOSIDED)
            unit->mOwners.template RemoveValue<false, true>(this);

         // Notify all other units about the environment change         
         mRefreshRequired = true;
         ENTITY_VERBOSE_SELF(unit << " removed");
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
      const auto found = mUnits.FindKeyIndex(MetaData::Of<T>());
      if (!found)
         return 0;

      auto& list = mUnits.GetValue(found);
      for (auto unit : list) {
         ENTITY_VERBOSE_SELF(unit << " removed");
         if constexpr (TWOSIDED)
            unit->mOwners.template RemoveValue<false, true>(this);
      }

      const auto removed = list.GetCount();
      mUnits.RemoveIndex(found);
      mRefreshRequired = true;
      return removed;
   }

   /// Count the number of matching units in this entity                      
   ///   @tparam T - the type of units to seach for, use Unit for all         
   ///   @return the number of matching units                                 
   template<CT::Unit T>
   Count Thing::HasUnits() const {
      return HasUnits(MetaData::Of<Decay<T>>());
   }

   /// Gather all units of a specific static type                             
   /// Use an abstract type to gather a broader range of units                
   ///   @tparam T - the type of unit we're searching for, use Unit for all   
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @return a container filled with the matches                          
   template<CT::Unit T, SeekStyle SEEK>
   TAny<const Decay<T>*> Thing::GatherUnits() const {
      return GatherUnits<SEEK>(MetaData::Of<Decay<T>>());
   }
   
   /// Collects all units of the given type inside the hierarchy              
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @param meta - the units to seek for                                  
   ///   @return the gathered units that match the type                       
   template<SeekStyle SEEK>
   TAny<const Unit*> Thing::GatherUnits(DMeta meta) const {
      TAny<const Unit*> result;
      if constexpr (SEEK & SeekStyle::Here) {
         // Seek here if requested                                      
         for (auto unitList : mUnits) {
            for (auto unit : unitList.mValue) {
               if (unit->CastsTo(meta))
                  result << unit;
            }
         }
      }

      if constexpr (SEEK & SeekStyle::Above) {
         // Seek in parents up to root, if requested                    
         if (mOwner) {
            auto inParents = mOwner->GatherUnits<SeekStyle::UpToHere>(meta);
            result.SmartPush(Abandon(inParents));
         }
      }

      if constexpr (SEEK & SeekStyle::Below) {
         // Seek children, if requested                                 
         for (auto child : mChildren) {
            auto inChildren = child->GatherUnits<SeekStyle::DownFromHere>(meta);
            result.SmartPush(Abandon(inChildren));
         }
      }

      return result;
   }

   /// Find a specific unit, searching into the hierarchy                     
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @param meta - the unit to seek for                                   
   ///   @param offset - which of the matches to return                       
   ///   @return the found unit, or nullptr if no such unit was found         
   template<SeekStyle SEEK>
   Unit* Thing::SeekUnit(DMeta meta, const Index& offset) {
      Unit* result = nullptr;
      if constexpr (SEEK & SeekStyle::Here) {
         // Seek here if requested                                      
         result = GetUnit(meta, offset);
         if (result)
            return result;
      }

      if constexpr (SEEK & SeekStyle::Above) {
         // Seek in parents up to root, if requested                    
         if (mOwner) {
            result = mOwner->SeekUnit<SeekStyle::UpToHere>(meta, offset);
            if (result)
               return result;
         }
      }

      if constexpr (SEEK & SeekStyle::Below) {
         // Seek children, if requested                                 
         for (auto child : mChildren) {
            result = child->SeekUnit<SeekStyle::DownFromHere>(meta, offset);
            if (result)
               return result;
         }
      }

      return nullptr;
   }

   /// Find a unit by type and index from the hierarchy                       
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param unit - the type of the unit to seek for                       
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<SeekStyle SEEK>
   const Unit* Thing::SeekUnit(DMeta unit, const Index& offset) const {
      return const_cast<Thing*>(this)->SeekUnit<SEEK>(unit, offset);
   }

   /// Find a unit by index and static type from the hierarchy (const)        
   ///   @tparam T - static unit type to search for                           
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<CT::Unit T, SeekStyle SEEK>
   const Decay<T>* Thing::SeekUnit(const Index& offset) const {
      return static_cast<const Decay<T>*>(
         SeekUnit<SEEK>(MetaData::Of<Decay<T>>(), offset));
   }

   /// Find a unit by index and static type from the hierarchy                
   ///   @tparam T - static unit type to search for                           
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<CT::Unit T, SeekStyle SEEK>
   Decay<T>* Thing::SeekUnit(const Index& offset) {
      return static_cast<Decay<T>*>(
         SeekUnit<SEEK>(MetaData::Of<Decay<T>>(), offset));
   }

   /// Create a unit by static type and arguments, relying on producers       
   /// in the hierarchy                                                       
   ///   @tparam T - the unit to create                                       
   ///   @tparam A... - arguments for the unit's creation                     
   ///   @param arguments... - the arguments to provide for construct         
   ///   @return the created unit(s)                                          
   template<CT::Unit T, class... A>
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
   Decay<T>* Thing::GetUnit(const Index& offset) {
      return static_cast<Decay<T>*>(
         GetUnit(MetaData::Of<Decay<T>>(), offset));
   }

   /// Get a unit by a static type and an optional offset (const)             
   ///   @tparam T - the type of unit we're searching for                     
   ///   @param offset - optional offset (Nth match)                          
   ///   @return the unit if found, or nullptr if not                         
   template<CT::Unit T>
   const Decay<T>* Thing::GetUnit(const Index& offset) const {
      return static_cast<const Decay<T>*>(
         GetUnit(MetaData::Of<Decay<T>>(), offset));
   }

   #if LANGULUS_FEATURE(MANAGED_REFLECTION)
      /// Get a unit by a token, and an optional offset (const)               
      /// This is available only if managed reflection feature is enabled     
      ///   @param token - unit type token                                    
      ///   @param offset - optional offset (Nth match)                       
      ///   @return the unit if found, or nullptr if not                      
      inline const Unit* Thing::GetUnit(const Token& token, const Index& offset) const {
         return const_cast<Thing*>(this)->GetUnit(token, offset);
      }

      /// Get a unit by a token, and an optional offset, then dynamic_cast it 
      /// This is available only if managed reflection feature is enabled     
      ///   @tparam T - the type of unit we're casting to                     
      ///   @param token - unit type token                                    
      ///   @param offset - optional offset (Nth match)                       
      ///   @return the unit if found, or nullptr if not                      
      template<CT::Unit T>
      Decay<T>* Thing::GetUnitAs(const Token& token, const Index& offset) {
         return dynamic_cast<Decay<T>*>(GetUnit(token, offset));
      }
   #endif

   /// Seek a value from static/dynamic/unit traits in the hierarchy          
   ///   @tparam SEEK - the direction we're searching the hierarchy in        
   ///   @tparam T - the type of value we're searching for (deducible)        
   ///   @param trait - the type of trait we're searching for                 
   ///   @param value - [out] the found values will be written here           
   ///   @return true if value has been found and rewritten                   
   template<SeekStyle SEEK, CT::Data D>
   bool Thing::SeekValue(TMeta trait, D& value) const {
      const auto found = SeekTrait<SEEK>(trait);
      if (found.IsEmpty())
         return false;

      value = found.template AsCast<D>();
      return true;
   }

   /// Seek a value from static/dynamic/unit traits in the hierarchy          
   ///   @tparam T - the type of trait we're searching for                    
   ///   @tparam SEEK - the direction we're searching the hierarchy in        
   ///   @tparam D - the type of value we're searching for (deducible)        
   ///   @param value - [out] the found values will be written here           
   ///   @return true if value has been found and rewritten                   
   template<CT::Trait T, SeekStyle SEEK, CT::Data D>
   bool Thing::SeekValue(D& value) const {
      const auto found = SeekTrait<T, SEEK>();
      if (found.IsEmpty())
         return false;

      value = found.template AsCast<D>();
      return true;
   }

   template<CT::Trait T>
   Trait Thing::GetTrait(const Index& offset) {
      return GetTrait(MetaTrait::Of<T>(), offset);
   }

   /// Get local trait by static type and offset                              
   ///   @tparam T - the trait to search for                                  
   ///   @param offset - the offset of the trait to return (optional)         
   ///   @return the trait or nullptr if none found                           
   template<CT::Trait T>
   Trait* Thing::GetLocalTrait(const Index& offset) {
      return GetLocalTrait(MetaTrait::Of<T>(), offset);
   }

   /// Get local trait by static type and offset                              
   ///   @tparam T - the trait to search for                                  
   ///   @param offset - the offset of the trait to return                    
   ///   @return the trait or nullptr if none found                           
   template<CT::Trait T>
   const Trait* Thing::GetLocalTrait(const Index& offset) const {
      return const_cast<Thing&>(*this).template GetLocalTrait<T>(offset);
   }

   /// Find a trait, searching into the hierarchy                             
   ///   @tparam T - the trait to search for                                  
   ///   @tparam SEEK - direction to search at                                
   ///   @param offset - the number of the matching trait to use              
   ///   @return the trait, which is not empty, if trait was found            
   template<CT::Trait T, SeekStyle SEEK>
   Trait Thing::SeekTrait(const Index& offset) {
      return SeekTrait<SEEK>(MetaTrait::Of<T>(), offset);
   }

   /// Find a trait, searching into the hierarchy (const)                     
   ///   @tparam T - the trait to search for                                  
   ///   @tparam SEEK - direction to search at                                
   ///   @param offset - the number of the matching trait to use              
   ///   @return the trait, which is not empty, if trait was found            
   template<CT::Trait T, SeekStyle SEEK>
   Trait Thing::SeekTrait(const Index& offset) const {
      return SeekTrait<SEEK>(MetaTrait::Of<T>(), offset);
   }

   /// Find a trait, searching into the hierarchy (const)                     
   ///   @tparam SEEK - direction to search at                                
   ///   @param var - the trait to search for                                 
   ///   @param offset - the offset to apply                                  
   ///   @return the trait, which is not empty, if trait was found            
   template<SeekStyle SEEK>
   Trait Thing::SeekTrait(TMeta var, const Index& offset) const {
      if constexpr (SEEK & SeekStyle::Here) {
         // Seek here if requested                                      
         auto output = GetTrait(var);
         if (!output.IsEmpty())
            return Abandon(output);
      }

      if constexpr (SEEK & SeekStyle::Above) {
         // Seek in parents up to root, if requested                    
         if (mOwner) {
            auto output = mOwner->SeekTrait<SeekStyle::UpToHere>(var, offset);
            if (!output.IsEmpty())
               return Abandon(output);
         }
      }

      if constexpr (SEEK & SeekStyle::Below) {
         // Seek children, if requested                                 
         for (auto child : mChildren) {
            auto output = child->SeekTrait<SeekStyle::DownFromHere>(var, offset);
            if (!output.IsEmpty())
               return Abandon(output);
         }
      }

      return {};
   }

   /// Find a trait, searching into the hierarchy                             
   ///   @tparam SEEK - direction to search at                                
   ///   @param var - the trait type to search for                            
   ///   @param offset - the number of the matching trait to use              
   ///   @return the trait, which is not empty, if trait was found            
   template<SeekStyle SEEK>
   Trait Thing::SeekTrait(TMeta var, const Index& offset) {
      return const_cast<const Thing*>(this)
         ->template SeekTrait<SEEK>(var, offset);
   }

   /// Produce any data (including units) from the hierarchy                  
   ///   @tparam SEEK - what part of the hierarchy to use for the creation    
   ///   @param construct - instructions for the creation of the data         
   ///   @return created data                                                 
   template<SeekStyle SEEK>
   Any Thing::CreateData(const Construct& construct) {
      const auto type = construct.GetType();
      const auto producer = type->mProducer;

      // Always implicitly attach a parent trait to descriptor          
      Construct descriptor {construct};
      descriptor << Traits::Parent {this};

      if (producer) {
         // Data has a specific producer, we can narrow the required    
         // contexts for creation a lot                                 
         if (producer->template CastsTo<Unit>()) {
            // Data is producible from a unit                           
            auto producers = GatherUnits<SEEK>(producer);
            if (!producers.IsEmpty()) {
               // Potential unit producers found, attempt creation      
               auto creator = Verbs::Create {&descriptor};
               if (Flow::DispatchFlat(producers, creator)) {
                  // Great success                                      
                  return Abandon(creator.GetOutput());
               }
            }
         }
         else if (producer->template CastsTo<Module>()) {
            // Data is producible from a module                         
            auto producers = GetRuntime()->GetModules(producer);
            if (!producers.IsEmpty()) {
               // Potential module producers found, attempt creation    
               auto creator = Verbs::Create {&descriptor};
               if (Flow::DispatchFlat(producers, creator)) {
                  // Great success                                      
                  return Abandon(creator.GetOutput());
               }
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
         if (!producers.IsEmpty()) {
            // Potential unit producers found, attempt creation there   
            auto creator = Verbs::Create {&descriptor};
            if (Flow::DispatchFlat(producers, creator))
               return Abandon(creator.GetOutput());
         }

         //TODO try modules too?
         TODO();
      }
      else {
         // Data is not abstract, nor has a producer, so just make it   
         // right here if possible, passing the descriptor over, if     
         // such constructor is reflected                               
         // If it's a unit, its descriptor is resposible for            
         // registering it with the parent                              
         if (type->mDescriptorConstructor) {
            auto result = Any::FromMeta(type);
            result.Emplace(descriptor.GetArgument());
            return Abandon(result);
         }
         else if (type->mDefaultConstructor) {
            auto result = Any::FromMeta(type);
            result.Emplace();
            return Abandon(result);
         }
         else LANGULUS_THROW(Construct, "Requested data is not default- nor descriptor-constructible");
      }

      return {};
   }







   /// Execute verb in all owners                                             
   ///   @param verb - the verb to execute in all owners                      
   ///   @param seek - where in the hierarchy to execute                      
   ///   @return true if succesfully executed                                 
   template<SeekStyle SEEK>
   bool Unit::DoInHierarchy(Verb& verb) {
      if (mOwners.IsEmpty()) {
         Logger::Warning() << "No owners available for executing " << verb;
         return false;
      }

      bool success {};
      for (auto owner : mOwners)
         success |= owner->DoInHierarchy<SEEK>(verb);
      return success;
   }

   /// Get a trait by scanning owners and other units                         
   ///   @return the trait                                                    
   template<CT::Trait T, SeekStyle SEEK>
   Trait Unit::SeekTrait() const {
      Trait output;
      static const auto data = MetaTrait::Of<T>();

      bool satisfied {};
      for (auto context : mOwners) {
         const bool found = context->SeekTrait<SEEK>(data, output);
         if (found && satisfied) {
            Logger::Warning() << "Multiple " << data << " traits found in hierarchy";
            Logger::Warning() << "Each sequential trait will overwrite the result";
         }

         satisfied |= found;
      }

      return output;
   }

   /// Get a value from inside a trait by scanning owners and other units     
   ///   @param trait - the trait to seek for                                 
   ///   @param value - [out] value to set if trait was found                 
   ///   @return true if anything was written to value                        
   template<SeekStyle SEEK, CT::Data D>
   bool Unit::SeekValue(TMeta trait, D& value) const {
      bool satisfied {};
      for (auto context : mOwners) {
         bool found = context->SeekValue<SEEK>(trait, value);
         if (found && satisfied) {
            Logger::Warning() << "Multiple " << trait << " traits found in hierarchy";
            Logger::Warning() << "Each sequential trait will overwrite the result";
         }

         satisfied |= found;
      }

      return satisfied;
   }

} // namespace Langulus::Entity

#undef ENTITY_VERBOSE_SELF
#undef ENTITY_VERBOSE
#undef ENTITY_CREATION_VERBOSE_SELF
#undef ENTITY_SELECTION_VERBOSE_SELF
