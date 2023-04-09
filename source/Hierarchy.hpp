///                                                                           
/// Langulus::Entity                                                          
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Module.hpp"

namespace Langulus::Entity
{

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
      LANGULUS(TYPED) T;
      LANGULUS_BASES(T);
      LANGULUS_PROPERTIES_START(Pinnable)
         LANGULUS_PROPERTY_TRAIT(mLocked, State),
      LANGULUS_PROPERTIES_END();

   public:
      static constexpr bool CTTI_Pinnable = true;

      Pinnable() = default;
      Pinnable(const Pinnable&) = default;
      Pinnable(Pinnable&&) noexcept = default;
      Pinnable(const T&) requires (CT::CopyMakable<T>);
      Pinnable(T&&) noexcept requires (CT::MoveMakable<T>);

      Pinnable& operator = (const Pinnable&) = default;
      Pinnable& operator = (Pinnable&&) noexcept = default;
      Pinnable& operator = (const T&) requires (CT::CopyAssignable<T>);
      Pinnable& operator = (T&&) noexcept requires (CT::MoveAssignable<T>);

      void Pin() noexcept;
      void Unpin() noexcept;

      NOD() bool IsPinned() const noexcept;
   };


   /// Seek interface for Hierarchy/Thing/Unit                                
   #define LANGULUS_SEEK_INTERFACE() \
      template<Seek = Seek::HereAndAbove> \
      NOD() Unit* SeekUnit(DMeta, Index = IndexFirst); \
      template<Seek = Seek::HereAndAbove> \
      NOD() const Unit* SeekUnit(DMeta, Index = IndexFirst) const; \
      template<Seek = Seek::HereAndAbove> \
      NOD() Unit* SeekUnitAux(const Any&, DMeta, Index = IndexFirst); \
      template<Seek = Seek::HereAndAbove> \
      NOD() const Unit* SeekUnitAux(const Any&, DMeta, Index = IndexFirst) const; \
      \
      template<Seek = Seek::HereAndAbove> \
      NOD() Unit* SeekUnitExt(const Construct&, Index = IndexFirst); \
      template<Seek = Seek::HereAndAbove> \
      NOD() const Unit* SeekUnitExt(const Construct&, Index = IndexFirst) const; \
      template<Seek = Seek::HereAndAbove> \
      NOD() Unit* SeekUnitAuxExt(const Any&, const Construct&, Index = IndexFirst); \
      template<Seek = Seek::HereAndAbove> \
      NOD() const Unit* SeekUnitAuxExt(const Any&, const Construct&, Index = IndexFirst) const; \
      \
      template<Seek = Seek::HereAndAbove> \
      NOD() Trait SeekTrait(TMeta, Index = IndexFirst); \
      template<Seek = Seek::HereAndAbove> \
      NOD() Trait SeekTrait(TMeta, Index = IndexFirst) const; \
      template<Seek = Seek::HereAndAbove, CT::Data D> \
      NOD() bool SeekTrait(TMeta, D&, Index = IndexFirst) const; \
      \
      template<Seek = Seek::HereAndAbove> \
      NOD() Trait SeekTraitAux(const Any&, TMeta, Index = IndexFirst) const; \
      template<Seek = Seek::HereAndAbove, CT::Data D> \
      NOD() bool SeekTraitAux(const Any&, TMeta, D&, Index = IndexFirst) const; \
      \
      template<Seek = Seek::HereAndAbove, CT::Data D> \
      bool SeekValue(D&, Index = IndexFirst) const; \
      template<Seek = Seek::HereAndAbove, CT::Data D> \
      bool SeekValueAux(const Any&, D&, Index = IndexFirst) const; \
      \
      template<CT::Data T, Seek SEEK = Seek::HereAndAbove> \
      NOD() LANGULUS(INLINED) Decay<T>* SeekUnit(Index offset = IndexFirst) { \
         return static_cast<Decay<T>*>( \
            SeekUnit<SEEK>(MetaData::Of<Decay<T>>(), offset) \
         ); \
      } \
      template<CT::Data T, Seek SEEK = Seek::HereAndAbove> \
      NOD() LANGULUS(INLINED) const Decay<T>* SeekUnit(Index offset = IndexFirst) const { \
         return static_cast<const Decay<T>*>( \
            SeekUnit<SEEK>(MetaData::Of<Decay<T>>(), offset) \
         ); \
      } \
      template<CT::Data T, Seek SEEK = Seek::HereAndAbove> \
      NOD() LANGULUS(INLINED) Decay<T>* SeekUnitAux(const Any& aux, Index offset = IndexFirst) { \
         return static_cast<Decay<T>*>( \
            SeekUnitAux<SEEK>(aux, MetaData::Of<Decay<T>>(), offset) \
         ); \
      } \
      template<CT::Data T, Seek SEEK = Seek::HereAndAbove> \
      NOD() LANGULUS(INLINED) const Decay<T>* SeekUnitAux(const Any& aux, Index offset = IndexFirst) const { \
         return static_cast<const Decay<T>*>( \
            SeekUnitAux<SEEK>(aux, MetaData::Of<Decay<T>>(), offset) \
         ); \
      } \
      \
      template<CT::Data T, Seek SEEK = Seek::HereAndAbove> \
      NOD() LANGULUS(INLINED) Decay<T>* SeekUnitAuxExt(const Any& aux, Index offset = IndexFirst) { \
         return static_cast<Decay<T>*>( \
            SeekUnitAuxExt<SEEK>(aux, Construct::From<T>(), offset) \
         ); \
      } \
      template<CT::Data T, Seek SEEK = Seek::HereAndAbove> \
      NOD() LANGULUS(INLINED) const Decay<T>* SeekUnitAuxExt(const Any& aux, Index offset = IndexFirst) const { \
         return static_cast<const Decay<T>*>( \
            SeekUnitAuxExt<SEEK>(aux, Construct::From<T>(), offset) \
         ); \
      } \
      template<CT::Data T, Seek SEEK = Seek::HereAndAbove> \
      NOD() LANGULUS(INLINED) Decay<T>* SeekUnitAuxExt(const Any& aux, const Any& construct, Index offset = IndexFirst) { \
         return static_cast<Decay<T>*>( \
            SeekUnitAuxExt<SEEK>(aux, Construct::From<T>(construct), offset) \
         ); \
      } \
      template<CT::Data T, Seek SEEK = Seek::HereAndAbove> \
      NOD() LANGULUS(INLINED) const Decay<T>* SeekUnitAuxExt(const Any& aux, const Any& construct, Index offset = IndexFirst) const { \
         return static_cast<const Decay<T>*>( \
            SeekUnitAuxExt<SEEK>(aux, Construct::From<T>(construct), offset) \
         ); \
      } \
      \
      template<CT::Trait T = Trait, Seek SEEK = Seek::HereAndAbove> \
      NOD() LANGULUS(INLINED) Trait SeekTrait(Index offset = IndexFirst) { \
         return SeekTrait<SEEK>(MetaTrait::Of<T>(), offset); \
      } \
      template<CT::Trait T = Trait, Seek SEEK = Seek::HereAndAbove> \
      NOD() LANGULUS(INLINED) Trait SeekTrait(Index offset = IndexFirst) const { \
         return SeekTrait<SEEK>(MetaTrait::Of<T>(), offset); \
      } \
      template<CT::Trait T = Trait, Seek SEEK = Seek::HereAndAbove, CT::Data D> \
      LANGULUS(INLINED) bool SeekTrait(D& output, Index offset = IndexFirst) const { \
         return SeekTrait<SEEK, D>(MetaTrait::Of<T>(), output, offset); \
      } \
      \
      template<CT::Trait T = Trait, Seek SEEK = Seek::HereAndAbove> \
      NOD() LANGULUS(INLINED) Trait SeekTraitAux(const Any& aux, Index offset = IndexFirst) const { \
         return SeekTraitAux<SEEK>(aux, MetaTrait::Of<T>(), offset); \
      } \
      template<CT::Trait T = Trait, Seek SEEK = Seek::HereAndAbove, CT::Data D> \
      LANGULUS(INLINED) bool SeekTraitAux(const Any& aux, D& output, Index offset = IndexFirst) const { \
         return SeekTraitAux<SEEK, D>(aux, MetaTrait::Of<T>(), output, offset); \
      }

   #if LANGULUS_FEATURE(MANAGED_MEMORY)
      #define LANGULUS_SEEK_TOKEN_INTERFACE() \
         template<Seek SEEK = Seek::HereAndAbove> \
         NOD() LANGULUS(INLINED) Unit* SeekUnit(const Token& token, Index offset = IndexFirst) { \
            return SeekUnit<SEEK>(RTTI::Database.GetMetaData(token), offset); \
         } \
         template<Seek SEEK = Seek::HereAndAbove> \
         NOD() LANGULUS(INLINED) const Unit* SeekUnit(const Token& token, Index offset = IndexFirst) const { \
            return SeekUnit<SEEK>(RTTI::Database.GetMetaData(token), offset); \
         } \
         template<Seek SEEK = Seek::HereAndAbove> \
         NOD() LANGULUS(INLINED) Unit* SeekUnitAux(const Any& aux, const Token& token, Index offset = IndexFirst) { \
            return SeekUnitAux<SEEK>(aux, RTTI::Database.GetMetaData(token), offset); \
         } \
         template<Seek SEEK = Seek::HereAndAbove> \
         NOD() LANGULUS(INLINED) const Unit* SeekUnitAux(const Any& aux, const Token& token, Index offset = IndexFirst) const { \
            return SeekUnitAux<SEEK>(aux, RTTI::Database.GetMetaData(token), offset); \
         } \
         \
         template<Seek SEEK = Seek::HereAndAbove> \
         NOD() LANGULUS(INLINED) Trait SeekTrait(const Token& token, Index offset = IndexFirst) { \
            return SeekTrait<SEEK>(RTTI::Database.GetMetaTrait(token), offset); \
         } \
         template<Seek SEEK = Seek::HereAndAbove> \
         NOD() LANGULUS(INLINED) Trait SeekTrait(const Token& token, Index offset = IndexFirst) const { \
            return SeekTrait<SEEK>(RTTI::Database.GetMetaTrait(token), offset); \
         } \
         template<Seek SEEK = Seek::HereAndAbove, CT::Data D> \
         LANGULUS(INLINED) bool SeekTrait(const Token& token, D& output, Index offset = IndexFirst) const { \
            return SeekTrait<SEEK>(RTTI::Database.GetMetaTrait(token), output, offset); \
         } \
         \
         template<Seek SEEK = Seek::HereAndAbove> \
         NOD() LANGULUS(INLINED) Trait SeekTraitAux(const Any& aux, const Token& token, Index offset = IndexFirst) const { \
            return SeekTraitAux<SEEK>(aux, RTTI::Database.GetMetaTrait(token), offset); \
         } \
         template<Seek SEEK = Seek::HereAndAbove, CT::Data D> \
         LANGULUS(INLINED) bool SeekTraitAux(const Any& aux, const Token& token, D& output, Index offset = IndexFirst) const { \
            return SeekTraitAux<SEEK>(aux, RTTI::Database.GetMetaTrait(token), output, offset); \
         }
   #else
      #define LANGULUS_SEEK_TOKEN_INTERFACE()
   #endif

   /// Gather interface for Hierarchy/Thing/Unit                              
   #define LANGULUS_GATHER_INTERFACE() \
      template<Seek = Seek::HereAndAbove> \
      NOD() TAny<Unit*> GatherUnits(const Construct&); \
      template<Seek = Seek::HereAndAbove> \
      NOD() TAny<const Unit*> GatherUnits(const Construct&) const; \
      template<Seek = Seek::HereAndAbove> \
      NOD() TAny<Unit*> GatherUnits(DMeta); \
      template<Seek = Seek::HereAndAbove> \
      NOD() TAny<const Unit*> GatherUnits(DMeta) const; \
      \
      template<Seek = Seek::HereAndAbove> \
      NOD() TAny<Trait> GatherTraits(TMeta); \
      template<Seek = Seek::HereAndAbove> \
      NOD() TAny<Trait> GatherTraits(TMeta) const; \
      \
      template<Seek = Seek::HereAndAbove, CT::Data D> \
      NOD() TAny<D> GatherValues() const; \
      \
      template<CT::Data T = Unit, Seek SEEK = Seek::HereAndAbove> \
      NOD() LANGULUS(INLINED) TAny<Decay<T>*> GatherUnits() { \
         return GatherUnits<SEEK>(MetaData::Of<Decay<T>>()); \
      } \
      template<CT::Data T = Unit, Seek SEEK = Seek::HereAndAbove> \
      NOD() LANGULUS(INLINED) TAny<const Decay<T>*> GatherUnits() const { \
         return GatherUnits<SEEK>(MetaData::Of<Decay<T>>()); \
      } \
      template<CT::Trait T = Trait, Seek SEEK = Seek::HereAndAbove> \
      NOD() LANGULUS(INLINED) TAny<Trait> GatherTraits() { \
         return GatherTraits<SEEK>(MetaTrait::Of<T>()); \
      } \
      template<CT::Trait T = Trait, Seek SEEK = Seek::HereAndAbove> \
      NOD() LANGULUS(INLINED) TAny<Trait> GatherTraits() const { \
         return GatherTraits<SEEK>(MetaTrait::Of<T>()); \
      }

   #if LANGULUS_FEATURE(MANAGED_MEMORY)
      #define LANGULUS_GATHER_TOKEN_INTERFACE() \
         template<Seek SEEK = Seek::HereAndAbove> \
         NOD() LANGULUS(INLINED) \
         TAny<Unit*> GatherUnits(const Token& token) { \
            return GatherUnits<SEEK>(RTTI::Database.GetMetaData(token)); \
         } \
         template<Seek SEEK = Seek::HereAndAbove> \
         NOD() LANGULUS(INLINED) \
         TAny<const Unit*> GatherUnits(const Token& token) const { \
            return GatherUnits<SEEK>(RTTI::Database.GetMetaData(token)); \
         } \
         \
         template<Seek SEEK = Seek::HereAndAbove> \
         NOD() LANGULUS(INLINED) \
         TAny<Trait> GatherTraits(const Token& token) { \
            return GatherTraits<SEEK>(RTTI::Database.GetMetaTrait(token)); \
         } \
         template<Seek SEEK = Seek::HereAndAbove> \
         NOD() LANGULUS(INLINED) \
         TAny<Trait> GatherTraits(const Token& token) const { \
            return GatherTraits<SEEK>(RTTI::Database.GetMetaTrait(token)); \
         }
   #else
      #define LANGULUS_GATHER_TOKEN_INTERFACE()
   #endif


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
      LANGULUS_SEEK_INTERFACE();
      LANGULUS_SEEK_TOKEN_INTERFACE();

      ///                                                                     
      ///   Gather                                                            
      ///                                                                     
      LANGULUS_GATHER_INTERFACE();
      LANGULUS_GATHER_TOKEN_INTERFACE();
   };

} // namespace Langulus::Entity


namespace Langulus::CT
{

   template<class T>
   concept Pinnable = requires { Decay<T>::CTTI_Pinnable; };

} // namespace Langulus::CT