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

      ///                                                                     
      ///   Seek                                                              
      ///                                                                     
      #if LANGULUS_FEATURE(MANAGED_MEMORY)
         template<SeekStyle = SeekStyle::HereAndAbove>
         NOD() Unit* SeekUnit(const Token&, const Index& = IndexFirst);
         template<SeekStyle = SeekStyle::HereAndAbove>
         NOD() const Unit* SeekUnit(const Token&, const Index& = IndexFirst) const;
         template<SeekStyle = SeekStyle::HereAndAbove>
         NOD() const Unit* SeekUnit(const Any&, const Token&, Index = IndexFirst) const;
      #endif

      template<SeekStyle = SeekStyle::HereAndAbove>
      NOD() Unit* SeekUnit(DMeta, const Index& = IndexFirst);
      template<SeekStyle = SeekStyle::HereAndAbove>
      NOD() const Unit* SeekUnit(DMeta, const Index& = IndexFirst) const;
      template<SeekStyle = SeekStyle::HereAndAbove>
      NOD() const Unit* SeekUnit(const Any&, DMeta, const Index& = IndexFirst) const;

      template<CT::Data T, SeekStyle = SeekStyle::HereAndAbove>
      NOD() Decay<T>* SeekUnit(const Index& = IndexFirst);
      template<CT::Data T, SeekStyle = SeekStyle::HereAndAbove>
      NOD() const Decay<T>* SeekUnit(const Index& = IndexFirst) const;
      template<CT::Data T, SeekStyle = SeekStyle::HereAndAbove>
      NOD() const Decay<T>* SeekUnit(const Any&, const Index& = IndexFirst) const;

      #if LANGULUS_FEATURE(MANAGED_MEMORY)
         template<SeekStyle = SeekStyle::HereAndAbove>
         NOD() Trait SeekTrait(const Token&, const Index& = IndexFirst);
         template<SeekStyle = SeekStyle::HereAndAbove>
         NOD() Trait SeekTrait(const Token&, const Index& = IndexFirst) const;
         template<SeekStyle = SeekStyle::HereAndAbove, CT::Data D>
         NOD() bool SeekTrait(const Token&, D&, const Index& = IndexFirst) const;

         template<SeekStyle = SeekStyle::HereAndAbove>
         NOD() Trait SeekTrait(const Any&, const Token&, const Index& = IndexFirst) const;
         template<SeekStyle = SeekStyle::HereAndAbove, CT::Data D>
         NOD() bool SeekTrait(const Any&, const Token&, D&, const Index& = IndexFirst) const;
      #endif

      template<SeekStyle = SeekStyle::HereAndAbove>
      NOD() Trait SeekTrait(TMeta, const Index& = IndexFirst);
      template<SeekStyle = SeekStyle::HereAndAbove>
      NOD() Trait SeekTrait(TMeta, const Index& = IndexFirst) const;
      template<SeekStyle = SeekStyle::HereAndAbove, CT::Data D>
      NOD() bool SeekTrait(TMeta, D&, const Index& = IndexFirst) const;

      template<SeekStyle = SeekStyle::HereAndAbove>
      NOD() Trait SeekTrait(const Any&, TMeta, const Index& = IndexFirst) const;
      template<SeekStyle = SeekStyle::HereAndAbove, CT::Data D>
      NOD() bool SeekTrait(const Any&, TMeta, D&, const Index& = IndexFirst) const;

      template<CT::Trait T = Trait, SeekStyle = SeekStyle::HereAndAbove>
      NOD() Trait SeekTrait(const Index& = IndexFirst);
      template<CT::Trait T = Trait, SeekStyle = SeekStyle::HereAndAbove>
      NOD() Trait SeekTrait(const Index& = IndexFirst) const;
      template<CT::Trait T = Trait, SeekStyle = SeekStyle::HereAndAbove, CT::Data D>
      NOD() bool SeekTrait(D&, const Index& = IndexFirst) const;

      template<CT::Trait T = Trait, SeekStyle = SeekStyle::HereAndAbove>
      NOD() Trait SeekTrait(const Any&, const Index& = IndexFirst) const;
      template<CT::Trait T = Trait, SeekStyle = SeekStyle::HereAndAbove, CT::Data D>
      NOD() bool SeekTrait(const Any&, D&, const Index& = IndexFirst) const;

      template<SeekStyle = SeekStyle::HereAndAbove, CT::Data D>
      bool SeekValue(D&, const Index& = IndexFirst) const;
      template<SeekStyle = SeekStyle::HereAndAbove, CT::Data D>
      bool SeekValue(const Any&, D&, const Index& = IndexFirst) const;

      ///                                                                     
      ///   Gather                                                            
      ///                                                                     
      #if LANGULUS_FEATURE(MANAGED_MEMORY)
         template<SeekStyle = SeekStyle::HereAndAbove>
         NOD() TAny<Unit*> GatherUnits(const Token&);
         template<SeekStyle = SeekStyle::HereAndAbove>
         NOD() TAny<const Unit*> GatherUnits(const Token&) const;
      #endif 

      template<SeekStyle = SeekStyle::HereAndAbove>
      NOD() TAny<Unit*> GatherUnits(DMeta);
      template<SeekStyle = SeekStyle::HereAndAbove>
      NOD() TAny<const Unit*> GatherUnits(DMeta) const;
      template<CT::Data T = Unit, SeekStyle = SeekStyle::HereAndAbove>
      NOD() TAny<Decay<T>*> GatherUnits();
      template<CT::Data T = Unit, SeekStyle = SeekStyle::HereAndAbove>
      NOD() TAny<const Decay<T>*> GatherUnits() const;

      #if LANGULUS_FEATURE(MANAGED_MEMORY)
         template<SeekStyle = SeekStyle::HereAndAbove>
         NOD() TAny<Trait> GatherTraits(const Token&);
         template<SeekStyle = SeekStyle::HereAndAbove>
         NOD() TAny<Trait> GatherTraits(const Token&) const;
      #endif 

      template<SeekStyle = SeekStyle::HereAndAbove>
      NOD() TAny<Trait> GatherTraits(TMeta);
      template<SeekStyle = SeekStyle::HereAndAbove>
      NOD() TAny<Trait> GatherTraits(TMeta) const;
      template<CT::Trait T = Trait, SeekStyle = SeekStyle::HereAndAbove>
      NOD() TAny<Trait> GatherTraits();
      template<CT::Trait T = Trait, SeekStyle = SeekStyle::HereAndAbove>
      NOD() TAny<Trait> GatherTraits() const;

      template<SeekStyle = SeekStyle::HereAndAbove, CT::Data D>
      NOD() TAny<D> GatherValues() const;
   };

} // namespace Langulus::Entity


namespace Langulus::CT
{

   template<class T>
   concept Pinnable = requires { Decay<T>::CTTI_Pinnable; };

} // namespace Langulus::CT