///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Module.hpp"
#include "Unit.hpp"
#include "Runtime.hpp"
#include <Anyness/Path.hpp>
#include <Flow/Factory.hpp>
#include <Flow/Rate.hpp>
#include <Math/LOD.hpp>
#include <Math/Mapping.hpp>
#include <Math/Scale.hpp>

LANGULUS_DEFINE_TRAIT(FOV,
   "Horizontal field of view angle, usually a real number");
LANGULUS_DEFINE_TRAIT(AspectRatio,
   "Aspect ratio trait (width / height), usually a real number");
LANGULUS_DEFINE_TRAIT(Viewport,
   "Viewport and depth clipping, usually a Range4");


namespace Langulus::A
{

   ///                                                                        
   ///   Abstract graphics module                                             
   ///                                                                        
   struct GraphicsModule : Module {
      LANGULUS_BASES(Module);
      using Module::Module;
   };

   ///                                                                        
   ///   Abstract graphics units                                              
   ///                                                                        
   struct Graphics : Unit {
      LANGULUS_BASES(Unit);
      using Unit::Unit;
   };

   ///                                                                        
   ///   Abstract graphics renderer                                           
   ///                                                                        
   struct Renderer : Graphics {
      LANGULUS(PRODUCER) GraphicsModule;
      LANGULUS_BASES(Graphics);
      using Graphics::Graphics;
   };

   ///                                                                        
   ///   Abstract graphics layer                                              
   ///                                                                        
   struct Layer : Graphics {
      LANGULUS(PRODUCER) Renderer;
      LANGULUS_BASES(Graphics);
      using Graphics::Graphics;
   };

   ///                                                                        
   ///   Abstract graphics camera                                             
   ///                                                                        
   struct Camera : Graphics {
      LANGULUS(PRODUCER) Layer;
      LANGULUS_BASES(Graphics);
      using Graphics::Graphics;
   };

   ///                                                                        
   ///   Abstract graphics renderable                                         
   ///                                                                        
   struct Renderable : Graphics {
      LANGULUS(PRODUCER) Layer;
      LANGULUS_BASES(Graphics);
      using Graphics::Graphics;
   };

   ///                                                                        
   ///   Abstract graphics light                                              
   ///                                                                        
   struct Light : Graphics {
      LANGULUS(PRODUCER) Layer;
      LANGULUS_BASES(Graphics);
      using Graphics::Graphics;
   };

} // namespace Langulus::A

namespace Langulus::CT
{

   /// A concept for any kind of a graphics unit                              
   template<class T>
   concept Graphics = DerivedFrom<T, A::Graphics>;

} // namespace Langulus::CT
