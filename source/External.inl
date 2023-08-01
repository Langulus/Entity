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
#include <Math/Colors.hpp>

namespace Langulus
{

   ///                                                                        
   ///   Vertex/index buffer view                                             
   ///                                                                        
   
   /// Compare geometry views                                                 
   ///   @param rhs - the geometry view to compare against                    
   ///   @return true if both views are the same                              
   LANGULUS(INLINED)
   bool MeshView::operator == (const MeshView& rhs) const noexcept {
      return mPrimitiveCount == rhs.mPrimitiveCount &&
             mPrimitiveStart == rhs.mPrimitiveStart &&
             mIndexCount == rhs.mIndexCount &&
             mIndexStart == rhs.mIndexStart &&
             (mTopology == rhs.mTopology
                || (mTopology && mTopology->IsExact(rhs.mTopology))) &&
             mBilateral == rhs.mBilateral;
   }

   /// Decay the geometry view to a list of points                            
   ///   @return the decayed view                                             
   LANGULUS(INLINED)
   MeshView MeshView::Decay() const {
      LANGULUS_ASSERT(mPrimitiveCount && mTopology, Convert, "Bad vertex view");
      if (mTopology->template Is<A::Point>())
         return *this;

      MeshView result {*this};
      result.mTopology = MetaOf<A::Point>();

      // mPrimitiveCount corresponds to the number of points in these   
      // cases:                                                         
      if (  mTopology->template Is<A::TriangleStrip>()
         || mTopology->template Is<A::TriangleFan>()
         || mTopology->template Is<A::LineStrip>()
         || mTopology->template Is<A::LineLoop>())
         return result;

      if (mTopology->template Is<A::Triangle>())
         // Decay triangles to points                                   
         result.mPrimitiveCount *= 3;
      if (mTopology->template Is<A::Line>())
         // Decay lines to points                                       
         result.mPrimitiveCount *= 2;
      else
         LANGULUS_THROW(Convert, "Bad primitive type");

      return result;
   }
   
   /// Get the hash of a normalized descriptor (cached)                       
   ///   @return the hash                                                     
   LANGULUS(INLINED)
   Hash MeshView::GetHash() const noexcept {
      return HashOf(
         mPrimitiveCount, mPrimitiveStart, mIndexCount, mIndexStart,
         mTopology, mBilateral, mTextureMapping
      );
   }


   ///                                                                        
   ///   Universal pixel buffer view                                          
   ///                                                                        
   
   /// Compare texture views                                                  
   ///   @param rhs - the texture view to compare against                     
   ///   @return true if both views are the same                              
   LANGULUS(INLINED)
   bool ImageView::operator == (const ImageView& rhs) const noexcept {
      return mWidth == rhs.mWidth &&
             mHeight == rhs.mHeight &&
             mDepth == rhs.mDepth &&
             mFrames == rhs.mFrames &&
             (mFormat == rhs.mFormat
                || (mFormat && mFormat->IsExact(rhs.mFormat)));
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
      return mFormat ? mFormat->mSize : 0;
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

   ///                                                                        
   ///   Abstract file system interface                                       
   ///                                                                        

   /// Get the current working path (where the main exe was executed)         
   ///   @return the path                                                     
   LANGULUS(INLINED)
   const Anyness::Path& FileSystem::GetWorkingPath() const noexcept {
      return mWorkingPath;
   }

   /// Get the current data path, like GetWorkingPath() / "data"              
   ///   @return the path                                                     
   LANGULUS(INLINED)
   const Anyness::Path& FileSystem::GetDataPath() const noexcept {
      return mMainDataPath;
   }


   ///                                                                        
   ///   Abstract file interface                                              
   ///                                                                        
   
   /// Check if file physically exists                                        
   ///   @return true if file exists                                          
   LANGULUS(INLINED)
   bool File::Exists() const noexcept {
      return mExists;
   }

   /// Check if file is read-only                                             
   ///   @return true if file exists and is read-only                         
   LANGULUS(INLINED)
   bool File::IsReadOnly() const noexcept {
      return mIsReadOnly;
   }

   /// Check file format, that is deduced from file extension, and compared   
   /// against all reflected file extensions                                  
   ///   @return the file format                                              
   LANGULUS(INLINED)
   Anyness::DMeta File::GetFormat() const noexcept {
      return mFormat;
   }

   /// Get the size of the file in bytes (if file exists)                     
   ///   @return the size of the file in bytes, or 0 if it doesn't exist      
   LANGULUS(INLINED)
   Size File::GetBytesize() const noexcept {
      return mByteCount;
   }

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
  
   /// Check if folder physically exists                                      
   ///   @return true if folder exists                                        
   LANGULUS(INLINED)
   bool Folder::Exists() const noexcept {
      return mExists;
   }

   /// Check if file is read-only                                             
   ///   @return true if file exists and is read-only                         
   LANGULUS(INLINED)
   bool Folder::IsReadOnly() const noexcept {
      return mIsReadOnly;
   }

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
      return GetData(T::GetTrait(), index);
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
      return GetDataList(T::GetTrait());
   }

