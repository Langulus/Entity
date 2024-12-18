///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Asset.hpp"
#include <Flow/Rate.hpp>

LANGULUS_DEFINE_TRAIT(Material, "Material unit");
LANGULUS_DEFINE_TRAIT(Shader,   "Shader unit");
LANGULUS_EXCEPTION(Material);


namespace Langulus
{

   ///                                                                        
   ///   Shader stages                                                        
   ///                                                                        
   namespace ShaderStage
   {
      enum Enum : uint32_t {
         Vertex = 0,          // Vertex shader stage                    
         Geometry,            // Geometry shader stage                  
         TessCtrl,            // Tesselation control stage              
         TessEval,            // Tesselation evaluation stage           
         Pixel,               // Pixel shader stage                     
         Compute,             // Compute shader stage                   

         Counter              // Shader stage counter (keep at end)     
      };

      /// Shader names                                                        
      constexpr Token Names[Enum::Counter] = {
         "vertex",
         "geometry",
         "tesselation control",
         "tesselation evaluation",
         "fragment",
         "compute"
      };
   }


   ///                                                                        
   ///   Shader layout tokens                                                 
   ///                                                                        
   namespace ShaderToken
   {
      constexpr Token Version       = "//#VERSION\n";
      constexpr Token Defines       = "//#DEFINES\n";
      constexpr Token Input         = "//#INPUT\n";
      constexpr Token Output        = "//#OUTPUT\n";
      constexpr Token Dependencies  = "//#DEPENDENCY\n";
      constexpr Token Colorize      = "//#COLORIZE\n";
      constexpr Token Transform     = "//#TRANSFORM\n";
      constexpr Token Position      = "//#POSITION\n";
      constexpr Token Uniform       = "//#UNIFORM\n";
      constexpr Token Texturize     = "//#TEXTURIZE\n";
      constexpr Token Functions     = "//#FUNCTIONS\n";
   }

   namespace A
   {

      ///                                                                     
      ///   Abstract material content                                         
      ///                                                                     
      struct Material : Asset {
         LANGULUS_BASES(Asset);
         using Asset::Asset;

         NOD() virtual auto GetLOD(const Math::LOD&) const -> Ref<Material> = 0;

         NOD() auto GetInputs(RefreshRate) const -> const TraitList&;
         NOD() auto GetInputs(Offset) const -> const TraitList&;

         NOD() auto GetOutputs(RefreshRate) const -> const TraitList&;
         NOD() auto GetOutputs(Offset) const -> const TraitList&;

      protected:
         mutable TraitList mInputs[RefreshRate::InputCount];
         mutable TraitList mOutputs[RefreshRate::InputCount];
      };

   } // namespace Langulus::A
} // namespace Langulus

#include "Material.inl"