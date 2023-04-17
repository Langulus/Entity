///                                                                           
/// Langulus::Entity                                                          
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Pinnable.hpp"

#define TEMPLATE() template<class T>
#define PINNED() Pinnable<T>

namespace Langulus::Entity
{

   /// Copy-construction                                                      
   ///   @param value - value to copy                                         
   TEMPLATE() LANGULUS(INLINED)
   PINNED()::Pinnable(const T& value) requires CT::CopyMakable<T>
      : mValue {value} { }

   /// Move-construction                                                      
   ///   @param value - value to transfer                                     
   TEMPLATE() LANGULUS(INLINED)
   PINNED()::Pinnable(T&& value) noexcept requires CT::MoveMakable<T>
      : mValue {Forward<T>(value)} { }

   /// Copy-assignment                                                        
   ///   @param rhs - value to assign                                         
   TEMPLATE() LANGULUS(INLINED)
   PINNED()& PINNED()::operator = (const T& rhs) requires CT::CopyAssignable<T> {
      mValue = rhs;
      return *this;
   }

   /// Move-assignment                                                        
   ///   @param rhs - value to assign                                         
   TEMPLATE() LANGULUS(INLINED)
   PINNED()& PINNED()::operator = (T&& rhs) noexcept requires CT::MoveAssignable<T> {
      mValue = Forward<T>(rhs);
      return *this;
   }

   /// Compare values                                                         
   ///   @param rhs - the value to compare against                            
   TEMPLATE() LANGULUS(INLINED)
   bool PINNED()::operator == (const T& rhs) const noexcept {
      return mValue == rhs;
   }

   /// Check if the value is pinned                                           
   ///   @return true if value is pinned                                      
   TEMPLATE() LANGULUS(INLINED)
   bool PINNED()::IsPinned() const noexcept {
      return mLocked;
   }

   /// Pin the value, it will no longer be overwritable by environment        
   TEMPLATE() LANGULUS(INLINED)
   void PINNED()::Pin() noexcept {
      mLocked = true;
   }

   /// Unpin the value, it now can be overwritten by environment              
   TEMPLATE() LANGULUS(INLINED)
   void PINNED()::Unpin() noexcept {
      mLocked = false;
   }

   /// Get a constant reference to the contained value                        
   ///   @return a constant reference to the contained value                  
   TEMPLATE() LANGULUS(INLINED)
   const T& PINNED()::Get() const noexcept {
      return mValue;
   }

   /// Get a mutable reference to the contained value                         
   ///   @return a mutable reference to the contained value                   
   TEMPLATE() LANGULUS(INLINED)
   T& PINNED()::Get() noexcept {
      return mValue;
   }
   
   /// Relay a dynamic_cast to the contained type, if it has the As() function
   ///   @tparam ALTT - the type to dynamic_cast to                           
   ///   @return the result of the dynamic cast                               
   TEMPLATE()
   template<class ALTT>
   LANGULUS(INLINED)
   decltype(auto) PINNED()::As() const noexcept requires Inner::HasAs<T, ALTT> {
      return mValue.template As<ALTT>();
   }

   /// Relay an arrow operator to the contained type, if it has it (const)    
   ///   @return the result of the T's arrow operator                         
   TEMPLATE() LANGULUS(INLINED)
   const T* PINNED()::operator -> () const noexcept {
      return &mValue;
   }

   /// Relay an arrow operator to the contained type, if it has it            
   ///   @return the result of the T's arrow operator                         
   TEMPLATE() LANGULUS(INLINED)
   T* PINNED()::operator -> () noexcept {
      return &mValue;
   }

   /// Dereference the contained type, if possible (const)                    
   ///   @return the dereferenced value                                       
   TEMPLATE() LANGULUS(INLINED)
   const T& PINNED()::operator * () const noexcept {
      return *mValue;
   }

   /// Dereference the contained type, if possible                            
   ///   @return the dereferenced value                                       
   TEMPLATE() LANGULUS(INLINED)
   T& PINNED()::operator * () noexcept {
      return *mValue;
   }

   /// Relay a subscript operator to the contained type, if it has it (const) 
   ///   @tparam INDEX - the type of index to use                             
   ///   @return the result of the T's subscript operator                     
   TEMPLATE()
   template<class INDEX>
   LANGULUS(INLINED)
   decltype(auto) PINNED()::operator [] (const INDEX& i) const requires Inner::HasSubscriptConst<T, INDEX> {
      return mValue[i];
   }

   /// Relay a subscript operator to the contained type, if it has it         
   ///   @tparam INDEX - the type of index to use                             
   ///   @return the result of the T's subscript operator                     
   TEMPLATE()
   template<class INDEX>
   LANGULUS(INLINED)
   decltype(auto) PINNED()::operator [] (const INDEX& i) requires Inner::HasSubscript<T, INDEX> {
      return mValue[i];
   }

   /// Implicitly cast to the contained type's reference (const)              
   ///   @return the immutable reference to the contained value               
   TEMPLATE() LANGULUS(INLINED)
   PINNED()::operator const T& () const noexcept {
      return mValue;
   }

   /// Implicitly cast to the contained type's reference                      
   ///   @return the mutable reference to the contained value                 
   TEMPLATE() LANGULUS(INLINED)
   PINNED()::operator T& () noexcept {
      return mValue;
   }
   
} // namespace namespace Langulus::Entity

#undef TEMPLATE
#undef PINNED
