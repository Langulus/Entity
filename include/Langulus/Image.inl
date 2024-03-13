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
      return mWidth == rhs.mWidth
         and mHeight == rhs.mHeight
         and mDepth == rhs.mDepth
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
         ? pixels->Get<Bytes>().GetRawAs<A>()
         : pixels->GetRawAs<A>();
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
   void Image::Upload(S<B>&& data) {
      // Check if provided data matches the view requirements           
      LANGULUS_ASSERT(mView.GetBytesize() == data->GetBytesize(), Image,
         "Data is of the wrong size");

      // Reset data and commit the new one                              
      mDataListMap.Reset();
      Commit<Traits::Color>(data.Forward());
   }

} // namespace Langulus::A