   /// Get a data list from the contents                                      
   ///   @param trait - the trait to search for                               
   ///   @return a pointer to the data list, or nullptr if none exists        
   LANGULUS(INLINED)
   const Asset::DataList* Asset::GetDataList(Anyness::TMeta trait) const noexcept {
      const auto found = mDataListMap.Find(trait);
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
      static_assert(CT::Block<TypeOf<S>>,
         "Content should be provided in a Block");
      mDataListMap[T::GetTrait()] << content.Forward();
   }


   ///                                                                        
   ///   Abstract geometry content                                            
   ///                                                                        
   
   /// Get the topology of the geometry                                       
   ///   @return the topology type                                            
   LANGULUS(INLINED)
   Anyness::DMeta Mesh::GetTopology() const noexcept {
      return mView.mTopology;
   }

   /// Check if topology matches a static type                                
   ///   @tparam T type to check                                              
   ///   @return true if T matches topology                                   
   template<CT::Topology T>
   LANGULUS(INLINED)
   bool Mesh::CheckTopology() const {
      return mView.mTopology && mView.mTopology->template Is<T>();
   }

   /// Helper that indirects in case there is an index buffer                 
   ///   @param indices - index buffer                                        
   ///   @param where - line indices                                          
   ///   @return the (eventually indirected) line indices                     
   inline Math::Vec2u Mesh::InnerGetIndices(const Data* indices, const Math::Vec2u& where) const {
      if (!indices || !*indices)
         return where;

      if (indices->IsExact<std::uint8_t>()) {
         auto idx = indices->As<std::uint8_t*>();
         return {idx[where[0]], idx[where[1]]};
      }
      
      if (indices->IsExact<std::uint16_t>()) {
         auto idx = indices->As<std::uint16_t*>();
         return {idx[where[0]], idx[where[1]]};
      }

      if (indices->IsExact<std::uint32_t>()) {
         auto idx = indices->As<std::uint32_t*>();
         return {idx[where[0]], idx[where[1]]};
      }

      if (indices->IsExact<std::uint64_t>()) {
         auto idx = indices->As<std::uint64_t*>();
         return {
            static_cast<std::uint32_t>(idx[where[0]]),
            static_cast<std::uint32_t>(idx[where[1]])
         };
      }

      LANGULUS_THROW(Access, "Trying to get index from incompatible index buffer");
   }

