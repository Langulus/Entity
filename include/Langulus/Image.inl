///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Image.hpp"


namespace Langulus
{
   
   /// Compare texture views                                                  
   ///   @param rhs - the texture view to compare against                     
   ///   @return true if both views are the same                              
   LANGULUS(INLINED)
   bool ImageView::operator == (const ImageView& rhs) const noexcept {
      return mWidth  == rhs.mWidth
         and mHeight == rhs.mHeight
         and mDepth  == rhs.mDepth
         and mFrames == rhs.mFrames
         and mFormat == rhs.mFormat;
   }

   /// Get the number of pixels in the texture                                
   ///   @return the number of pixels                                         
   LANGULUS(INLINED)
   constexpr uint32_t ImageView::GetPixelCount() const noexcept {
      return mWidth * mHeight * mDepth * mFrames;
   }

   /// Get number of dimensions used for the image                            
   ///   @return return up to 4 dimensions                                    
   LANGULUS(INLINED)
   constexpr uint32_t ImageView::GetDimensionCount() const noexcept {
      return (mWidth > 1) + (mHeight > 1) + (mDepth > 1) + (mFrames > 1);
   }

   /// Get the size in bytes for a single pixel                               
   ///   @return the size in bytes                                            
   LANGULUS(INLINED)
   Size ImageView::GetPixelBytesize() const noexcept {
      return mFormat ? mFormat->mSize : Size {0};
   }

   /// Get the bytesize of the entire image across all dimensions             
   ///   @return the bytesize of the entire image                             
   LANGULUS(INLINED)
   Size ImageView::GetBytesize() const noexcept {
      return GetPixelCount() * GetPixelBytesize();
   }

   /// Get the number of channels inside the color format                     
   ///   @return the number of channels                                       
   LANGULUS(INLINED)
   uint32_t ImageView::GetChannelCount() const noexcept {
      return static_cast<uint32_t>(mFormat->GetMemberCount());
   }
   
   /// Get the hash of a normalized descriptor (cached)                       
   ///   @return the hash                                                     
   LANGULUS(INLINED)
   Hash ImageView::GetHash() const noexcept {
      return HashOf(
         mWidth, mHeight, mDepth, mFrames,
         mFormat, mReverseFormat
      );
   }

   /// Get the scale of the image, represented by real numbers                
   ///   @return the scale                                                    
   LANGULUS(INLINED)
   Math::Scale3 ImageView::GetScale() const noexcept {
      return {mWidth, mHeight, mDepth};
   }

} // namespace Langulus

namespace Langulus::A
{
   
   /// Get the pixel format of the texture                                    
   ///   @return the pixel format type                                        
   LANGULUS(INLINED)
   DMeta Image::GetFormat() const noexcept {
      return mView.mFormat;
   }

   /// Get the texture view (const)                                           
   ///   @return the texture view                                             
   LANGULUS(INLINED)
   const ImageView& Image::GetView() const noexcept {
      return mView;
   }
   
   /// Get the texture view                                                   
   ///   @return the texture view                                             
   LANGULUS(INLINED)
   ImageView& Image::GetView() noexcept {
      return mView;
   }

   /// Execute a function for each pixel, represented by a desired type       
   ///   @tparam F - the function signature (deducible)                       
   ///   @param call - the function to execute for each pixel                 
   ///   @return the number of pixels that were iterated                      
   template<class F>
   auto Image::ForEachPixel(F&& call) const {
      auto pixels = GetData<Traits::Color>();

      LANGULUS_ASSUME(DevAssumes, pixels and *pixels,
         "No color data", " in ", Self());
      LANGULUS_ASSUME(DevAssumes, pixels->IsDense(),
         "Image data is not dense", " for ", Self());
      LANGULUS_ASSUME(DevAssumes,
         pixels->CastsTo<Bytes>() or pixels->CastsTo<A::Color>(),
         "Image doesn't contain pixel data", 
         " - contains ", pixels->GetType(), " instead");

      using A = Deref<ArgumentOf<F>>;
      using R = ReturnOf<F>;

      static_assert(CT::Constant<A>, "Color iterator must be constant");
      static_assert(CT::Dense<A>, "Color iterator must be dense");

      LANGULUS_ASSUME(DevAssumes, mView.mFormat->IsSimilar<A>(),
         "Iterator type is not compatible with contained color data");

      // Iterate using the desired color type                           
      UNUSED() Count counter = 0;
      auto data = pixels->CastsTo<Bytes>()
         ? pixels->Get<Bytes>().GetRaw<A>()
         : pixels->GetRaw<A>();
      const auto dataEnd = data + mView.GetPixelCount();

      while (data != dataEnd) {
         if constexpr (CT::Bool<R>) {
            if (not call(*data))
               return counter;
            ++counter;
         }
         else call(*data);

         ++data;
      }

      if constexpr (CT::Bool<R>)
         return counter;

      //TODO map other kinds of pixel iterators?
   }

   /// Upload raw data to the image by a semantic                             
   ///   @attention deletes any data previously commited                      
   ///   @param data - the block of data                                      
   template<template<class> class S, CT::Block B>
   requires CT::Semantic<S<B>> LANGULUS(INLINED)
   void Image::Upload(S<B>&& data) const {
      // Check if provided data matches the view requirements           
      LANGULUS_ASSERT(mView.GetBytesize() == data->GetBytesize(), Image,
         "Data is of the wrong size");

      // Reset data and commit the new one                              
      mDataListMap.Reset();
      Commit<Traits::Color>(data.Forward());
   }

