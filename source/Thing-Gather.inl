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
            auto inParents =
               mOwner->GatherUnits<SeekStyle::HereAndAbove>(meta);
            result += Abandon(inParents);
         }
      }

      if constexpr (SEEK & SeekStyle::Below) {
         // Seek children, if requested                                 
         for (auto child : mChildren) {
            auto inChildren =
               child->GatherUnits<SeekStyle::HereAndBelow>(meta);
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

   /// Gather all units of a specific static type                             
   /// Use an abstract type to gather a broader range of units                
   ///   @tparam T - the type of unit we're searching for, use Unit for all   
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @return a container filled with the matches                          
   template<CT::Unit T, SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   TAny<Decay<T>*> Thing::GatherUnits() {
      return GatherUnits<SEEK>(MetaData::Of<Decay<T>>());
   }
   
   /// Gather all units of a specific static type                             
   /// Use an abstract type to gather a broader range of units                
   ///   @tparam T - the type of unit we're searching for, use Unit for all   
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @return a container filled with the matches                          
   template<CT::Unit T, SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   TAny<const Decay<T>*> Thing::GatherUnits() const {
      return GatherUnits<SEEK>(MetaData::Of<Decay<T>>());
   }

} // namespace Langulus::Entity

#undef ENTITY_VERBOSE_ENABLED
#undef ENTITY_VERBOSE_SELF
#undef ENTITY_VERBOSE