   /// Helper that indirects in case there is an index buffer                 
   ///   @param indices - index buffer                                        
   ///   @param where - triangle indices                                      
   ///   @return the (eventually indirected) triangle indices                 
   inline Math::Vec3u Mesh::InnerGetIndices(const Data* indices, const Math::Vec3u& where) const {
      if (!indices || !*indices)
         return where;

      if (indices->IsExact<std::uint8_t>()) {
         auto idx = indices->As<std::uint8_t*>();
         return {idx[where[0]], idx[where[1]], idx[where[2]]};
      }

      if (indices->IsExact<std::uint16_t>()) {
         auto idx = indices->As<std::uint16_t*>();
         return {idx[where[0]], idx[where[1]], idx[where[2]]};
      }

      if (indices->IsExact<std::uint32_t>()) {
         auto idx = indices->As<std::uint32_t*>();
         return {idx[where[0]], idx[where[1]], idx[where[2]]};
      }

      if (indices->IsExact<std::uint64_t>()) {
         auto idx = indices->As<std::uint64_t*>();
         return {
            static_cast<std::uint32_t>(idx[where[0]]),
            static_cast<std::uint32_t>(idx[where[1]]),
            static_cast<std::uint32_t>(idx[where[2]])
         };
      }

      LANGULUS_THROW(Access, "Trying to get index from incompatible index buffer");
   }

   /// Is topology a point list?                                              
   ///   @return true if geometry is made of points                           
   LANGULUS(INLINED)
   bool Mesh::MadeOfPoints() const noexcept {
      return CheckTopology<A::Point>();
   }

   /// Get the number of points inside the geometry                           
   ///   @return the number of points                                         
   LANGULUS(INLINED)
   Count Mesh::GetPointCount() const {
      TODO();
   }

   template<CT::Trait T>
   Anyness::Any Mesh::GetPointTrait(Offset) const {
      TODO();
   }

   /// Is topology line list/strip/loop?                                      
   ///   @return true if geometry is made of lines                            
   LANGULUS(INLINED)
   bool Mesh::MadeOfLines() const noexcept {
      return CheckTopology<A::Line>();
   }

   /// Get the number of lines inside the geometry                            
   ///   @return the number of points                                         
   inline Count Mesh::GetLineCount() const {
      if (MadeOfPoints())
         return 0;
      
      if (CheckTopology<A::Line>()) {
         if (mView.mIndexCount > 1)
            return mView.mIndexCount / 2;
         else {
            auto decayed = mView.Decay();
            if (decayed.mPrimitiveCount > 1)
               return decayed.mPrimitiveCount / 2;
         }
         return 0;
      }
      else if (CheckTopology<A::LineStrip>()) {
         if (mView.mIndexCount > 1)
            return mView.mIndexCount - 1;
         else if (mView.mPrimitiveCount > 1)
            return mView.mPrimitiveCount - 1;
         return 0;
      }
      else if (CheckTopology<A::LineLoop>()) {
         if (mView.mIndexCount > 1)
            return mView.mIndexCount;
         else if (mView.mPrimitiveCount > 1)
            return mView.mPrimitiveCount;
         return 0;
      }
      else if (CheckTopology<A::Triangle>()) {
         if (mView.mIndexCount > 2)
            return mView.mIndexCount / 3;
         else {
            auto decayed = mView.Decay();
            if (decayed.mPrimitiveCount > 2)
               return decayed.mPrimitiveCount;
         }
         return 0;
      }
      else if (CheckTopology<A::TriangleStrip>() || CheckTopology<A::TriangleFan>()) {
         if (mView.mIndexCount > 2)
            return 1 + (mView.mIndexCount - 2) * 2;
         else if (mView.mPrimitiveCount > 2)
            return 1 + (mView.mPrimitiveCount - 2) * 2;
         return 0;
      }

      LANGULUS_THROW(Access, "Trying to count lines for unknown topology");
   }

