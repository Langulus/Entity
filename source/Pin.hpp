///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Module.hpp"

namespace Langulus
{

   namespace A
   {
      /// Abstract pinnable value                                             
      struct Pinnable {
         LANGULUS(ABSTRACT) true;
      };
   }

   namespace CT
   {
      /// Concept for a pinnable value                                        
      template<class... T>
      concept Pinnable = (DerivedFrom<T, A::Pinnable> and ...);
   }

} // namespace Langulus::A

namespace Langulus::Entity
{

   ///                                                                        
   ///   Pinnable item                                                        
   ///                                                                        
   /// Lockable value, that wouldn't change on operator =, if locked          
   /// You are able to pin members of units, making them not dependent on     
   /// hierarchy. Otherwise, unpinned values may change on Unit::Refresh()    
   ///                                                                        
   template<class T>
   struct Pin : A::Pinnable {
   protected:
      // Hierarchies are allowed to pin pinnables                       
      friend struct Hierarchy;
      friend class Thing;

      // The value itself                                               
      T mValue;

      // Is the pinnable value pinned?                                  
      bool mLocked {};

   public:
      LANGULUS(ABSTRACT) false;
      LANGULUS_BASES(A::Pinnable);

      LANGULUS_PROPERTIES_START(Pin)
         LANGULUS_PROPERTY_TRAIT(mLocked, State),
      LANGULUS_PROPERTIES_END();

      Pin() requires CT::Inner::Defaultable<T>;
      Pin(const Pin&) requires CT::Inner::CopyMakable<T>;
      Pin(Pin&&) requires CT::Inner::MoveMakable<T>;

      //Pin(const Neat&);

      Pin& operator = (const Pin&) requires CT::Inner::CopyAssignable<T>;
      Pin& operator = (Pin&&) requires CT::Inner::MoveAssignable<T>;
      Pin& operator = (const T&) requires CT::Inner::CopyAssignable<T>;
      Pin& operator = (T&&) requires CT::Inner::MoveAssignable<T>;

      bool operator == (const Pin&) const requires CT::Inner::Comparable<T>;

      void Lock() noexcept;
      void Unlock() noexcept;

      NOD() bool IsLocked() const noexcept;

      const T& operator *  () const noexcept;
            T& operator *  ()       noexcept;
      const T& operator -> () const noexcept;
            T& operator -> ()       noexcept;
   };

} // namespace Langulus::Entity