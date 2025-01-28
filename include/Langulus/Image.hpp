///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Asset.hpp"
#include <Langulus/Math/Color.hpp>
#include <Langulus/Math/Scale.hpp>

LANGULUS_DEFINE_TRAIT(Image, "Image unit");
LANGULUS_EXCEPTION(Image);


namespace Langulus
{

   ///                                                                        
   ///   Universal pixel buffer view                                          
   ///                                                                        
   struct ImageView {
      uint32_t mWidth  = 1;
      uint32_t mHeight = 1;
      uint32_t mDepth  = 1;
      uint32_t mFrames = 1;
      DMeta mFormat {};
      // Reverse RGBA to BGRA                                           
      // This is not a scalable solution and would eventually fail      
      bool mReverseFormat {};

      bool operator == (const ImageView&) const noexcept;

      constexpr uint32_t GetPixelCount() const noexcept;
      constexpr uint32_t GetDimensionCount() const noexcept;
      Size GetPixelBytesize() const noexcept;
      Size GetBytesize() const noexcept;
      auto GetChannelCount() const noexcept -> uint32_t;
      Hash GetHash() const noexcept;
      auto GetScale() const noexcept -> Math::Scale3;
   };
   
} // namespace Langulus

namespace Langulus::A
{

   ///                                                                        
   ///   Abstract texture content                                             
   ///                                                                        
   struct Image : Asset {
   protected:
      ImageView mView;

   public:
      LANGULUS_BASES(Asset);
      Image() : Resolvable {this} {}

      virtual auto GetLOD(const Math::LOD&) const -> Ref<Image> { return {}; }
      virtual auto GetGPUHandle() const noexcept -> void* { return nullptr; }

      auto GetFormat() const noexcept -> DMeta;
      auto GetView() const noexcept -> ImageView const&;
      auto GetView() noexcept -> ImageView&;

      template<class F>
      auto ForEachPixel(F&&) const;

      template<template<class> class S, CT::Block B> requires CT::Intent<S<B>>
      void Upload(S<B>&&);

      ///                                                                     
      ///   Iteration                                                         
      ///                                                                     
      template<bool MUTABLE>
      struct Iterator;

      auto begin()       noexcept -> Iterator<true>;
      auto begin() const noexcept -> Iterator<false>;
      auto end()   const noexcept -> A::IteratorEnd { return {}; }
   };
  

   ///                                                                        
   /// Image iterator                                                         
   ///                                                                        
   template<bool MUTABLE>
   struct Image::Iterator : A::Iterator {
      static constexpr bool Mutable = MUTABLE;
      LANGULUS(ABSTRACT) false;

   protected:
      friend struct Image;

      // Current iterator position pointer                              
      Byte const* mValue;
      // Iterator position which is considered the 'end' iterator       
      Byte const* mEnd;
      // Iterator position which is considered the 'end' iterator       
      Image* mImage;

      constexpr Iterator(Image*, Byte const* it, Byte const* end) noexcept;

   public:
      Iterator() noexcept = delete;
      constexpr Iterator(const Iterator&) noexcept = default;
      constexpr Iterator(Iterator&&) noexcept = default;
      constexpr Iterator(const A::IteratorEnd&) noexcept;

      constexpr auto operator = (const Iterator&) noexcept -> Iterator& = default;
      constexpr auto operator = (Iterator&&) noexcept -> Iterator& = default;

      constexpr bool operator == (const Iterator&) const noexcept;
      constexpr bool operator == (const A::IteratorEnd&) const noexcept;

      // Prefix operator                                                
      constexpr auto operator ++ () noexcept -> Iterator&;

      // Suffix operator                                                
      constexpr auto operator ++ (int) noexcept -> Iterator;

      constexpr explicit operator bool() const noexcept;
      constexpr operator Iterator<false>() const noexcept requires Mutable;

      template<CT::ColorBased T>
      T As() const;
   };


   ///                                                                        
   ///   Abstract font content                                                
   ///                                                                        
   struct Font : Image {
      LANGULUS_BASES(Image);

      Font()
         : Resolvable   {this}
         , ProducedFrom {nullptr, {}} {}
   };

} // namespace Langulus::A

namespace Langulus::CT
{

   /// A concept for any kind of image content unit                           
   template<class T>
   concept Image = DerivedFrom<T, A::Image>;

} // namespace Langulus::CT

#include "Image.inl"