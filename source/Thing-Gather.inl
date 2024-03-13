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
#include "Hierarchy-Gather.inl"

#if 0
   #define ENTITY_VERBOSE_ENABLED() 1
   #define ENTITY_VERBOSE_SELF(...)    Logger::Verbose(Self(), __VA_ARGS__)
   #define ENTITY_VERBOSE(...)         Logger::Append(__VA_ARGS__)
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
   template<Seek SEEK>
   TAny<A::Unit*> Thing::GatherUnits(DMeta meta) {
      TAny<A::Unit*> result;

      if constexpr (SEEK & Seek::Here) {
         // Seek here if requested                                      
         const auto found = mUnitsAmbiguous.Find(meta);
         if (found) {
            auto& list = mUnitsAmbiguous.GetValue(found);
            for (auto& unit : list)
               result << unit.Get();
         }
      }

      if constexpr (SEEK & Seek::Above) {
         // Seek in parents up to root, if requested                    
         if (mOwner) {
            auto inParents = mOwner->template
               GatherUnits<Seek::HereAndAbove>(meta);
            result += Abandon(inParents);
         }
      }

      if constexpr (SEEK & Seek::Below) {
         // Seek children, if requested                                 
         for (auto& child : mChildren) {
            auto inChildren = child->template 
               GatherUnits<Seek::HereAndBelow>(meta);
            result += Abandon(inChildren);
         }
      }

      return result;
   }
      
   /// Collects all traits of the given type inside the hierarchy             
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @param trait - the trait to seek for                                 
   ///   @return the gathered traits that match the type                      
   template<Seek SEEK>
   TAny<Trait> Thing::GatherTraits(TMeta trait) {
      TAny<Trait> results;

      if constexpr (SEEK & Seek::Here) {
         // Handle some predefined traits here                          
         if (trait->template Is<Traits::Unit>()) {
            // Gather all units                                         
            for (auto& unit : mUnitsList)
               results << Traits::Unit {unit};
         }
         else if (trait->template Is<Traits::Child>()) {
            // Gather all children                                      
            for (auto& child : mChildren)
               results << Traits::Unit {child};
         }
         else if (trait->template Is<Traits::Runtime>()) {
            // Get the nearest runtime                                  
            results << Traits::Runtime {*mRuntime};
         }
         else if (trait->template Is<Traits::Parent>()) {
            // Get the parent                                           
            results << Traits::Parent {*mOwner};
         }

         // Check dynamic traits in the entity                          
         const auto found = mTraits.Find(trait);
         if (found)
            results += mTraits.GetValue(found);

         // Then check each unit's static traits                        
         for (auto& unit : mUnitsList) {
            Offset index {};
            auto t = unit->GetMember(trait, index);
            while (t) {
               results <<= Trait::From(trait, t);
               t = unit->GetMember(trait, ++index);
            }
         }
      }

      if constexpr (SEEK & Seek::Above) {
         // Seek in parents up to root, if requested                    
         if (mOwner) {
            results += mOwner->template
               GatherTraits<Seek::HereAndAbove>(trait);
         }
      }

      if constexpr (SEEK & Seek::Below) {
         // Seek children, if requested                                 
         for (auto& child : mChildren) {
            results += child->template
               GatherTraits<Seek::HereAndBelow>(trait);
         }
      }

      return Abandon(results);
   }

   /// Gather all traits/members convertible to a type                        
   ///   @tparam D - type to convert to                                       
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @return the gathered values                                          
   template<CT::Data D, Seek SEEK>
   TAny<D> Thing::GatherValues() const {
      TAny<D> results;

      if constexpr (SEEK & Seek::Here) {
         // Check dynamic traits in the entity                          
         for (auto traitGroup : mTraits) {
            for (auto trait : traitGroup.mValue) {
               try { results << trait.template AsCast<D>(); }
               catch (...) {}
            }
         }

         // Then check each unit's static traits                        
         for (auto& unit : mUnitsList) {
            Offset index {};
            auto t = unit->GetMember(TMeta {}, index);
            while (t) {
               try { results << t.template AsCast<D>(); }
               catch (...) {}

               t = unit->GetMember(TMeta {}, ++index);
            }
         }

         // Then check the Thing's members                              
         Offset index {};
         auto t = GetMember(TMeta {}, index);
         while (t) {
            try { results << t.template AsCast<D>(); }
            catch (...) {}

            t = GetMember(TMeta {}, ++index);
         }
      }

      if constexpr (SEEK & Seek::Above) {
         // Seek in parents up to root, if requested                    
         if (mOwner) {
            results += mOwner->template
               GatherValues<Seek::HereAndAbove, D>();
         }
      }

      if constexpr (SEEK & Seek::Below) {
         // Seek children, if requested                                 
         for (auto& child : mChildren) {
            results += child->template
               GatherValues<Seek::HereAndBelow, D>();
         }
      }

      return Abandon(results);
   }

} // namespace Langulus::Entity

#undef ENTITY_VERBOSE_ENABLED
#undef ENTITY_VERBOSE_SELF
#undef ENTITY_VERBOSE
