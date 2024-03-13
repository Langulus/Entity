///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
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
      uint32_t mPrimitiveCount {};
      // Starting primitive                                             
      uint32_t mPrimitiveStart {};
      // Number of indices                                              
      uint32_t mIndexCount {};
      // Starting index                                                 
      uint32_t mIndexStart {};
      // Data topology                                                  
      DMeta mTopology {};
      // Double-sidedness                                               
      bool mBilateral {};
      // Texture mapping mode                                           
      Math::MapMode mTextureMapping {};

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
      LANGULUS_BASES(Asset);
      using Asset::Asset;

   protected:
      MeshView mView;

   public:
      template<CT::Topology, CT::Topology...>
      NOD() bool CheckTopology() const;
      NOD() DMeta GetTopology() const noexcept;

      NOD() Math::MapMode GetTextureMapper() const noexcept;

      NOD() const MeshView& GetView() const noexcept;
      NOD() MeshView& GetView() noexcept;

      NOD() virtual Ref<Mesh> GetLOD(const Math::LOD&) const = 0;

      NOD() Math::Vec2u InnerGetIndices(const Data*, const Math::Vec2u&) const;
      NOD() Math::Vec3u InnerGetIndices(const Data*, const Math::Vec3u&) const;

      //                                                                
      NOD() bool MadeOfPoints() const noexcept;
      NOD() Count GetPointCount() const;
      template<CT::Trait T>
      NOD() Any GetPointTrait(Offset) const;

      //                                                                
      NOD() bool MadeOfLines() const noexcept;
      NOD() Count GetLineCount() const;
      NOD() Math::Vec2u GetLineIndices(Offset) const;
      template<CT::Trait T>
      NOD() Any GetLineTrait(Offset) const;

      //                                                                
      NOD() bool MadeOfTriangles() const noexcept;
      NOD() Count GetTriangleCount() const;
      NOD() Math::Vec3u GetTriangleIndices(Offset) const;
      template<CT::Trait T>
      NOD() Any GetTriangleTrait(Offset) const;
   };

} // namespace Langulus::A

namespace Langulus::CT
{

   /// A concept for any kind of geometric content unit                       
   template<class T>
   concept Mesh = DerivedFrom<T, A::Mesh>;

} // namespace Langulus::CT

#include "Mesh.inl"