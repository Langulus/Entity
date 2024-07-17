///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Common.hpp"


namespace Langulus
{
   namespace A
   {

      /// Abstract pinnable value                                             
      struct Pinnable {
         LANGULUS(ABSTRACT) true;
      };

   } // namespace Langulus::A

   namespace CT
   {

      /// Concept for a pinnable value                                        
      template<class...T>
      concept Pinnable = (DerivedFrom<T, A::Pinnable> and ...);

      /// Concept for a non-pinnable value                                    
      template<class...T>
      concept NotPinnable = ((not Pinnable<T>) and ...);

   } // namespace Langulus::CT

} // namespace Langulus


namespace Langulus::Entity
{

   ///                                                                        
   ///   Pinnable item                                                        
   ///                                                                        
   ///   Lockable value, that wouldn't change on operator =, if locked        
   /// You are able to pin members of units, making them not dependent on     
   /// hierarchy. Otherwise, unpinned values may change on Unit::Refresh()    
   /// When cloning, only the pinned values will be cloned                    
   ///                                                                        
   template<CT::NoIntent T>
   struct Pin : A::Pinnable {
   protected:
      // Hierarchies are allowed to pin pinnables                       
      friend struct Hierarchy;
      friend class Thing;

      // The value itself                                               
      T mValue;

      // Is the pinnable value pinned?                                  
      Traits::State::Tag<bool> mLocked = false;

   public:
      LANGULUS(ABSTRACT) false;
      LANGULUS(TYPED) T;
      LANGULUS_BASES(A::Pinnable);
      LANGULUS_MEMBERS(&Pin::mValue, &Pin::mLocked);

      ///                                                                     
      ///   Construction                                                      
      ///                                                                     
      Pin() requires CT::Defaultable<T>;
      Pin(const Pin&) requires CT::ReferMakable<T>;
      Pin(Pin&&) requires CT::MoveMakable<T>;
      template<template<class> class S>
      Pin(S<Pin>&&) requires CT::IntentMakable<S, T>;

      template<CT::NotPinnable...TN>
      Pin(TN&&...) requires ::std::constructible_from<T, TN...>;

      ///                                                                     
      ///   Assignment                                                        
      ///                                                                     
      Pin& operator = (const Pin&) requires CT::ReferAssignable<T>;
      Pin& operator = (Pin&&) requires CT::MoveAssignable<T>;

      template<template<class> class S> requires CT::IntentAssignable<S, T>
      Pin& operator = (S<Pin>&&);

      template<CT::NotPinnable A> requires CT::AssignableFrom<T, A>
      Pin& operator = (A&&);

      ///                                                                     
      ///   Capsulation                                                       
      ///                                                                     
      NOD() bool IsLocked() const noexcept;

      auto Get() -> T&;
      auto Get() const noexcept -> T const&;
      
      template<class ALT>
      explicit operator ALT () const noexcept requires CT::Convertible<T, ALT>;
      operator T const& () const noexcept;

      constexpr decltype(auto) operator &  () const noexcept;
      constexpr decltype(auto) operator *  () const noexcept;
      constexpr decltype(auto) operator -> () noexcept;
      constexpr decltype(auto) operator -> () const noexcept;

      ///                                                                     
      ///   Comparison                                                        
      ///                                                                     
      bool operator == (const Pin&) const requires CT::Comparable<T, T>;
      template<CT::NotPinnable T1>
      bool operator == (const T1&) const requires CT::Comparable<T, T1>;

      ///                                                                     
      ///   Services                                                          
      ///                                                                     
      void Lock() noexcept;
      void Unlock() noexcept;
      void Reset() noexcept;
   };

} // namespace Langulus::Entity