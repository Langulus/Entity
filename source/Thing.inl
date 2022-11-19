///                                                                           
/// Langulus::Entity                                                          
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Thing.hpp"

#define ENTITY_VERBOSE_SELF(a)            //Logger::Verbose() << this << ": "<< a
#define ENTITY_VERBOSE(a)                 //Logger::Append() << a
#define ENTITY_CREATION_VERBOSE_SELF(a)   //Logger::Verbose() << this << ": " <<a
#define ENTITY_SELECTION_VERBOSE_SELF(a)  //Logger::Verbose() << this << ": " <<a

namespace Langulus::Entity
{
   
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
         LANGULUS_ASSERT(owners->GetRuntime() == GetRuntime(), Except::Access,
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
      if (foundType) {
         const auto removed = mUnits.GetValue(foundType).RemoveValue(unit);
         if (removed) {
            // Decouple before unit is destroyed                        
            if constexpr (TWOSIDED)
               unit->mOwners.RemoveValue<false, true>(this);

            // Notify all other units about the environment change      
            mRefreshRequired = true;
            ENTITY_VERBOSE_SELF(unit << " removed");
            return 1;
         }
      }

      return 0;
   }

   /// Remove all units that are derived from the provided type               
   ///   @tparam T - the type of units to remove                              
   ///   @return the number of removed units                                  
   template<CT::Unit T, bool TWOSIDED>
   Count Thing::RemoveUnits() {
      const auto found = mUnits.FindKeyIndex(MetaData::Of<T>());
      if (found) {
         auto& list = mUnits.GetValue(found);
         for (auto unit : list) {
            ENTITY_VERBOSE_SELF(unit << " removed");
            if constexpr (TWOSIDED)
               unit->mOwners.Remove(this);
         }

         const auto removed = list.GetCount();
         mUnits.RemoveIndex(found);
         mRefreshRequired = true;
         return removed;
      }

      return 0;
   }

   /// Gather all units of a specific static type                             
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @return a container filled with the matches                          
   template<SeekStyle SEEK, CT::Unit T>
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
   ///   @tparam SEEK - where to seek for the unit                            
   ///   @param offset - the index of the unit to return                      
   ///   @return the unit if found, or nullptr otherwise                      
   template<CT::Unit T, SeekStyle SEEK>
   const Decay<T>* Thing::SeekUnit(const Index& offset) const {
      return static_cast<const Decay<T>*>(
         SeekUnit<SEEK>(MetaData::Of<Decay<T>>(), offset));
   }

   /// Find a unit by index and static type from the hierarchy                
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
   ///   @return the newly created unit if able to create it                  
   template<CT::Unit T, class... A>
   Decay<T>* Thing::CreateUnit(A&&...arguments) {
      TAny<Unit*> produced;
      auto request = Construct::From<Decay<T>>();
      if constexpr (sizeof...(arguments) > 0)
         (request << ... << Forward<A>(arguments));

      CreateUnitInner(request, produced);
      return produced.As<Decay<T>*>();
   }

   inline const Unit* Thing::GetUnit(DMeta type, const Index& offset) const {
      return const_cast<Thing*>(this)->GetUnit(type, offset);
   }

   template<CT::Unit T>
   Decay<T>* Thing::GetUnit(const Index& offset) {
      return static_cast<Decay<T>*>(
         GetUnit(MetaData::Of<Decay<T>>(), offset));
   }

   template<CT::Unit T>
   const Decay<T>* Thing::GetUnit(const Index& offset) const {
      return static_cast<const Decay<T>*>(
         GetUnit(MetaData::Of<Decay<T>>(), offset));
   }

   #if LANGULUS_FEATURE(MANAGED_REFLECTION)
      inline const Unit* Thing::GetUnit(const Token& token, const Index& offset) const {
         return const_cast<Thing*>(this)->GetUnit(token, offset);
      }

      template<CT::Unit T>
      Decay<T>* Thing::GetUnitAs(const Token& token, const Index& offset) {
         return static_cast<Decay<T>*>(GetUnit(token, offset));
      }
   #endif

   inline bool Thing::GetTrait(TMeta trait, Trait& output, const Index& offset) const {
      return const_cast<Thing*>(this)->GetTrait(trait, output, offset);
   }

   template<SeekStyle SEEK, CT::Data T>
   bool Thing::SeekValue(TMeta trait, T& value) const {
      Trait found;
      if (!SeekTrait<SEEK>(trait, found) || found.IsEmpty())
         return false;

      value = found.AsCast<T>();
      return true;
   }

   template<SeekStyle SEEK, CT::Trait T, CT::Data D>
   bool Thing::SeekValue(D& value) const {
      static const auto tmeta = MetaTrait::Of<T>();
      Trait found;
      if (!SeekTrait<SEEK>(tmeta, found) || found.IsEmpty())
         return false;

      value = found.AsCast<D>();
      return true;
   }

   inline bool Thing::GetTrait(const Trait& trait, Trait& output, const Index& offset) const {
      return const_cast<Thing*>(this)->GetTrait(trait, output, offset);
   }