   LANGULUS(INLINED)
   Image::Iterator<true> Image::begin() noexcept {
      return {
         this,
         reinterpret_cast<const Byte*>(GetData<Traits::Color>()->GetRaw()),
         reinterpret_cast<const Byte*>(GetData<Traits::Color>()->GetRawEnd())
      };
   }

   LANGULUS(INLINED)
   Image::Iterator<false> Image::begin() const noexcept {
      return {
         const_cast<Image*>(this),
         reinterpret_cast<const Byte*>(GetData<Traits::Color>()->GetRaw()),
         reinterpret_cast<const Byte*>(GetData<Traits::Color>()->GetRawEnd())
      };
   }

   /// Construct an iterator                                                  
   ///   @param start - the current iterator position                         
   ///   @param end - the ending marker                                       
   template<bool M> LANGULUS(INLINED)
   constexpr Image::Iterator<M>::Iterator(Image* image, Byte const* it, Byte const* end) noexcept
      : mValue {it}
      , mEnd {end}
      , mImage {image} {}

   /// Construct an end iterator                                              
   template<bool M> LANGULUS(INLINED)
   constexpr Image::Iterator<M>::Iterator(const A::IteratorEnd&) noexcept
      : mValue {nullptr}
      , mEnd {nullptr}
      , mImage {nullptr} {}

   /// Compare two iterators                                                  
   ///   @param rhs - the other iterator                                      
   ///   @return true if iterators point to the same element                  
   template<bool M> LANGULUS(INLINED)
   constexpr bool Image::Iterator<M>::operator == (const Iterator& rhs) const noexcept {
      return mValue == rhs.mValue;
   }

   /// Compare iterator with an end marker                                    
   ///   @param rhs - the end iterator                                        
   ///   @return true element is at or beyond the end marker                  
   template<bool M> LANGULUS(INLINED)
   constexpr bool Image::Iterator<M>::operator == (const A::IteratorEnd&) const noexcept {
      return mValue >= mEnd;
   }

   /// Prefix increment operator                                              
   ///   @attention assumes iterator points to a valid element                
   ///   @return the modified iterator                                        
   template<bool M> LANGULUS(INLINED)
   constexpr Image::Iterator<M>& Image::Iterator<M>::operator ++ () noexcept {
      mValue += mImage->GetView().GetPixelBytesize();
      return *this;
   }

   /// Suffix increment operator                                              
   ///   @attention assumes iterator points to a valid element                
   ///   @return the previous value of the iterator                           
   template<bool M> LANGULUS(INLINED)
   constexpr Image::Iterator<M> Image::Iterator<M>::operator ++ (int) noexcept {
      const auto backup = *this;
      operator ++ ();
      return backup;
   }

   /// Check if iterator is valid                                             
   template<bool M> LANGULUS(INLINED)
   constexpr Image::Iterator<M>::operator bool() const noexcept {
      return *this != A::IteratorEnd {};
   }

   /// Interpret the pixel as a desired color format                          
   template<bool M> template<CT::ColorBased T>
   T Image::Iterator<M>::As() const {
      const auto from = mImage->GetView().mFormat;
      LANGULUS_ASSUME(DevAssumes, from, "No color type");
      const auto fromRed = from->GetMember(MetaOf<Traits::R>());
      const auto fromGre = from->GetMember(MetaOf<Traits::G>());
      const auto fromBlu = from->GetMember(MetaOf<Traits::B>());
      const auto fromAlf = from->GetMember(MetaOf<Traits::A>());
      const RTTI::Member* fromAny =
         fromRed ? fromRed :
         fromGre ? fromGre :
         fromBlu ? fromBlu :
         fromAlf ? fromAlf :
         nullptr;

      LANGULUS_ASSUME(DevAssumes, fromAny, "Supposed color type doesn't have "
         "a recognizable channel member");

      // Get the values (and normalize them if we have to)              
      // Missing RGB channels are defaulted to 0, while missing Alpha   
      // channel is defaulted to 255 (or 1, if normalized)              
      const Anyness::Block<> pixel {{}, from, 1, mValue, nullptr};
      T result;
      using TT = TypeOf<T>;

      if constexpr (requires { result.r; }) {
         if (fromRed)
            result.r = pixel.GetMember(*fromRed, 0).AsCast<TT>();
         else
            result.r = TT {0};
      }

      if constexpr (requires { result.g; }) {
         if (fromGre)
            result.g = pixel.GetMember(*fromGre, 0).AsCast<TT>();
         else
            result.g = TT {0};
      }

      if constexpr (requires { result.b; }) {
         if (fromBlu)
            result.b = pixel.GetMember(*fromBlu, 0).AsCast<TT>();
         else
            result.b = TT {0};
      }

      if constexpr (requires { result.a; }) {
         if (fromAlf)
            result.a = pixel.GetMember(*fromAlf, 0).AsCast<TT>();
         else if constexpr (CT::Integer<TT>)
            result.a = std::numeric_limits<TT>::max();
         else
            result.a = TT {1};
      }

      if constexpr (not CT::Integer<TT>) {
         if (fromAny->GetType()->CastsTo<A::Integer>()) {
            // We have to normalize the color                           
            if (fromAny->GetType()->CastsTo<A::Unsigned>()) {
               // Unsigned normalization                                
               switch (fromAny->GetType()->mSize) {
               case 1:
                  result /= TT {255};
                  break;
               case 2:
                  result /= TT {65535};
                  break;
               case 4:
                  result /= TT {16777215};
                  break;
               default:
                  LANGULUS_OOPS(Image, "Unhandled channel size");
               }
            }
            else LANGULUS_OOPS(Image, "Unhandled channel format");
         }
      }

      return result;
   }

} // namespace Langulus::A