   /// Get the point indices of a given line                                  
   ///   @param index - line index                                            
   ///   @return the point indices as a 32bit unsigned 2D vector              
   inline Math::Vec2u Mesh::GetLineIndices(Offset index) const {
      const auto indices = GetData<Traits::Index>();

      if (CheckTopology<A::Line>()) {
         return InnerGetIndices(indices, Math::Vec2u(index * 2, index * 2 + 1));
      }
      else if (CheckTopology<A::LineStrip>()) {
         return InnerGetIndices(indices, Math::Vec2u(index > 0 ? index - 1 : 0, index > 0 ? index : 1));
      }
      else if (CheckTopology<A::LineLoop>()) {
         if (index == GetLineCount() - 1)
            return InnerGetIndices(indices, Math::Vec2u(index, 0));
         else
            return InnerGetIndices(indices, Math::Vec2u(index > 0 ? index - 1 : 0, index > 0 ? index : 1));
      }
      else if (CheckTopology<A::Triangle>()) {
         switch (index % 3) {
         case 0: case 1:
            return InnerGetIndices(indices, Math::Vec2u(index, index + 1));
         case 2:
            return InnerGetIndices(indices, Math::Vec2u(index, index - 2));
         }
      }
      else if (CheckTopology<A::TriangleStrip>()) {
         switch (index % 3) {
         case 0:
            return InnerGetIndices(indices, Math::Vec2u(index == 0 ? 0 : index - 2, index == 0 ? 1 : index - 1));
         case 1:
            return InnerGetIndices(indices, Math::Vec2u(index - 1, index));
         case 2:
            return InnerGetIndices(indices, Math::Vec2u(index, index - 2));
         }
      }
      else if (CheckTopology<A::TriangleFan>()) {
         switch (index % 3) {
         case 0:
            return InnerGetIndices(indices, Math::Vec2u(0, index == 0 ? 1 : index - 1));
         case 1:
            return InnerGetIndices(indices, Math::Vec2u(index - 1, index));
         case 2:
            return InnerGetIndices(indices, Math::Vec2u(index, 0));
         }
      }

      LANGULUS_THROW(Access, "Trying to count lines for unknown topology");
   }

   /// Get a specific property of a specific line                             
   ///   @tparam T - the trait to retrieve                                    
   ///   @param lineIndex - the line index                                    
   ///   @return data for the specific line                                   
   template<CT::Trait T>
   Anyness::Any Mesh::GetLineTrait(Offset lineIndex) const {
      const auto indices = GetLineIndices(lineIndex);
      const auto soughtt = GetData<T>(0);
      if (!soughtt || !*soughtt)
         return {};

      Anyness::Block soughtDecayed;
      if (soughtt->template CastsTo<Math::Triangle3>())
         soughtDecayed = Anyness::Block::From(
            soughtt->template As<Math::Point3*>(), soughtt->GetCount() * 3);
      else if (soughtt->template CastsTo<Math::Triangle2>())
         soughtDecayed = Anyness::Block::From(
            soughtt->template As<Math::Point2*>(), soughtt->GetCount() * 3);
      else
         soughtDecayed = *static_cast<const Anyness::Block*>(soughtt);

      Anyness::Any result;
      result.InsertBlock(soughtDecayed.GetElement(indices[0]));
      result.InsertBlock(soughtDecayed.GetElement(indices[1]));
      return result;
   }
   
   /// Is geometry made up of triangles list/strip/fan?                       
   ///   @return true if geometry is made of triangles                        
   LANGULUS(INLINED)
   bool Mesh::MadeOfTriangles() const noexcept {
      return CheckTopology<A::Triangle>();
   }
   
   /// Get the number of triangles inside the geometry                        
   ///   @return the number of points                                         
   inline Count Mesh::GetTriangleCount() const {
      if (MadeOfPoints() || MadeOfLines())
         return 0;

      if (CheckTopology<A::Triangle>()) {
         if (mView.mIndexCount > 2)
            return mView.mIndexCount / 3;

         auto decayed = mView.Decay();
         LANGULUS_ASSERT(decayed.mPrimitiveCount % 3, Access, "Bad topology");
         return decayed.mPrimitiveCount / 3;
      }
      else if (CheckTopology<A::TriangleStrip>() || CheckTopology<A::TriangleFan>()) {
         if (mView.mIndexCount > 2)
            return mView.mIndexCount - 2;

         LANGULUS_ASSERT(mView.mPrimitiveCount < 3, Access, "Bad topology");
         return mView.mPrimitiveCount - 2;
      }

      LANGULUS_THROW(Access, "Trying to count triangles for unknown topology");
   }

