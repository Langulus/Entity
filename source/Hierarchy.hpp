///                                                                           
/// Langulus::Entity                                                          
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Common.hpp"

namespace Langulus::Entity
{

   struct Unit;
   class Thing;
   class Runtime;
   struct Hierarchy;


   ///                                                                        
   ///   Pinnable member                                                      
   ///                                                                        
   /// You are able to pin values, making them not dependent on hierarchy     
   /// Usually, unpinned values may change on Unit::Refresh()                 
   ///                                                                        
   template<class T>
   struct Pinnable : T {
      friend struct Hierarchy;

      // Is the pinnable value pinned?                                  
      bool mLocked = false;

      LANGULUS(POD) CT::POD<T>;
      LANGULUS(NULLIFIABLE) CT::Nullifiable<T>;
      LANGULUS_BASES(T);
      LANGULUS_PROPERTIES_START(Pinnable)
         LANGULUS_PROPERTY_TRAIT(mLocked, State),
      LANGULUS_PROPERTIES_END();

   public:
      static constexpr bool CTTI_Pinnable = true;

      using MemberType = T;

      Pinnable() = default;
      Pinnable(const Pinnable&) = default;
      Pinnable(Pinnable&&) noexcept = default;
      Pinnable(const T&) requires (CT::CopyMakable<T>);
      Pinnable(T&&) noexcept requires (CT::MoveMakable<T>);

      Pinnable& operator = (const Pinnable&) = default;
      Pinnable& operator = (Pinnable&&) noexcept = default;
      Pinnable& operator = (const T&) requires (CT::Copyable<T>);
      Pinnable& operator = (T&&) noexcept requires (CT::Movable<T>);

      void Pin() noexcept;
      void Unpin() noexcept;

      NOD() bool IsPinned() const noexcept;
   };


   ///                                                                        
   ///   A hierarchy interface                                                
   ///                                                                        
   /// Simply a sparse container of Things, with various quality-of-life      
   /// functions related to hierarchical retrieval of objects, traits, etc.   
   ///                                                                        
   struct Hierarchy : TAny<Thing*> {
      using TAny::TAny;
      using TAny::operator =;

      template<CT::Trait, SeekStyle = SeekStyle::UpToHere, CT::Data D>
      bool SeekTrait(const Any&, D&) const;
      template<CT::Trait, SeekStyle = SeekStyle::UpToHere, CT::Data D>
      bool SeekTrait(D&) const;

      template<SeekStyle = SeekStyle::UpToHere, CT::Data D>
      bool SeekValue(const Any&, D&) const;
      template<SeekStyle = SeekStyle::UpToHere, CT::Data D>
      bool SeekValue(D&) const;

      template<CT::Data T, SeekStyle = SeekStyle::UpToHere>
      Unit* SeekUnit(const Any&) const;
      template<CT::Data, SeekStyle = SeekStyle::UpToHere>
      Unit* SeekUnit() const;

      template<SeekStyle = SeekStyle::UpToHere>
      Unit* SeekUnit(const Construct&, const Any&) const;
      template<SeekStyle = SeekStyle::UpToHere>
      Unit* SeekUnit(const Construct&) const;

      #if LANGULUS_FEATURE(MANAGED_MEMORY)
         template<SeekStyle = SeekStyle::UpToHere>
         Unit* SeekUnit(const Token&, const Any&) const;
         template<SeekStyle = SeekStyle::UpToHere>
         Unit* SeekUnit(const Token&) const;
      #endif
   };

} // namespace Langulus::Entity


namespace Langulus::CT
{

   template<class T>
   concept Pinnable = requires { Decay<T>::CTTI_Pinnable; };

} // namespace Langulus::CT