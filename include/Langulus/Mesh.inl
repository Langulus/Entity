///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Mesh.hpp"
#include <Math/Primitives.hpp>


namespace Langulus
{
   
   /// Compare geometry views                                                 
   ///   @param rhs - the geometry view to compare against                    
   ///   @return true if both views are the same                              
   LANGULUS(INLINED)
   bool MeshView::operator == (const MeshView& rhs) const noexcept {
      return mPrimitiveCount == rhs.mPrimitiveCount
         and mPrimitiveStart == rhs.mPrimitiveStart
         and mIndexCount == rhs.mIndexCount
         and mIndexStart == rhs.mIndexStart
         and (mTopology == rhs.mTopology
            or (mTopology and mTopology->IsExact(rhs.mTopology)))
         and mBilateral == rhs.mBilateral;
   }

   /// Decay the geometry view to a list of points                            
   ///   @return the decayed view                                             
   LANGULUS(INLINED)
   MeshView MeshView::Decay() const {
      LANGULUS_ASSERT(mPrimitiveCount and mTopology, Convert, "Bad vertex view");
      if (mTopology->template Is<A::Point>())
         return *this;

      MeshView result {*this};
      result.mTopology = MetaOf<A::Point>();

      // mPrimitiveCount corresponds to the number of points in these   
      // cases:                                                         
      if (  mTopology->template Is<A::TriangleStrip>()
         or mTopology->template Is<A::TriangleFan>()
         or mTopology->template Is<A::LineStrip>()
         or mTopology->template Is<A::LineLoop>())
         return result;

      if (mTopology->template Is<A::Triangle>()) {
         // Decay triangles to points                                   
         result.mPrimitiveCount *= 3;
      }
      if (mTopology->template Is<A::Line>()) {
         // Decay lines to points                                       
         result.mPrimitiveCount *= 2;
      }
      else LANGULUS_THROW(Convert, "Bad primitive type");

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

} // namespace Langulus

namespace Langulus::A
{
   
   /// Get the topology of the geometry                                       
   ///   @return the topology type                                            
   LANGULUS(INLINED)
   DMeta Mesh::GetTopology() const noexcept {
      return mView.mTopology;
   }

   /// Check if topology matches one of the specified types                   
   ///   @tparam T1, TN - types to check                                      
   ///   @return true if T matches one of the topology types                  
   template<CT::Topology T1, CT::Topology...TN> LANGULUS(INLINED)
   bool Mesh::CheckTopology() const {
      return   mView.mTopology == MetaDataOf<T1>()
          or ((mView.mTopology == MetaDataOf<TN>()) or ...);
   }

   /// Helper that indirects in case there is an index buffer                 
   ///   @param indices - index buffer                                        
   ///   @param where - line indices                                          
   ///   @return the (eventually indirected) line indices                     
   inline Math::Vec2u Mesh::InnerGetIndices(const Data* indices, const Math::Vec2u& where) const {
      if (not indices or not *indices)
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
      if (not indices or not *indices)
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
      return {};
   }