   /// Get the indices of a given triangle                                    
   ///   @param index - triangle index                                        
   ///   @return the indices as a 32bit unsigned 3D vector                    
   inline Math::Vec3u Mesh::GetTriangleIndices(Offset index) const {
      const auto indices = GetData<Traits::Index>(0);

      if (CheckTopology<A::Triangle>()) {
         return InnerGetIndices(indices, Math::Vec3u(
            index * 3,
            index * 3 + 1,
            index * 3 + 2)
         );
      }
      else if (CheckTopology<A::TriangleStrip>()) {
         return InnerGetIndices(indices, Math::Vec3u(
            index == 0 ? 0 : index - 1,
            index == 0 ? 1 : index,
            index == 0 ? 2 : index + 1)
         );
      }
      else if (CheckTopology<A::TriangleFan>()) {
         return InnerGetIndices(indices, Math::Vec3u(
            0,
            index == 0 ? 1 : index,
            index == 0 ? 2 : index + 1)
         );
      }

      LANGULUS_THROW(Access, "Trying to count triangles for unknown topology");
   }

   /// Get a specific property of a specific triangle                         
   ///   @tparam T - the trait to retrieve                                    
   ///   @param triangleIndex - the triangle index                            
   ///   @return data for the specific triangle                               
   template<CT::Trait T>
   Anyness::Any Mesh::GetTriangleTrait(Offset triangleIndex) const {
      const auto indices = GetTriangleIndices(triangleIndex);
      const auto soughtt = GetData<T>(0);
      if (!soughtt || !*soughtt)
         return {};

      Anyness::Block soughtDecayed;
      if (soughtt->template CastsTo<Math::Triangle3>())
         soughtDecayed = soughtt->template ReinterpretAs<Math::Point3>();
      else if (soughtt->template CastsTo<Math::Triangle2>())
         soughtDecayed = soughtt->template ReinterpretAs<Math::Point2>();
      else
         soughtDecayed = *static_cast<const Anyness::Block*>(soughtt);

      Anyness::Any result;
      result.InsertBlock(soughtDecayed.GetElement(indices[0]));
      result.InsertBlock(soughtDecayed.GetElement(indices[1]));
      result.InsertBlock(soughtDecayed.GetElement(indices[2]));
      return result;
   }
   
   /// Get texture mapping mode                                               
   ///   @return the texturing mode                                           
   LANGULUS(INLINED)
   Math::MapMode Mesh::GetTextureMapper() const noexcept {
      return mView.mTextureMapping;
   }

   /// Get the geometry view (const)                                          
   ///   @return the geometry view                                            
   LANGULUS(INLINED)
   const MeshView& Mesh::GetView() const noexcept {
      return mView;
   }

   /// Get the geometry view                                                  
   ///   @return the geometry view                                            
   LANGULUS(INLINED)
      MeshView& Mesh::GetView() noexcept {
      return mView;
   }


   ///                                                                        
   ///   Abstract material content                                            
   ///                                                                        
   
   /// Get input traits for a given rate (const)                              
   ///   @param rate - the rate                                               
   ///   @return the input trait list                                         
   LANGULUS(INLINED)
   const Entity::TraitList& Material::GetInputs(Flow::Rate rate) const {
      return GetInputs(rate.GetInputIndex());
   }

   /// Get input traits for a given input offset (const)                      
   ///   @attention offset must be in the range [0; Rate::InputCount)         
   ///   @param rate - the input offset                                       
   ///   @return the input trait list                                         
   LANGULUS(INLINED)
   const Entity::TraitList& Material::GetInputs(Offset rate) const {
      LANGULUS_ASSUME(DevAssumes,
         Asset::mDataListMap.ContainsKey(Traits::Input::GetTrait()),
         "Material doesn't contain inputs"
      );
      LANGULUS_ASSUME(DevAssumes,
         Asset::GetDataList<Traits::Input>()->GetCount() == Flow::Rate::InputCount,
         "Material doesn't contain the correct number of rates"
      );
      LANGULUS_ASSUME(DevAssumes,
         rate < Flow::Rate::InputCount,
         "Input offset out of range"
      );

      return *reinterpret_cast<const Entity::TraitList*>(
         Asset::GetData<Traits::Input>(rate)
      );
   }

