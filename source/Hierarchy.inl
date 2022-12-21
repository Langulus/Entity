///                                                                           
/// Langulus::Entity                                                          
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Hierarchy.hpp"
#include "Unit.hpp"

namespace Langulus::Entity
{

   /// Copy-construction                                                      
   template<class T>
   Pinnable<T>::Pinnable(const T& value) requires (CT::CopyMakable<T>)
      : T {value} { }

   /// Move-construction                                                      
   template<class T>
   Pinnable<T>::Pinnable(T&& value) noexcept requires (CT::MoveMakable<T>)
      : T {Forward<T>(value)} { }

   /// Copy-assignment                                                        
   template<class T>
   Pinnable<T>& Pinnable<T>::operator = (const T& rhs) requires (CT::Copyable<T>) {
      T::operator = (rhs);
      return *this;
   }

   /// Move-assignment                                                        
   template<class T>
   Pinnable<T>& Pinnable<T>::operator = (T&& rhs) noexcept requires (CT::Movable<T>) {
      T::operator = (Forward<T>(rhs));
      return *this;
   }

   /// Check if the value is pinned                                           
   template<class T>
   bool Pinnable<T>::IsPinned() const noexcept {
      return mLocked;
   }

   /// Pin the value, it will no longer be overwritten by environment         
   template<class T>
   void Pinnable<T>::Pin() noexcept {
      mLocked = true;
   }

   /// Unpin the value, it will no longer be overwritten by environment       
   template<class T>
   void Pinnable<T>::Unpin() noexcept {
      mLocked = false;
   }


   /// Seek a trait inside descriptor first, and then the hierarchy           
   /// If D is a pinnable type and trait was found in descriptor, value will  
   /// be pinned, and any subsequent seeks will no longer affect this value   
   ///   @tparam T - the type of trait to search for                          
   ///   @tparam SEEK - the direction we're seeking in the hierarchy          
   ///   @tparam D - type of data we're filling in (deducible)                
   ///   @param descriptor - descriptor to search through                     
   ///   @param value - the value to fill in                                  
   ///   @return true if value changed                                        
   template<CT::Trait T, SeekStyle SEEK, CT::Data D>
   bool Hierarchy::SeekTrait(const Any& descriptor, D& value) const {
      if constexpr (CT::Pinnable<D>) {
         // Never touch pinned values                                   
         if (value.mLocked)
            return false;
      }

      // Scan descriptor                                                
      bool done = false;
      descriptor.ForEachDeep([&done,&value](const Trait& trait) {
         if (trait.template TraitIs<T>()) {
            // Found match                                              
            try {
               if constexpr (CT::Pinnable<D>)
                  value = trait.template AsCast<TypeOf<D>>();
               else
                  value = trait.template AsCast<D>();
               done = true;
               return false;
            }
            catch(...) {
               // Even though trait matches, data is not convertible    
            }
         }

         // Just keep searching...                                      
         return true;
      });

      if (done) {
         // Trait was found in the descriptor                           
         if constexpr (CT::Pinnable<D>) {
            // Make sure to pin the pinnable value                      
            value.mLocked = true;
         }

         return true;
      }

      // If reached, then no trait was found in the descriptor          
      // Let's delve into the hierarchy                                 
      for (auto owner : *this) {
         if constexpr (CT::Pinnable<D>) {
            if (owner->template SeekValue<T, SEEK, TypeOf<D>>(value))
               return true;
         }
         else {
            if (owner->template SeekValue<T, SEEK, D>(value))
               return true;
         }
      }

      // If reached, nothing was found                                  
      return false;
   }

   /// Seek data inside descriptor first, and then the hierarchy              
   /// If D is a pinnable type and data was found in descriptor, value will   
   /// be pinned, and any subsequent seeks will no longer affect this value   
   ///   @tparam SEEK - the direction we're seeking in the hierarchy          
   ///   @tparam D - type of data we're filling in (deducible)                
   ///   @param descriptor - descriptor to search through                     
   ///   @param value - the value to fill in                                  
   ///   @return true if value changed                                        
   template<SeekStyle SEEK, CT::Data D>
   bool Hierarchy::SeekValue(const Any& descriptor, D& value) const {
      if constexpr (CT::Pinnable<D>) {
         // Never touch pinned values                                   
         if (value.mLocked)
            return false;
      }

      // Scan descriptor                                                
      bool done = false;
      descriptor.ForEachDeep([&done,&value](const Block& group) {
         // Found match                                                 
         try {
            value = group.template AsCast<D>();
            done = true;
            return false;
         }
         catch(...) {
            // Data is not convertible                                  
         }

         // Just keep searching...                                      
         return true;
      });

      if (done) {
         // Data was found in the descriptor                            
         if constexpr (CT::Pinnable<D>) {
            // Make sure to pin the pinnable value                      
            value.mLocked = true;
         }

         return true;
      }

      // If reached, then no data was found in the descriptor           
      // Let's delve into the hierarchy                                 
      for (auto owner : *this) {
         if (owner->template SeekValue<SEEK>(value)) {
            // Value was found                                          
            return true;
         }
      }

      // If reached, nothing was found                                  
      return false;
   }

   /// Seek a trait inside the hierarchy                                      
   /// If D is a pinnable type and is pinned, it will not be overwritten      
   ///   @tparam T - the type of trait to search for                          
   ///   @tparam SEEK - the direction we're seeking in the hierarchy          
   ///   @tparam D - type of data we're filling in (deducible)                
   ///   @param value - the value to fill in                                  
   ///   @return true if value changed                                        
   template<CT::Trait T, SeekStyle SEEK, CT::Data D>
   bool Hierarchy::SeekTrait(D& value) const {
      if constexpr (CT::Pinnable<D>) {
         // Never touch pinned values                                   
         if (value.mLocked)
            return false;
      }

      // Let's delve into the hierarchy                                 
      for (auto owner : *this) {
         if (owner->template SeekValue<T, SEEK>(value)) {
            // Value was found                                          
            return true;
         }
      }

      // If reached, nothing was found                                  
      return false;
   }

