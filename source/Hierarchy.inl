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
   LANGULUS(INLINED)
   Pinnable<T>::Pinnable(const T& value) requires (CT::CopyMakable<T>)
      : mValue {value} { }

   /// Move-construction                                                      
   template<class T>
   LANGULUS(INLINED)
   Pinnable<T>::Pinnable(T&& value) noexcept requires (CT::MoveMakable<T>)
      : mValue {Forward<T>(value)} { }

   /// Copy-assignment                                                        
   template<class T>
   LANGULUS(INLINED)
   Pinnable<T>& Pinnable<T>::operator = (const T& rhs) requires (CT::CopyAssignable<T>) {
      mValue = rhs;
      return *this;
   }

   /// Move-assignment                                                        
   template<class T>
   LANGULUS(INLINED)
   Pinnable<T>& Pinnable<T>::operator = (T&& rhs) noexcept requires (CT::MoveAssignable<T>) {
      mValue = Forward<T>(rhs);
      return *this;
   }

   /// Check if the value is pinned                                           
   template<class T>
   LANGULUS(INLINED)
   bool Pinnable<T>::IsPinned() const noexcept {
      return mLocked;
   }

   /// Pin the value, it will no longer be overwritable by environment        
   template<class T>
   LANGULUS(INLINED)
   void Pinnable<T>::Pin() noexcept {
      mLocked = true;
   }

   /// Unpin the value, it now can be overwritten by environment              
   template<class T>
   LANGULUS(INLINED)
   void Pinnable<T>::Unpin() noexcept {
      mLocked = false;
   }

   template<class T>
   LANGULUS(INLINED)
   const T& Pinnable<T>::Get() const noexcept {
      return mValue;
   }

   template<class T>
   LANGULUS(INLINED)
   T& Pinnable<T>::Get() noexcept {
      return mValue;
   }
   
   template<class T>
   LANGULUS(INLINED)
   Pinnable<T>::operator const T& () const noexcept {
      return mValue;
   }

   template<class T>
   LANGULUS(INLINED)
   Pinnable<T>::operator T& () noexcept {
      return mValue;
   }
   
} // namespace namespace Langulus::Entity