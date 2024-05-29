///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Pin.hpp"

#define TEMPLATE()   template<class T>
#define PINNED()     Pin<T>


namespace Langulus::Entity
{

   /// Default construction                                                   
   TEMPLATE() LANGULUS(INLINED)
   PINNED()::Pin() requires CT::Defaultable<T>
      : mValue {} {
      static_assert(not CT::Pinnable<T>, "Can't nest pinnable types");
   }

   /// Refer construction                                                     
   ///   @param other - the pinned value to refer to                          
   TEMPLATE() LANGULUS(INLINED)
   PINNED()::Pin(const Pin& other) requires CT::ReferMakable<T>
      : Pin {Refer(other)} {}

   /// Move construction                                                      
   ///   @param other - the pinned value to move                              
   TEMPLATE() LANGULUS(INLINED)
   PINNED()::Pin(Pin&& other) requires CT::MoveMakable<T>
      : Pin {Move(other)} {}

   /// Semantic construction                                                  
   ///   @param other - the value to initialize with                          
   TEMPLATE() template<template<class> class S> LANGULUS(INLINED)
   PINNED()::Pin(S<Pin>&& other) requires CT::SemanticMakable<S, T>
      : mValue {S<T> {other->mValue}}
      , mLocked {other->mLocked} {}

   /// Forward any compatible arguments towards contained value constructor   
   ///   @param arguments... - the arguments to forward                       
   TEMPLATE() template<CT::NotPinnable...A> LANGULUS(INLINED)
   PINNED()::Pin(A&&...arguments) requires ::std::constructible_from<T, A...>
      : mValue {Forward<A>(arguments)...} {}

   /// Refer assignment                                                       
   ///   @attention doesn't change value, if locked                           
   ///   @param rhs - the pinned value to refer to                            
   TEMPLATE() LANGULUS(INLINED)
   PINNED()& PINNED()::operator = (const Pin& rhs) requires CT::ReferAssignable<T> {
      return operator = (Refer(rhs));
   }

   /// Move assignment                                                        
   ///   @attention doesn't change value, if locked                           
   ///   @param rhs - the pinned value to move-assign                         
   TEMPLATE() LANGULUS(INLINED)
   PINNED()& PINNED()::operator = (Pin&& rhs) requires CT::MoveAssignable<T> {
      return operator = (Move(rhs));
   }
   
   /// Semantic assignment                                                    
   ///   @attention doesn't change value, if locked                           
   ///   @param rhs - the pinned value to move-assign                         
   TEMPLATE() template<template<class> class S>
   requires CT::SemanticAssignable<S, T> LANGULUS(INLINED)
   PINNED()& PINNED()::operator = (S<Pin>&& rhs) {
      if (not mLocked)
         mValue = S<T> {rhs->mValue};
      return *this;
   }

   /// Forward any compatible argument towards contained value assigner       
   ///   @param argument - the argument to forward                            
   TEMPLATE() template<CT::NotPinnable A>
   requires CT::AssignableFrom<T, A> LANGULUS(INLINED)
   PINNED()& PINNED()::operator = (A&& arg) {
      if (not mLocked)
         mValue = Forward<A>(arg);
      return *this;
   }

   /// Compare two pinned values                                              
   ///   @param rhs - the pinned value to compare against                     
   ///   @return true if values match                                         
   TEMPLATE() LANGULUS(INLINED)
   bool PINNED()::operator == (const Pin& rhs) const requires CT::Comparable<T, T> {
      return mValue == rhs.mValue;
   }

   /// Compare with anything that is comparable to the contained value        
   ///   @param rhs - thing to compare with                                   
   ///   @return true if contained value equals rhs                           
   TEMPLATE() template<CT::NotPinnable A> LANGULUS(INLINED)
   bool PINNED()::operator == (const A& rhs) const requires CT::Comparable<T, A> {
      return mValue == rhs;
   }

   /// Check if the value is pinned - pinned values aren't affected by assign 
   ///   @return true if value is pinned                                      
   TEMPLATE() LANGULUS(INLINED)
   bool PINNED()::IsLocked() const noexcept {
      return mLocked;
   }

   /// Pin the value, it will no longer be overwritable by assignment         
   TEMPLATE() LANGULUS(INLINED)
   void PINNED()::Lock() noexcept {
      mLocked = true;
   }

   /// Unpin the value, it now can be overwritten by assignment               
   TEMPLATE() LANGULUS(INLINED)
   void PINNED()::Unlock() noexcept {
      mLocked = false;
   }

   /// Get a reference to the contained value                                 
   ///   @return constant reference to the contained value                    
   TEMPLATE() LANGULUS(INLINED)
   const T& PINNED()::operator * () const noexcept {
      return mValue;
   }

   /// Get a reference to the contained value                                 
   ///   @return constant reference to the contained value                    
   TEMPLATE() LANGULUS(INLINED)
   const T* PINNED()::operator -> () const noexcept {
      if constexpr (CT::Sparse<T>)
         return mValue;
      else
         return &mValue;
   }

   /// Get a reference to the contained value                                 
   ///   @return mutable reference to the contained value                     
   TEMPLATE() LANGULUS(INLINED)
   T* PINNED()::operator -> () noexcept {
      if constexpr (CT::Sparse<T>)
         return mValue;
      else
         return &mValue;
   }

} // namespace namespace Langulus::Entity

#undef TEMPLATE
#undef PINNED
