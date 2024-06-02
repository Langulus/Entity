///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Pin.hpp"

#define TEMPLATE()   template<CT::NotSemantic T>
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
      : mValue {other.Nest(DesemCast(other).mValue)}
      , mLocked {DesemCast(other).mLocked} {}

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
         mValue = rhs.Nest(DesemCast(rhs).mValue);
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

   /// Get the contained data                                                 
   ///   @return a reference to the contained data                            
   TEMPLATE() LANGULUS(INLINED)
   const T& PINNED()::Get() const noexcept {
      return mValue;
   }

   /// Get the contained data (mutable)                                       
   ///   @attention will throw if pin was locked when accessed                
   ///   @return a mutable reference to the contained data                    
   TEMPLATE() LANGULUS(INLINED)
   T& PINNED()::Get() {
      LANGULUS_ASSERT(not IsLocked(), Access,
         "Accessing mutable data in a locked pin");
      return mValue;
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

   /// Unpin the value and reset contents, if possible                        
   TEMPLATE() LANGULUS(INLINED)
   void PINNED()::Reset() noexcept {
      mLocked = false;
      if constexpr (requires { mValue.Reset(); })
         mValue.Reset();
   }

   /// Explicit cast to any type supported by the pinned value                
   TEMPLATE() template<class ALT> LANGULUS(INLINED)
   PINNED()::operator ALT () const noexcept requires CT::Convertible<T, ALT> {
      return static_cast<ALT>(mValue);
   }

   /// Implicit cast to the contained value                                   
   TEMPLATE() LANGULUS(INLINED)
   PINNED()::operator T const& () const noexcept {
      return mValue;
   }

   /// Get the most inner pointer of the contained item (nested & operator)   
   TEMPLATE() LANGULUS(INLINED)
   constexpr decltype(auto) PINNED()::operator & () const noexcept {
      if constexpr (requires { mValue.operator & (); })
         return mValue.operator & ();
      else
         return &mValue;
   }

   /// Get the most inner reference of the contained item (nested * operator) 
   TEMPLATE() LANGULUS(INLINED)
   constexpr decltype(auto) PINNED()::operator * () const noexcept {
      if constexpr (requires { mValue.operator * (); })
         return mValue.operator * ();
      else
         return (mValue);
   }

   /// Get the most inner pointer of the contained item (nested -> operator)  
   TEMPLATE() LANGULUS(INLINED)
   constexpr decltype(auto) PINNED()::operator -> () noexcept {
      if constexpr (requires { mValue.operator -> (); })
         return mValue.operator -> ();
      else if constexpr (CT::Dense<T>)
         return &mValue;
      else
         return mValue;
   }

   /// Get the most inner pointer of the contained item (nested -> operator)  
   TEMPLATE() LANGULUS(INLINED)
   constexpr decltype(auto) PINNED()::operator -> () const noexcept {
      if constexpr (requires { mValue.operator -> (); })
         return mValue.operator -> ();
      else if constexpr (CT::Dense<T>)
         return &mValue;
      else
         return mValue;
   }

} // namespace namespace Langulus::Entity

#undef TEMPLATE
#undef PINNED