   /// Get local trait by static type and offset                              
   ///   @tparam TRAIT - the trait to search for                              
   ///   @param offset - the offset of the trait to return                    
   ///   @return the trait or nullptr if none found                           
   template<CT::Trait T>
   Trait* Thing::GetLocalTrait(const Index& offset) {
      return GetLocalTrait(MetaTrait::Of<T>(), offset);
   }

   /// Get local trait by static type and offset                              
   ///   @tparam TRAIT - the trait to search for                              
   ///   @param offset - the offset of the trait to return                    
   ///   @return the trait or nullptr if none found                           
   template<CT::Trait T>
   const Trait* Thing::GetLocalTrait(const Index& offset) const {
      return const_cast<Thing&>(*this).GetLocalTrait<T>(offset);
   }

   /// Get child entity by offset                                             
   ///   @param offset - the offset of the entity to return                   
   ///   @return the child or nullptr if none found                           
   inline const Thing* Thing::GetChild(const Index& offset) const {
      return const_cast<Thing*>(this)->GetChild(offset);
   }

   /// Get child entity by name and offset                                    
   ///   @param name - the name trait to search for                           
   ///   @param offset - the offset of the matching entity to return          
   ///   @return the child or nullptr if none found                           
   inline const Thing* Thing::GetChild(const Token& name, const Index& offset) const {
      return const_cast<Thing*>(this)->GetChild(name, offset);
   }

   /// Find a trait, searching into the hierarchy (const)                     
   ///   @param seek - direction to search at                                 
   ///   @param var - the trait to search for                                 
   ///   @param output - found traits go there                                
   ///   @param offset - the offset to apply                                  
   ///   @return true if anything matching was found                          
   template<SeekStyle SEEK>
   bool Thing::SeekTrait(TMeta var, Trait& output, const Index& offset) const {
      // Seek here if requested                                         
      if constexpr (SEEK & SeekStyle::Here) {
         if (GetTrait(var, output))
            return true;
      }

      // Seek in parents up to root, if requested                       
      if constexpr (SEEK & SeekStyle::Above) {
         if (mOwner && mOwner->SeekTrait<SeekStyle::UpToHere>(var, output, offset))
            return true;
      }

      // Seek children, if requested                                    
      if constexpr (SEEK & SeekStyle::Below) {
         for (auto child : mChildren) {
            if (child->SeekTrait<SeekStyle::DownFromHere>(var, output, offset))
               return true;
         }
      }

      return false;
   }

   /// Find a trait, searching into the hierarchy                             
   ///   @tparam SEEK - direction to search at                                
   ///   @param var - the trait type to search for                            
   ///   @param output - [out] found traits go there                          
   ///   @param offset - the number of the matching trait to use              
   ///   @return true if anything matching was found                          
   template<SeekStyle SEEK>
   bool Thing::SeekTrait(TMeta var, Trait& output, const Index& offset) {
      return const_cast<const Thing*>(this)->SeekTrait<SEEK>(var, output, offset);
   }

   /// Attempt creating data from the hierarchy, invoking creation verbs with 
   /// the provided arguments in each unit/module, in the specified way       
   ///   @tparam T - the type of data we're producing                         
   ///   @tparam SEEK - in what part of the hierarchy are we producing        
   ///   @param arguments - arguments that get sent to T's constructors       
   ///   @return the produced data, if any                                    
   template<CT::Data T, SeekStyle SEEK>
   Any Thing::CreateData(const Any& arguments) {
      if constexpr (CT::Producible<T>) {
         // Data has a specific producer, we can narrow the required    
         // contexts for creation a lot                                 
         using Producer = CT::ProducerOf<T>;

         if constexpr (CT::Unit<Producer>) {
            // Producible inside a unit                                 
            auto producers = GatherUnits<SEEK, Producer>();
            if (!producers.IsEmpty()) {
               auto creator = Verbs::Create {
                  Construct::From<T>(arguments)
               };

               if (Flow::DispatchFlat(producers, creator))
                  return Abandon(creator.GetOutput());
            }
         }
         else if constexpr (CT::Module<Producer>) {
            TODO();
         }
         else if constexpr (CT::Same<Thing, T>) {
            TODO();
         }
      }
      else if constexpr (CT::Abstract<T> && !CT::Concretizable<T>) {
         // Data doesn't have a specific producer, but it is abstract   
         // so we know that only a module/unit can concretize it        
         auto producers = GatherUnits<SEEK>();
         if (!producers.IsEmpty()) {
            auto creator = Verbs::Create {
               Construct::From<T>(arguments)
            };

            if (Flow::DispatchFlat(producers, creator))
               return Abandon(creator.GetOutput());
         }

         //TODO try modules too?
      }
      else {
         // Data is not abstract, and doesn't have a producer, so just  
         // make it statically right here                               
         return T {arguments};
      }

      return {};
   }

#if LANGULUS_FEATURE(MANAGED_REFLECTION)
   template<SeekStyle>
   Any Thing::CreateData(const Token&, const Any&) {
      TODO();
   }
#endif

   template<SeekStyle>
   Any Thing::CreateData(DMeta, const Any&) {
      TODO();
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
