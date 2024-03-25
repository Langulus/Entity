///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "../Entity/Thing.hpp"

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
   struct GraphicsModule : virtual Module {
      LANGULUS_BASES(Module);
      GraphicsModule()
         : Resolvable {DMeta {}}
         , Module {nullptr} {}
   };

   ///                                                                        
   ///   Abstract graphics units                                              
   ///                                                                        
   struct Graphics : virtual Unit {
      LANGULUS_BASES(Unit);
      Graphics()
         : Resolvable {DMeta {}} {}
   };

   ///                                                                        
   ///   Abstract graphics renderer                                           
   ///                                                                        
   struct Renderer : virtual Graphics {
      LANGULUS(PRODUCER) GraphicsModule;
      LANGULUS_BASES(Graphics);
      Renderer()
         : Resolvable {DMeta {}} {}
   };

   ///                                                                        
   ///   Abstract graphics layer                                              
   ///                                                                        
   struct Layer : virtual Graphics {
      LANGULUS(PRODUCER) Renderer;
      LANGULUS_BASES(Graphics);
      Layer()
         : Resolvable {DMeta {}} {}
   };

   ///                                                                        
   ///   Abstract graphics camera                                             
   ///                                                                        
   struct Camera : virtual Graphics {
      LANGULUS(PRODUCER) Layer;
      LANGULUS_BASES(Graphics);
      Camera()
         : Resolvable {DMeta {}} {}
   };

   ///                                                                        
   ///   Abstract graphics renderable                                         
   ///                                                                        
   struct Renderable : virtual Graphics {
      LANGULUS(PRODUCER) Layer;
      LANGULUS_BASES(Graphics);
      Renderable()
         : Resolvable {DMeta {}} {}
   };

   ///                                                                        
   ///   Abstract graphics light                                              
   ///                                                                        
   struct Light : virtual Graphics {
      LANGULUS(PRODUCER) Layer;
      LANGULUS_BASES(Graphics);
      Light()
         : Resolvable {DMeta {}} {}
   };

} // namespace Langulus::A

namespace Langulus::CT
{

   /// A concept for any kind of a graphics unit                              
   template<class T>
   concept Graphics = DerivedFrom<T, A::Graphics>;

} // namespace Langulus::CT