   /// Get output traits for a given rate (const)                             
   ///   @param rate - the rate                                               
   ///   @return the output trait list                                        
   LANGULUS(INLINED)
   const Entity::TraitList& Material::GetOutputs(Flow::Rate rate) const {
      return GetOutputs(rate.GetInputIndex());
   }

   /// Get output traits for a given input offset (const)                     
   ///   @attention offset must be in the range [0; Rate::InputCount)         
   ///   @param rate - the input offset                                       
   ///   @return the output trait list                                        
   LANGULUS(INLINED)
   const Entity::TraitList& Material::GetOutputs(Offset rate) const {
      LANGULUS_ASSUME(DevAssumes,
         Asset::mDataListMap.ContainsKey(Traits::Output::GetTrait()),
         "Material doesn't contain inputs"
      );
      LANGULUS_ASSUME(DevAssumes,
         Asset::GetDataList<Traits::Output>()->GetCount() == Flow::Rate::InputCount,
         "Material doesn't contain the correct number of rates"
      );
      LANGULUS_ASSUME(DevAssumes,
         rate < Flow::Rate::InputCount,
         "Input offset out of range"
      );

      return *reinterpret_cast<const Entity::TraitList*>(
         Asset::GetData<Traits::Output>(rate)
      );
   }


   ///                                                                        
   ///   Abstract texture content                                             
   ///                                                                        
   
   /// Get the pixel format of the texture                                    
   ///   @return the pixel format type                                        
   LANGULUS(INLINED)
   Anyness::DMeta Image::GetFormat() const noexcept {
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

      LANGULUS_ASSUME(DevAssumes, pixels && *pixels,
         "No color data", " in ", Self());
      LANGULUS_ASSUME(DevAssumes, pixels->IsDense(),
         "Image data is not dense", " for ", Self());
      LANGULUS_ASSUME(DevAssumes,
         pixels->CastsTo<Anyness::Bytes>() || pixels->CastsTo<A::Color>(),
         "Image doesn't contain pixel data", 
         " - contains ", pixels->GetType(), " instead"
      );

      using A = ArgumentOf<F>;
      using R = ReturnOf<F>;

      static_assert(CT::Constant<A>, "Color iterator must be constant");
      static_assert(CT::Dense<A>, "Color iterator must be dense");

      LANGULUS_ASSUME(DevAssumes, mView.mFormat->IsExact<A>(),
         "Iterator type is not compatible with contained color data");

      // Iterate using the desired color type                           
      UNUSED() Count counter = 0;
      auto data = pixels->CastsTo<Anyness::Bytes>()
         ? pixels->Get<Anyness::Bytes>().GetRaw()
         : pixels->GetRaw();
      const auto dataEnd = data + mView.GetBytesize();
      while (data != dataEnd) {
         if constexpr (CT::Bool<R>) {
            ++counter;
            if (!call(*reinterpret_cast<const Deref<A>*>(data)))
               return counter;
         }
         else call(*reinterpret_cast<const Deref<A>*>(data));

         data += mView.mFormat->mSize;
      }

      if constexpr (CT::Bool<R>)
         return counter;
   }

   /// Upload raw data to the image by a semantic                                
   ///   @attention deletes any data previously commited                         
   ///   @param data - the block of data                                         
   LANGULUS(INLINED)
   void Image::Upload(CT::Semantic auto&& data) {
      using S = Decay<decltype(data)>;
      using T = TypeOf<S>;
      static_assert(CT::Block<T>, "Data must be inside a Block");

      // Check if provided data matches the view requirements              
      LANGULUS_ASSERT(mView.GetBytesize() == data->GetBytesize(), Image,
         "Data is of the wrong size");

      // Reset data and commit the new one                                 
      mDataListMap.Reset();
      Commit<Traits::Color>(data.Forward());
   }

} // namespace Langulus::A