   template<CT::Trait T>
   Anyness::Any Mesh::GetPointTrait(Offset) const {
      TODO();
      return {};
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
      else if (CheckTopology<A::TriangleStrip, A::TriangleFan>()) {
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
      using Math::Vec2u;
      const auto indices = GetData<Traits::Index>();

      if (CheckTopology<A::Line>())
         return InnerGetIndices(indices, Vec2u(index * 2, index * 2 + 1));
      else if (CheckTopology<A::LineStrip>())
         return InnerGetIndices(indices, Vec2u(index > 0 ? index - 1 : 0, index > 0 ? index : 1));
      else if (CheckTopology<A::LineLoop>()) {
         if (index == GetLineCount() - 1)
            return InnerGetIndices(indices, Vec2u(index, 0));
         else
            return InnerGetIndices(indices, Vec2u(index > 0 ? index - 1 : 0, index > 0 ? index : 1));
      }
      else if (CheckTopology<A::Triangle>()) {
         switch (index % 3) {
         case 0: case 1:
            return InnerGetIndices(indices, Vec2u(index, index + 1));
         case 2:
            return InnerGetIndices(indices, Vec2u(index, index - 2));
         }
      }
      else if (CheckTopology<A::TriangleStrip>()) {
         switch (index % 3) {
         case 0:
            return InnerGetIndices(indices, Vec2u(index == 0 ? 0 : index - 2, index == 0 ? 1 : index - 1));
         case 1:
            return InnerGetIndices(indices, Vec2u(index - 1, index));
         case 2:
            return InnerGetIndices(indices, Vec2u(index, index - 2));
         }
      }
      else if (CheckTopology<A::TriangleFan>()) {
         switch (index % 3) {
         case 0:
            return InnerGetIndices(indices, Vec2u(0, index == 0 ? 1 : index - 1));
         case 1:
            return InnerGetIndices(indices, Vec2u(index - 1, index));
         case 2:
            return InnerGetIndices(indices, Vec2u(index, 0));
         }
      }

      LANGULUS_THROW(Access, "Trying to count lines for unknown topology");
   }

   /// Get a specific property of a specific line                             
   ///   @tparam T - the trait to retrieve                                    
   ///   @param lineIndex - the line index                                    
   ///   @return data for the specific line                                   
   template<CT::Trait T>
   Any Mesh::GetLineTrait(Offset lineIndex) const {
      const auto indices = GetLineIndices(lineIndex);
      const auto soughtt = GetData<T>(0);
      if (not soughtt or not *soughtt)
         return {};

      Any soughtDecayed;
      if (soughtt->template CastsTo<Math::Triangle3>())
         soughtDecayed = soughtt->template ReinterpretAs<Math::Point3>();
      else if (soughtt->template CastsTo<Math::Triangle2>())
         soughtDecayed = soughtt->template ReinterpretAs<Math::Point2>();
      else
         soughtDecayed = *soughtt;

      Any result;
      result.InsertBlock(IndexBack, soughtDecayed.GetElement(indices[0]));
      result.InsertBlock(IndexBack, soughtDecayed.GetElement(indices[1]));
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
      if (MadeOfPoints() or MadeOfLines())
         return 0;

      if (CheckTopology<A::Triangle>()) {
         if (mView.mIndexCount > 2)
            return mView.mIndexCount / 3;

         auto decayed = mView.Decay();
         LANGULUS_ASSERT(decayed.mPrimitiveCount % 3, Access, "Bad topology");
         return decayed.mPrimitiveCount / 3;
      }
      else if (CheckTopology<A::TriangleStrip, A::TriangleFan>()) {
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
      using Math::Vec3u;
      const auto indices = GetData<Traits::Index>(0);

      if (CheckTopology<A::Triangle>()) {
         return InnerGetIndices(indices, Vec3u(
            index * 3,
            index * 3 + 1,
            index * 3 + 2)
         );
      }
      else if (CheckTopology<A::TriangleStrip>()) {
         return InnerGetIndices(indices, Vec3u(
            index == 0 ? 0 : index - 1,
            index == 0 ? 1 : index,
            index == 0 ? 2 : index + 1)
         );
      }
      else if (CheckTopology<A::TriangleFan>()) {
         return InnerGetIndices(indices, Vec3u(
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
   Any Mesh::GetTriangleTrait(Offset triangleIndex) const {
      const auto indices = GetTriangleIndices(triangleIndex);
      const auto soughtt = GetData<T>(0);
      if (not soughtt or not *soughtt)
         return {};

      Any soughtDecayed;
      if (soughtt->template CastsTo<Math::Triangle3>())
         soughtDecayed = soughtt->template ReinterpretAs<Math::Point3>();
      else if (soughtt->template CastsTo<Math::Triangle2>())
         soughtDecayed = soughtt->template ReinterpretAs<Math::Point2>();
      else
         soughtDecayed = *soughtt;

      Any result;
      result.InsertBlock(IndexBack, soughtDecayed.GetElement(indices[0]));
      result.InsertBlock(IndexBack, soughtDecayed.GetElement(indices[1]));
      result.InsertBlock(IndexBack, soughtDecayed.GetElement(indices[2]));
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

} // namespace Langulus::A
