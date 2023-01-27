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
   #define ENTITY_VERBOSE_SELF(...)            Logger::Verbose(Self(), __VA_ARGS__)
   #define ENTITY_VERBOSE(...)                 Logger::Append(__VA_ARGS__)
#else
   #define ENTITY_VERBOSE_ENABLED() 0
   #define ENTITY_VERBOSE_SELF(...)
   #define ENTITY_VERBOSE(...)
#endif

namespace Langulus::Entity
{
   
   /// Collects all units of the given type inside the hierarchy              
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @param meta - the units to seek for                                  
   ///   @return the gathered units that match the type                       
   template<SeekStyle SEEK>
   TAny<Unit*> Thing::GatherUnits(DMeta meta) {
      TAny<Unit*> result;
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
            auto inParents = mOwner->template
               GatherUnits<SeekStyle::HereAndAbove>(meta);
            result += Abandon(inParents);
         }
      }

      if constexpr (SEEK & SeekStyle::Below) {
         // Seek children, if requested                                 
         for (auto child : mChildren) {
            auto inChildren = child->template 
               GatherUnits<SeekStyle::HereAndBelow>(meta);
            result += Abandon(inChildren);
         }
      }

      return result;
   }

   /// Collects all units of the given type inside the hierarchy              
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @param meta - the units to seek for                                  
   ///   @return the gathered units that match the type                       
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   TAny<const Unit*> Thing::GatherUnits(DMeta meta) const {
      return const_cast<Thing*>(this)->template GatherUnits<SEEK>(meta);
   }
      
   /// Collects all traits of the given type inside the hierarchy             
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @param trait - the trait to seek for                                 
   ///   @return the gathered traits that match the type                      
   template<SeekStyle SEEK>
   TAny<Trait> Thing::GatherTraits(TMeta trait) {
      TAny<Trait> results;

      if constexpr (SEEK & SeekStyle::Here) {
         // Handle some predefined traits here                          
         if (trait->template Is<Traits::Unit>()) {
            for (auto unit : mUnits)
               results << Traits::Unit {unit.mValue};
         }
         else if (trait->template Is<Traits::Child>()) {
            for (auto child : mChildren)
               results << Traits::Unit {child};
         }
         else if (trait->template Is<Traits::Runtime>()) {
            // Get the nearest runtime                                  
            results << Traits::Runtime {mRuntime.Get()};
         }
         else if (trait->template Is<Traits::Parent>()) {
            // Get the parent                                           
            results << Traits::Parent {mOwner.Get()};
         }

         // Check dynamic traits in the entity                          
         const auto found = mTraits.FindKeyIndex(trait);
         if (found)
            results += mTraits.GetValue(found);

         // Then check each unit's static traits                        
         mUnits.ForEachValue([&](Unit* unit) {
            Offset index {};
            auto t = unit->GetMember(trait, index);
            while (!t.IsEmpty()) {
               results <<= Trait::From(trait, t);
               t = unit->GetMember(trait, ++index);
            }
         });

         // Then check the Thing's members                              
         //TODO isn't this redundant and slower than the code in the beginning of this function?
         Offset index {};
         auto t = GetMember(trait, index);
         while (!t.IsEmpty()) {
            results <<= Trait::From(trait, t);
            t = GetMember(trait, ++index);
         }
      }

      if constexpr (SEEK & SeekStyle::Above) {
         // Seek in parents up to root, if requested                    
         if (mOwner) {
            results += mOwner->template
               GatherTraits<SeekStyle::HereAndAbove>(trait);
         }
      }

      if constexpr (SEEK & SeekStyle::Below) {
         // Seek children, if requested                                 
         for (auto child : mChildren) {
            results += mOwner->template
               GatherTraits<SeekStyle::HereAndBelow>(trait);
         }
      }

      return Abandon(results);
   }

   /// Collects all traits of the given type inside the hierarchy             
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @param trait - the trait to seek for                                 
   ///   @return the gathered traits that match the type                      
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   TAny<Trait> Thing::GatherTraits(TMeta trait) const {
      return const_cast<Thing*>(this)->template GatherTraits<SEEK>(trait);
   }

   /// Gather all values convertible to a type                                
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @tparam D - type to convert to                                       
   ///   @return the gathered values                                          
   template<SeekStyle SEEK, CT::Data D>
   TAny<D> Thing::GatherValues() const {
      TAny<D> results;

      if constexpr (SEEK & SeekStyle::Here) {
         // Check dynamic traits in the entity                          
         for (auto traitGroup : mTraits) {
            for (auto trait : traitGroup.mValue) {
               try { results << trait.template AsCast<D>(); }
               catch (...) {}
            }
         }

         // Then check each unit's static traits                        
         mUnits.ForEachValue([&](Unit* unit) {
            Offset index {};
            auto t = unit->GetMember(nullptr, index);
            while (!t.IsEmpty()) {
               try { results <<= t.template AsCast<D>(); }
               catch (...) {}

               t = unit->GetMember(nullptr, ++index);
            }
         });

         // Then check the Thing's members                              
         Offset index {};
         auto t = GetMember(nullptr, index);
         while (!t.IsEmpty()) {
            try { results <<= t.template AsCast<D>(); }
            catch (...) {}

            t = GetMember(nullptr, ++index);
         }
      }

      if constexpr (SEEK & SeekStyle::Above) {
         // Seek in parents up to root, if requested                    
         if (mOwner) {
            results += mOwner->template
               GatherValues<SeekStyle::HereAndAbove, D>();
         }
      }

      if constexpr (SEEK & SeekStyle::Below) {
         // Seek children, if requested                                 
         for (auto child : mChildren) {
            results += mOwner->template
               GatherValues<SeekStyle::HereAndBelow, D>();
         }
      }

      return Abandon(results);
   }

} // namespace Langulus::Entity

#undef ENTITY_VERBOSE_ENABLED
#undef ENTITY_VERBOSE_SELF
#undef ENTITY_VERBOSE
