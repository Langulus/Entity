///                                                                           
/// Langulus::Entity                                                          
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "External.hpp"

namespace Langulus
{

   ///                                                                        
   ///   Vertex/index buffer view                                             
   ///                                                                        
   
   /// Compare geometry views                                                 
   ///   @param rhs - the geometry view to compare against                    
   ///   @return true if both views are the same                              
   LANGULUS(INLINED)
   bool GeometryView::operator == (const GeometryView& rhs) const noexcept {
      return
         mPrimitiveCount == rhs.mPrimitiveCount &&
         mPrimitiveStart == rhs.mPrimitiveStart &&
         mIndexCount == rhs.mIndexCount &&
         mIndexStart == rhs.mIndexStart &&
         (mPrimitiveType == rhs.mPrimitiveType ||
            (mPrimitiveType && mPrimitiveType->IsExact(rhs.mPrimitiveType))) &&
         mBilateral == rhs.mBilateral;
   }

   /// Decay the geometry view to a simpler primitive type, for example,      
   /// decay triangles to points                                              
   ///   @return the decayed view                                             
   LANGULUS(INLINED)
   GeometryView GeometryView::Decay() const {
      TODO();
   }


   ///                                                                        
   ///   Universal pixel buffer view                                          
   ///                                                                        
   
   /// Compare texture views                                                  
   ///   @param rhs - the texture view to compare against                     
   ///   @return true if both views are the same                              
   LANGULUS(INLINED)
   bool TextureView::operator == (const TextureView& rhs) const noexcept {
      return
         mWidth == rhs.mWidth &&
         mHeight == rhs.mHeight &&
         mDepth == rhs.mDepth &&
         mFrames == rhs.mFrames &&
         (mFormat == rhs.mFormat || (mFormat && mFormat->IsExact(rhs.mFormat)));
   }

   /// Get the number of pixels in the texture                                
   ///   @return the number of pixels                                         
   LANGULUS(INLINED)
   constexpr uint32_t TextureView::GetPixelCount() const noexcept {
      return mWidth * mHeight * mDepth * mFrames;
   }

   /// Get number of dimensions used for the image                            
   ///   @return return up to 4 dimensions                                    
   LANGULUS(INLINED)
   constexpr uint32_t TextureView::GetDimensionCount() const noexcept {
      return (mWidth > 1) + (mHeight > 1) + (mDepth > 1) + (mFrames > 1);
   }

   /// Get the size in bytes for a single pixel                               
   ///   @return the size in bytes                                            
   LANGULUS(INLINED)
   Size TextureView::GetPixelBytesize() const noexcept {
      return mFormat ? mFormat->mSize : 0;
   }

   /// Get the bytesize of the entire image across all dimensions             
   ///   @return the bytesize of the entire image                             
   LANGULUS(INLINED)
   Size TextureView::GetBytesize() const noexcept {
      return GetPixelCount() * GetPixelBytesize();
   }

   /// Get the number of channels inside the color format                     
   ///   @return the number of channels                                       
   LANGULUS(INLINED)
   uint32_t TextureView::GetChannelCount() const noexcept {
      return static_cast<uint32_t>(mFormat->GetMemberCount());
   }

} // namespace Langulus

namespace Langulus::A
{

   ///                                                                        
   ///   Abstract file interface                                              
   ///                                                                        
   template<class T>
   LANGULUS(INLINED)
   T File::ReadAs() const {
      return ReadAs(RTTI::MetaData::Of<T>()).template As<T>();
   }


   ///                                                                        
   ///   Abstract content unit                                                
   ///                                                                        
   
   /// Construct abstract content                                             
   ///   @param type - concrete type of the content                           
   ///   @param producer - the content producer                               
   ///   @param descriptor - messy descriptor for the content                 
   LANGULUS(INLINED)
   Content::Content(
      Anyness::DMeta type,
      ContentModule* producer,
      const Anyness::Any& descriptor)
      : Unit {type, descriptor}
      , ProducedFrom {producer, descriptor} {}

   /// Get the entire content data map                                        
   ///   @return a reference to the contents                                  
   LANGULUS(INLINED)
   const Content::DataListMap& Content::GetDataListMap() const noexcept {
      return mDataListMap;
   }

   /// Get a single data entry from the contents                              
   ///   @tparam T - the trait to search for                                  
   ///   @param index - the Nth data associated to the trait                  
   ///   @return a pointer to the data entry, or nullptr if none exists       
   template<CT::Trait T>
   LANGULUS(INLINED)
   const Content::Data* Content::GetData(Offset index) const noexcept {
      return GetData(RTTI::MetaTrait::Of<T>(), index);
   }

   /// Get a single data entry from the contents                              
   ///   @param trait - the trait to search for                               
   ///   @param index - the Nth data associated to the trait                  
   ///   @return a pointer to the data entry, or nullptr if none exists       
   LANGULUS(INLINED)
   const Content::Data* Content::GetData(Anyness::TMeta trait, Offset index) const noexcept {
      const auto datalist = GetDataList(trait);
      return datalist && datalist->GetCount() > index
         ? &(*datalist)[index]
         : nullptr;
   }

   /// Get a data list from the contents                                      
   ///   @tparam T - the trait to search for                                  
   ///   @return a pointer to the data list, or nullptr if none exists        
   template<CT::Trait T>
   LANGULUS(INLINED)
   const Content::DataList* Content::GetDataList() const noexcept {
      return GetDataList(RTTI::MetaTrait::Of<T>());
   }

   /// Get a data list from the contents                                      
   ///   @param trait - the trait to search for                               
   ///   @return a pointer to the data list, or nullptr if none exists        
   LANGULUS(INLINED)
   const Content::DataList* Content::GetDataList(Anyness::TMeta trait) const noexcept {
      const auto found = mDataListMap.FindKeyIndex(trait);
      if (found)
         return &mDataListMap.GetValue(found);
      return nullptr;
   }


   ///                                                                        
   ///   Abstract geometry content                                            
   ///                                                                        
   
   /// Get the topology of the geometry                                       
   ///   @return the topology type                                            
   LANGULUS(INLINED)
   Anyness::DMeta Geometry::GetTopology() const noexcept {
      return mView.mPrimitiveType;
   }

   /// Get the geometry view                                                  
   ///   @return the geometry view                                            
   LANGULUS(INLINED)
   const GeometryView& Geometry::GetView() const noexcept {
      return mView;
   }


   ///                                                                        
   ///   Abstract texture content                                             
   ///                                                                        
   
   /// Get the pixel format of the texture                                    
   ///   @return the pixel format type                                        
   LANGULUS(INLINED)
   Anyness::DMeta Texture::GetFormat() const noexcept {
      return mView.mFormat;
   }

   /// Get the texture view                                                   
   ///   @return the texture view                                             
   LANGULUS(INLINED)
   const TextureView& Texture::GetView() const noexcept {
      return mView;
   }

} // namespace Langulus::A