   /// Seek a trait inside the hierarchy                                      
   /// If D is a pinnable type and is pinned, it will not be overwritten      
   ///   @tparam SEEK - the direction we're seeking in the hierarchy          
   ///   @tparam D - type of data we're filling in (deducible)                
   ///   @param value - the value to fill in                                  
   ///   @return true if value changed                                        
   template<SeekStyle SEEK, CT::Data D>
   bool Hierarchy::SeekValue(D& value) const {
      if constexpr (CT::Pinnable<D>) {
         // Never touch pinned values                                   
         if (value.mLocked)
            return false;
      }

      // Let's delve into the hierarchy                                 
      for (auto owner : *this) {
         if (owner->template SeekValue<SEEK>(value)) {
            // Value was found                                          
            return true;
         }
      }

      // If reached, nothing was found                                  
      return false;
   }

   /// Seek a unit inside descriptor first, and then the hierarchy            
   ///   @tparam T - the type of unit to search for                           
   ///   @tparam SEEK - the direction we're seeking in the hierarchy          
   ///   @param descriptor - descriptor to search through                     
   ///   @return a valid unit pointer, if unit was found                      
   template<CT::Data T, SeekStyle SEEK>
   Unit* Hierarchy::SeekUnit(const Any& descriptor) const {
      static_assert(CT::Unit<T>, "T must be a unit");

      // Scan descriptor                                                
      Unit* result {};
      descriptor.ForEachDeep([&result](const Unit* unit) {
         if (unit->template Is<T>()) {
            // Found match                                              
            result = unit;
            return false;
         }

         // Just keep searching...                                      
         return true;
      });

      if (result)
         return result;

      // If reached, then no unit was found in the descriptor           
      // Let's delve into the hierarchy                                 
      for (auto owner : *this) {
         result = owner->template SeekUnit<T, SEEK>();
         if (result)
            return result;
      }

      // If reached, nothing was found                                  
      return nullptr;
   }

   /// Seek a unit by token inside descriptor first, and then the hierarchy   
   ///   @tparam SEEK - the direction we're seeking in the hierarchy          
   ///   @param token - name of the unit to search for                        
   ///   @param descriptor - descriptor to search through                     
   ///   @return a valid unit pointer, if unit was found                      
   template<SeekStyle SEEK>
   Unit* Hierarchy::SeekUnit(const Token& token, const Any& descriptor) const {
      return SeekUnit<SEEK>(Construct::FromToken(token), descriptor);
   }

   /// Seek a unit by construct inside descriptor first, then the hierarchy   
   ///   @tparam SEEK - the direction we're seeking in the hierarchy          
   ///   @param what - the unit descriptor to match                           
   ///   @param descriptor - descriptor to search through                     
   ///   @return a valid unit pointer, if unit was found                      
   template<SeekStyle SEEK>
   Unit* Hierarchy::SeekUnit(const Construct& what, const Any& descriptor) const {
      // Scan descriptor                                                
      Unit* result {};
      descriptor.ForEachDeep([&](const Unit* unit) {
         if (unit->Is(what.GetType())) {
            // Found match                                              
            //TODO check construct descriptor
            result = const_cast<Unit*>(unit);
            return false;
         }

         // Just keep searching...                                      
         return true;
      });

      if (result)
         return result;

      // If reached, then no unit was found in the descriptor           
      // Let's delve into the hierarchy                                 
      for (auto owner : *this) {
         result = owner->template SeekUnit<SEEK>(what.GetType());
         //TODO implement Thing::SeekUnit by descriptor
         if (result)
            return result;
      }

      // If reached, nothing was found                                  
      return nullptr;
   }

   /// Seek a unit inside the hierarchy                                       
   ///   @tparam T - the type of unit to search for                           
   ///   @tparam SEEK - the direction we're seeking in the hierarchy          
   ///   @return a valid unit pointer, if unit was found                      
   template<CT::Data T, SeekStyle SEEK>
   Unit* Hierarchy::SeekUnit() const {
      static_assert(CT::Unit<T>, "T must be a unit");

      Unit* result {};
      for (auto owner : *this) {
         result = owner->template SeekUnit<T, SEEK>();
         if (result)
            return result;
      }

      // If reached, nothing was found                                  
      return nullptr;
   }

   /// Seek a unit by token inside the hierarchy                              
   ///   @tparam SEEK - the direction we're seeking in the hierarchy          
   ///   @param token - name of the unit to search for                        
   ///   @return a valid unit pointer, if unit was found                      
   template<SeekStyle SEEK>
   Unit* Hierarchy::SeekUnit(const Token& token) const {
      return SeekUnit<SEEK>(Construct::FromToken(token));
   }

   /// Seek a unit by construct inside the hierarchy                          
   ///   @tparam SEEK - the direction we're seeking in the hierarchy          
   ///   @param what - the unit descriptor to match                           
   ///   @return a valid unit pointer, if unit was found                      
   template<SeekStyle SEEK>
   Unit* Hierarchy::SeekUnit(const Construct& what) const {
      Unit* result {};
      for (auto owner : *this) {
         result = owner->template SeekUnit<SEEK>(what.GetType());
         //TODO implement Thing::SeekUnit by descriptor
         if (result)
            return result;
      }

      // If reached, nothing was found                                  
      return nullptr;
   }

} // namespace namespace Langulus::Entity