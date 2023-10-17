///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Pin.hpp"

#define TEMPLATE() template<class T>
#define PINNED() Pin<T>


namespace Langulus::Entity
{

   TEMPLATE() LANGULUS(INLINED)
   PINNED()::Pin() requires CT::Inner::Defaultable<T>
      : mValue {} {}

   TEMPLATE() LANGULUS(INLINED)
   PINNED()::Pin(const Pin& other) requires CT::Inner::CopyMakable<T>
      : mValue {other.mValue}
      , mLocked {other.mLocked} {}

   TEMPLATE() LANGULUS(INLINED)
   PINNED()::Pin(Pin&& other) requires CT::Inner::MoveMakable<T>
      : mValue {::std::move(other.mValue)}
      , mLocked {other.mLocked} {}

   /// Descriptor-constructor                                                 
   ///   @param describe - the descriptor to use                              
   TEMPLATE() LANGULUS(INLINED)
   PINNED()::Pin(Describe&& describe) requires CT::Inner::DescriptorMakable<T>
      : T {::std::forward<Describe>(describe)} {}

   TEMPLATE() LANGULUS(INLINED)
   PINNED()& PINNED()::operator = (const Pin& rhs) requires CT::Inner::CopyAssignable<T> {
      if (not mLocked)
         mValue = rhs.mValue;
      return *this;
   }

   TEMPLATE() LANGULUS(INLINED)
   PINNED()& PINNED()::operator = (Pin&& rhs) requires CT::Inner::MoveAssignable<T> {
      if (not mLocked)
         mValue = ::std::move(rhs.mValue);
      return *this;
   }

   TEMPLATE() LANGULUS(INLINED)
   PINNED()& PINNED()::operator = (const T& rhs) requires CT::Inner::CopyAssignable<T> {
      if (not mLocked)
         mValue = rhs;
      return *this;
   }

   TEMPLATE() LANGULUS(INLINED)
   PINNED()& PINNED()::operator = (T&& rhs) requires CT::Inner::MoveAssignable<T> {
      if (not mLocked)
         mValue = ::std::move(rhs);
      return *this;
   }

   TEMPLATE() LANGULUS(INLINED)
   bool PINNED()::operator == (const Pin& rhs) const requires CT::Inner::Comparable<T> {
      return mValue == rhs.mValue;
   }

   TEMPLATE() LANGULUS(INLINED)
   bool PINNED()::operator == (const T& rhs) const requires CT::Inner::Comparable<T> {
      return mValue == rhs;
   }

   /// Check if the value is pinned                                           
   ///   @return true if value is pinned                                      
   TEMPLATE() LANGULUS(INLINED)
   bool PINNED()::IsLocked() const noexcept {
      return mLocked;
   }

   /// Pin the value, it will no longer be overwritable by environment        
   TEMPLATE() LANGULUS(INLINED)
   void PINNED()::Lock() noexcept {
      mLocked = true;
   }

   /// Unpin the value, it now can be overwritten by environment              
   TEMPLATE() LANGULUS(INLINED)
   void PINNED()::Unlock() noexcept {
      mLocked = false;
   }

   TEMPLATE() LANGULUS(INLINED)
   const T& PINNED()::operator * () const noexcept {
      return mValue;
   }

   TEMPLATE() LANGULUS(INLINED)
   T& PINNED()::operator * () noexcept {
      return mValue;
   }

   TEMPLATE() LANGULUS(INLINED)
   const T& PINNED()::operator -> () const noexcept {
      return mValue;
   }

   TEMPLATE() LANGULUS(INLINED)
   T& PINNED()::operator -> () noexcept {
      return mValue;
   }

} // namespace namespace Langulus::Entity

#undef TEMPLATE
#undef PINNED
