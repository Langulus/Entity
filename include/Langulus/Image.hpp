///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Asset.hpp>

LANGULUS_DEFINE_TRAIT(Image, "Image unit");
LANGULUS_EXCEPTION(Image);


namespace Langulus
{

   using namespace Langulus::Flow;
   using namespace Langulus::Anyness;
   using namespace Langulus::Entity;
   using namespace Langulus::Math;


   ///                                                                        
   ///   Universal pixel buffer view                                          
   ///                                                                        
   struct ImageView {
      uint32_t mWidth {1};
      uint32_t mHeight {1};
      uint32_t mDepth {1};
      uint32_t mFrames {1};
      DMeta mFormat {};
      // Reverse RGBA to BGRA                                           
      // This is not a scalable solution and would eventually fail      
      bool mReverseFormat {};

      bool operator == (const ImageView&) const noexcept;

      NOD() constexpr uint32_t GetPixelCount() const noexcept;
      NOD() constexpr uint32_t GetDimensionCount() const noexcept;
      NOD() Size GetPixelBytesize() const noexcept;
      NOD() Size GetBytesize() const noexcept;
      NOD() uint32_t GetChannelCount() const noexcept;
      NOD() Hash GetHash() const noexcept;
   };
   
} // namespace Langulus

namespace Langulus::A
{

   ///                                                                        
   ///   Abstract texture content                                             
   ///                                                                        
   struct Image : Asset {
      LANGULUS_BASES(Asset);
      using Asset::Asset;

   protected:
      ImageView mView;

   public:
      NOD() virtual Ref<Image> GetLOD(const LOD&) const = 0;
      NOD() virtual void* GetGPUHandle() const noexcept = 0;

      NOD() DMeta GetFormat() const noexcept;
      NOD() ImageView const& GetView() const noexcept;
      NOD() ImageView& GetView() noexcept;

      template<class F>
      auto ForEachPixel(F&&) const;

      template<template<class> class S, CT::Block B>
      requires CT::Semantic<S<B>>
      void Upload(S<B>&&);

      ///                                                                     
      ///   Iteration                                                         
      ///                                                                     
      template<bool MUTABLE>
      struct Iterator;

      NOD() constexpr Iterator<true>  begin() noexcept;
      NOD() constexpr Iterator<false> begin() const noexcept;
      NOD() constexpr Iterator<true>  last() noexcept;
      NOD() constexpr Iterator<false> last() const noexcept;
      constexpr A::IteratorEnd        end() const noexcept { return {}; }
   };
   
   ///                                                                        
   ///   Abstract font content                                                
   ///                                                                        
   struct Font : Image {
      LANGULUS_BASES(Image);
      using Image::Image;
   };

} // namespace Langulus::A

namespace Langulus::CT
{

   /// A concept for any kind of image content unit                           
   template<class T>
   concept Image = DerivedFrom<T, A::Image>;

} // namespace Langulus::CT
