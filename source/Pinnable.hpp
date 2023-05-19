///                                                                           
/// Langulus::Entity                                                          
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>                    
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
      template<class... T>
      concept Pinnable = (DerivedFrom<T, A::Pinnable> && ...);
   }

} // namespace Langulus::A

namespace Langulus::Entity
{

   ///                                                                        
   ///   Pinnable member                                                      
   ///                                                                        
   /// You are able to pin values, making them not dependent on hierarchy     
   /// Usually, unpinned values may change on Unit::Refresh()                 
   ///                                                                        
   template<class T>
   struct Pinnable : T, A::Pinnable {
   protected:
      static_assert(!CT::Pinnable<T>, "Can't nest Pinnable types");

      // Hierarchies are allowed to pin pinnables                       
      friend struct Hierarchy;
      friend class Thing;

      // Is the pinnable value pinned?                                  
      bool mLocked {};

   public:
      LANGULUS(ABSTRACT) false;
      LANGULUS(NAME) RTTI::CppNameOf<Pinnable>();
      LANGULUS_BASES(T, A::Pinnable);

      LANGULUS_PROPERTIES_START(Pinnable)
         LANGULUS_PROPERTY_TRAIT(mLocked, State),
      LANGULUS_PROPERTIES_END();

      Pinnable() = default;
      Pinnable(const Pinnable&) = default;
      Pinnable(Pinnable&&) noexcept = default;
      using T::T;

      Pinnable(const Descriptor&) requires (CT::DescriptorMakable<T>);
      Pinnable(const Descriptor&) requires (!CT::DescriptorMakable<T>);

      Pinnable& operator = (const Pinnable&) = default;
      Pinnable& operator = (Pinnable&&) noexcept = default;
      using T::operator =;

      void Pin() noexcept;
      void Unpin() noexcept;

      NOD() bool IsPinned() const noexcept;
   };

} // namespace Langulus::Entity

#include "../source/Pinnable.inl"
