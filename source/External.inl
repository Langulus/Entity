///                                                                           
/// Langulus::Entity                                                          
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "External.hpp"
#include <Math/Primitives/TTriangle.hpp>
#include <Math/Primitives/TLine.hpp>
#include <Math/Primitives/TPoint.hpp>

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
         (mTopology == rhs.mTopology || (mTopology && mTopology->IsExact(rhs.mTopology))) &&
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
   /// Get the full path of the file                                          
   ///   @return a reference to the path                                      
   LANGULUS(INLINED)
   const Anyness::Path& File::GetFilePath() const noexcept {
      return mFilePath;
   }

   /// Read a file and attempt to deserialize it as provided type T           
   /// Throws if deserialization isn't possible                               
   ///   @tparam T - the type to deserialize as, if possible                  
   ///   @return the deserialized instance of T                               
   template<class T>
   LANGULUS(INLINED)
   T File::ReadAs() const {
      return ReadAs(RTTI::MetaData::Of<T>()).template As<T>();
   }


   ///                                                                        
   ///   Abstract folder interface                                            
   ///                                                                        
   /// Get the full path of the folder                                        
   ///   @return a reference to the path                                      
   LANGULUS(INLINED)
      const Anyness::Path& Folder::GetFolderPath() const noexcept {
      return mFolderPath;
   }


   ///                                                                        
   ///   Abstract content unit                                                
   ///                                                                        
   /// Asset constructor                                                      
   ///   @param type - concrete type of the asset                             
   ///   @param producer - the asset library and producer                     
   ///   @param desc - messy descriptor for the content                       
   LANGULUS(INLINED)
   Asset::Asset(RTTI::DMeta type, AssetModule* producer, const Anyness::Descriptor& desc)
      : Unit {type, desc}
      , ProducedFrom<AssetModule> {producer, desc} {}

   /// Get the entire content data map                                        
   ///   @return a reference to the contents                                  
   LANGULUS(INLINED)
   const Asset::DataListMap& Asset::GetDataListMap() const noexcept {
      return mDataListMap;
   }

   /// Get a single data entry from the contents                              
   ///   @tparam T - the trait to search for                                  
   ///   @param index - the Nth data associated to the trait                  
   ///   @return a pointer to the data entry, or nullptr if none exists       
   template<CT::Trait T>
   LANGULUS(INLINED)
   const Asset::Data* Asset::GetData(Offset index) const noexcept {
      return GetData(RTTI::MetaTrait::Of<T>(), index);
   }

   /// Get a single data entry from the contents                              
   ///   @param trait - the trait to search for                               
   ///   @param index - the Nth data associated to the trait                  
   ///   @return a pointer to the data entry, or nullptr if none exists       
   LANGULUS(INLINED)
   const Asset::Data* Asset::GetData(Anyness::TMeta trait, Offset index) const noexcept {
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
   const Asset::DataList* Asset::GetDataList() const noexcept {
      return GetDataList(RTTI::MetaTrait::Of<T>());
   }

   /// Get a data list from the contents                                      
   ///   @param trait - the trait to search for                               
   ///   @return a pointer to the data list, or nullptr if none exists        
   LANGULUS(INLINED)
   const Asset::DataList* Asset::GetDataList(Anyness::TMeta trait) const noexcept {
      const auto found = mDataListMap.FindKeyIndex(trait);
      if (found)
         return &mDataListMap.GetValue(found);
      return nullptr;
   }

   /// Commit data to an asset                                                
   ///   @attention data is always commited, even if such trait already exists
   ///   @tparam T - data trait, the intent we're commiting with              
   ///   @tparam S - semantic and data type to commit (deducible)             
   ///   @param content - the data and semantic to commit                     
   template<CT::Trait T, CT::Semantic S>
   LANGULUS(INLINED)
   void Asset::Commit(S&& content) {
      static_assert(CT::Deep<TypeOf<S>>, "Content should be CT::Deep");
      const auto meta = RTTI::MetaTrait::Of<T>();
      mDataListMap[meta] << content.Forward();
   }


   ///                                                                        
   ///   Abstract geometry content                                            
   ///                                                                        
   /// Get the topology of the geometry                                       
   ///   @return the topology type                                            
   LANGULUS(INLINED)
   Anyness::DMeta Geometry::GetTopology() const noexcept {
      return mView.mTopology;
   }

   /// Check if topology matches a static type                                
   ///   @tparam T type to check                                              
   ///   @return true if T matches topology                                   
   template<CT::Topology T>
   LANGULUS(INLINED)
   bool Geometry::CheckTopology() const {
      return mView.mTopology && mView.mTopology->template Is<T>();
   }

   /// Is geometry made up of triangles/triangle strips/triangle fans?        
   ///   @return true if geometry is made of triangles                        
   LANGULUS(INLINED)
   bool Geometry::MadeOfTriangles() const noexcept {
      return CheckTopology<A::Triangle>();
   }

   /// Is geometry made up of lines/line strips                               
   ///   @return true if geometry is made of lines                            
   LANGULUS(INLINED)
   bool Geometry::MadeOfLines() const noexcept {
      return CheckTopology<A::Line>();
   }

   /// Is geometry made up of points                                          
   ///   @return true if geometry is made of points                           
   LANGULUS(INLINED)
   bool Geometry::MadeOfPoints() const noexcept {
      return CheckTopology<A::Point>();
   }

   /// Get texture mapping mode                                               
   ///   @return the texturing mode                                           
   LANGULUS(INLINED)
   Math::MapMode Geometry::GetTextureMapper() const noexcept {
      return mView.mTextureMapping;
   }

   /// Get the geometry view (const)                                          
   ///   @return the geometry view                                            
   LANGULUS(INLINED)
   const GeometryView& Geometry::GetView() const noexcept {
      return mView;
   }

   /// Get the geometry view                                                  
   ///   @return the geometry view                                            
   LANGULUS(INLINED)
   GeometryView& Geometry::GetView() noexcept {
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

   /// Get the texture view (const)                                           
   ///   @return the texture view                                             
   LANGULUS(INLINED)
   const TextureView& Texture::GetView() const noexcept {
      return mView;
   }
   
   /// Get the texture view                                                   
   ///   @return the texture view                                             
   LANGULUS(INLINED)
   TextureView& Texture::GetView() noexcept {
      return mView;
   }

} // namespace Langulus::A
