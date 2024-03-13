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
#include <Math/Scale.hpp>

LANGULUS_DEFINE_TRAIT(Cursor,
   "Enables or disables cursor for window");
LANGULUS_DEFINE_TRAIT(Monitor,
   "Monitor to which window is bound, making it fullscreen");
LANGULUS_DEFINE_TRAIT(MousePosition,
   "Mouse position, relative to a window");
LANGULUS_DEFINE_TRAIT(MouseScroll,
   "Mouse scroll for window");
LANGULUS_DEFINE_TRAIT(NativeWindowHandle,
   "Native window handle");


namespace Langulus::A
{

   ///                                                                        
   ///   Abstract platform module                                             
   ///                                                                        
   struct PlatformModule : Module {
      LANGULUS_BASES(Module);
      using Module::Module;
   };

   ///                                                                        
   ///   Abstract platform unit                                               
   ///                                                                        
   struct Platform : Unit {
      LANGULUS_BASES(Unit);
      using Unit::Unit;
   };
   
   ///                                                                        
   ///   Abstract platform window                                             
   ///                                                                        
   struct Window : Platform {
      LANGULUS(PRODUCER) PlatformModule;
      LANGULUS_BASES(Platform);
      using Platform::Platform;
      using Scale2 = Math::Scale2;

      NOD() virtual void*  GetNativeHandle() const noexcept = 0;
      NOD() virtual Scale2 GetSize() const noexcept = 0;
      NOD() virtual bool   IsMinimized() const noexcept = 0;
   };
   
   ///                                                                        
   ///   Abstract platform cursor                                             
   ///                                                                        
   struct Cursor : Platform {
      LANGULUS(PRODUCER) PlatformModule;
      LANGULUS_BASES(Platform);
      using Platform::Platform;
   };

} // namespace Langulus::A
