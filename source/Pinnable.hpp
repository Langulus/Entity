///                                                                           
/// Langulus::Entity                                                          
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Module.hpp"

namespace Langulus::A
{

   /// Abstract pinnable value                                                
   struct Pinnable {
      LANGULUS(ABSTRACT) true;
   };

} // namespace Langulus::A

namespace Langulus::Entity
{
   namespace Inner
   {
      template<class T, class ALTT>
      concept HasAs = requires(const T& a) {
         {a.template As<ALTT>()} -> CT::Exact<ALTT>;
      };

      template<class T, class INDEX>
      concept HasSubscriptConst = requires(const T& a, INDEX i) { a[i]; };

      template<class T, class INDEX>
      concept HasSubscript =      requires(T& a, INDEX i)       { a[i]; };

   } // namespace Langulus::Entity::Inner


   ///                                                                        
   ///   Pinnable member                                                      
   ///                                                                        
   /// You are able to pin values, making them not dependent on hierarchy     
   /// Usually, unpinned values may change on Unit::Refresh()                 
   ///                                                                        
   template<class T>
   struct Pinnable : A::Pinnable {
   protected:
      // Hierarchies are allowed to pin pinnables                       
      friend struct Hierarchy;
      friend class Thing;

      // The value                                                      
      T mValue {};
      // Is the pinnable value pinned?                                  
      bool mLocked {};

   public:
      LANGULUS(POD) CT::POD<T>;
      LANGULUS(NULLIFIABLE) CT::Nullifiable<T>;
      LANGULUS(TYPED) T;
      LANGULUS_BASES(A::Pinnable);
      LANGULUS_PROPERTIES_START(Pinnable)
         LANGULUS_PROPERTY_TRAIT(mLocked, State),
      LANGULUS_PROPERTIES_END();

      Pinnable() = default;
      Pinnable(const Pinnable&) = default;
      Pinnable(Pinnable&&) noexcept = default;
      Pinnable(const T&) requires CT::CopyMakable<T>;
      Pinnable(T&&) noexcept requires CT::MoveMakable<T>;

      Pinnable& operator = (const Pinnable&) = default;
      Pinnable& operator = (Pinnable&&) noexcept = default;
      Pinnable& operator = (const T&) requires CT::CopyAssignable<T>;
      Pinnable& operator = (T&&) noexcept requires CT::MoveAssignable<T>;

      NOD() bool operator == (const T&) const noexcept;

      void Pin() noexcept;
      void Unpin() noexcept;

      NOD() bool IsPinned() const noexcept;

      NOD() const T& Get() const noexcept;
      NOD() T& Get() noexcept;

      template<class ALTT>
      NOD() decltype(auto) As() const noexcept requires Inner::HasAs<T, ALTT>;

      NOD() const T* operator -> () const noexcept;
      NOD() T* operator -> () noexcept;

      NOD() const Deptr<T>& operator * () const noexcept;
      NOD() Deptr<T>& operator * () noexcept;

      template<class INDEX>
      NOD() decltype(auto) operator [] (const INDEX&) const requires Inner::HasSubscriptConst<T, INDEX>;
      template<class INDEX>
      NOD() decltype(auto) operator [] (const INDEX&) requires Inner::HasSubscript<T, INDEX>;

      NOD() operator const T&() const noexcept;
      NOD() operator T&() noexcept;
   };

} // namespace Langulus::Entity


namespace Langulus::CT
{

   template<class... T>
   concept Pinnable = (DerivedFrom<T, A::Pinnable> && ...);

} // namespace Langulus::CT

#include "../source/Pinnable.inl"

namespace fmt
{

   ///                                                                        
   /// Extend FMT to be capable of logging any pinnable values                
   ///                                                                        
   template<::Langulus::CT::Pinnable T>
   struct formatter<T> {
      template<class CONTEXT>
      constexpr auto parse(CONTEXT& ctx) {
         return ctx.begin();
      }

      template<class CONTEXT>
      LANGULUS(INLINED)
      auto format(T const& element, CONTEXT& ctx) {
         using namespace ::Langulus;
         return fmt::vformat_to(ctx.out(), "{}",
            fmt::make_format_args(element.Get()));
      }
   };

} // namespace fmt
