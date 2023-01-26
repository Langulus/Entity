///                                                                           
/// Langulus::Entity                                                          
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Hierarchy.hpp"

namespace Langulus::Entity
{
   
#if LANGULUS_FEATURE(MANAGED_MEMORY)
   /// Collects all units of the given type (by token) inside the hierarchy   
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @param token - the unit to seek for                                  
   ///   @return the gathered units that match the type                       
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   TAny<Unit*> Hierarchy::GatherUnits(const Token& token) {
      return mOwners.template GatherUnits<SEEK>(token);
   }

   /// Collects all units of the given type (by token) inside the hierarchy   
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @param token - the unit to seek for                                  
   ///   @return the gathered units that match the type                       
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   TAny<const Unit*> Hierarchy::GatherUnits(const Token& token) const {
      return mOwners.template GatherUnits<SEEK>(token);
   }
#endif 

   /// Collects all units of the given type inside the hierarchy              
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @param meta - the units to seek for                                  
   ///   @return the gathered units that match the type                       
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   TAny<Unit*> Hierarchy::GatherUnits(DMeta meta) {
      return mOwners.template GatherUnits<SEEK>(meta);
   }

   /// Collects all units of the given type inside the hierarchy              
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @param meta - the units to seek for                                  
   ///   @return the gathered units that match the type                       
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   TAny<const Unit*> Hierarchy::GatherUnits(DMeta meta) const {
      return mOwners.template GatherUnits<SEEK>(meta);
   }

   /// Gather all units of a specific static type                             
   /// Use an abstract type to gather a broader range of units                
   ///   @tparam T - the type of unit we're searching for, use Unit for all   
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @return a container filled with the matches                          
   template<CT::Data T, SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   TAny<Decay<T>*> Hierarchy::GatherUnits() {
      return mOwners.template GatherUnits<T, SEEK>();
   }
   
   /// Gather all units of a specific static type                             
   /// Use an abstract type to gather a broader range of units                
   ///   @tparam T - the type of unit we're searching for, use Unit for all   
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @return a container filled with the matches                          
   template<CT::Data T, SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   TAny<const Decay<T>*> Hierarchy::GatherUnits() const {
      return mOwners.template GatherUnits<T, SEEK>();
   }
   
#if LANGULUS_FEATURE(MANAGED_MEMORY)
   /// Collects all traits of the given type (by token) inside the hierarchy  
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @param token - the trait to seek for                                 
   ///   @return the gathered traits that match the type                      
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   TAny<Trait> Hierarchy::GatherTraits(const Token& token) {
      return mOwners.template GatherTraits<SEEK>(token);
   }

   /// Collects all traits of the given type (by token) inside the hierarchy  
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @param token - the trait to seek for                                 
   ///   @return the gathered traits that match the type                      
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   TAny<Trait> Hierarchy::GatherTraits(const Token& token) const {
      return mOwners.template GatherTraits<SEEK>(token);
   }
#endif 

   /// Collects all traits of the given type inside the hierarchy             
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @param trait - the trait to seek for                                 
   ///   @return the gathered traits that match the type                      
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   TAny<Trait> Hierarchy::GatherTraits(TMeta trait) {
      return mOwners.template GatherTraits<SEEK>(trait);
   }

   /// Collects all traits of the given type inside the hierarchy             
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @param trait - the trait to seek for                                 
   ///   @return the gathered traits that match the type                      
   template<SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   TAny<Trait> Hierarchy::GatherTraits(TMeta trait) const {
      return mOwners.template GatherTraits<SEEK>(trait);
   }

   template<CT::Trait T, SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   TAny<Trait> Hierarchy::GatherTraits() {
      return mOwners.template GatherTraits<T, SEEK>();
   }

   template<CT::Trait T, SeekStyle SEEK>
   LANGULUS(ALWAYSINLINE)
   TAny<Trait> Hierarchy::GatherTraits() const {
      return mOwners.template GatherTraits<T, SEEK>();
   }

   /// Gather all values convertible to a type                                
   ///   @tparam SEEK - where in the hierarchy are we seeking in?             
   ///   @tparam D - type to convert to                                       
   ///   @return the gathered values                                          
   template<SeekStyle SEEK, CT::Data D>
   LANGULUS(ALWAYSINLINE)
   TAny<D> Hierarchy::GatherValues() const {
      return mOwners.template GatherValues<SEEK, D>();
   }

} // namespace Langulus::Entity