///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Asset.hpp"
#include <Math/Mapping.hpp>

LANGULUS_DEFINE_TRAIT(Mesh, "Mesh unit");
LANGULUS_EXCEPTION(Mesh);


namespace Langulus
{

   ///                                                                        
   ///   Vertex/index buffer view                                             
   ///                                                                        
   struct MeshView {
      // Number of primitives                                           
      uint32_t mPrimitiveCount = 0;
      // Starting primitive                                             
      uint32_t mPrimitiveStart = 0;
      // Number of indices                                              
      uint32_t mIndexCount = 0;
      // Starting index                                                 
      uint32_t mIndexStart = 0;
      // Data topology                                                  
      DMeta mTopology;
      // Double-sidedness                                               
      bool mBilateral = false;
      // Texture mapping mode                                           
      Math::MapModeType mTextureMapping = Math::MapModeType::Auto;

      bool operator == (const MeshView&) const noexcept;

      NOD() MeshView Decay() const;
      NOD() Hash GetHash() const noexcept;
   };
   
} // namespace Langulus

namespace Langulus::A
{
   
   ///                                                                        
   ///   Abstract geometry content                                            
   ///                                                                        
   struct Mesh : Asset {
   protected:
      MeshView mView;

   public:
      LANGULUS_BASES(Asset);
      Mesh() : Resolvable {this} {}

      template<CT::Topology, CT::Topology...>
      NOD() bool CheckTopology() const;
      NOD() DMeta GetTopology() const noexcept;
      NOD() Math::MapModeType GetTextureMapper() const noexcept;

      NOD() MeshView const& GetView() const noexcept;
      NOD() MeshView&       GetView() noexcept;

      NOD() virtual Ref<Mesh> GetLOD(const Math::LOD&) const = 0;

      // Point utilities                                                
      NOD() bool MadeOfPoints() const noexcept;
      NOD() Count GetPointCount() const;
      template<CT::Trait>
      NOD() Many GetPointTrait(Offset) const;

      // Line utilities                                                 
      NOD() bool MadeOfLines() const noexcept;
      NOD() Count GetLineCount() const;
      NOD() Math::Vec2u GetLineIndices(Offset) const;
      template<CT::Trait>
      NOD() Many GetLineTrait(Offset) const;

      // Triangle utilities                                             
      NOD() bool MadeOfTriangles() const noexcept;
      NOD() Count GetTriangleCount() const;
      NOD() Math::Vec3u GetTriangleIndices(Offset) const;
      template<CT::Trait>
      NOD() Many GetTriangleTrait(Offset) const;

      ///                                                                     
      ///   Iteration                                                         
      ///                                                                     
      Count ForEachVertex(auto&&) const;

   protected:
      NOD() Math::Vec2u InnerGetIndices(const Data*, const Math::Vec2u&) const;
      NOD() Math::Vec3u InnerGetIndices(const Data*, const Math::Vec3u&) const;

      template<bool INDEXED, class...T>
      Count ForEachVertexInner(Types<T...>, auto&& call) const;

      template<CT::Trait T>
      T ForEachVertex_PrepareStream() const;

      template<CT::Trait T>
      T ForEachVertex_PrepareIndexStream() const;

      template<CT::Topology T>
      auto PickVertex(Offset i, const CT::Trait auto& data, const CT::Trait auto& indices) const;

      template<CT::Topology T, size_t...STREAM_ID>
      auto GenerateVertex(Offset i, const auto& data, const auto& indices, std::index_sequence<STREAM_ID...>&&) const;
   };

} // namespace Langulus::A

namespace Langulus::CT
{

   /// A concept for any kind of geometric content unit                       
   template<class T>
   concept Mesh = DerivedFrom<T, A::Mesh>;

} // namespace Langulus::CT

#include "Mesh.inl"