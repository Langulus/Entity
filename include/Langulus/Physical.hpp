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
#include <Math/LOD.hpp>
#include <Math/Level.hpp>
#include <Math/Matrix.hpp>


namespace Langulus::A
{

   ///                                                                        
   ///   Abstract physical module                                             
   ///                                                                        
   struct PhysicalModule : Module {
      LANGULUS_BASES(Module);
      using Module::Module;
   };

   ///                                                                        
   ///   Abstract physical unit                                               
   ///                                                                        
   struct Physical : Unit {
      LANGULUS_BASES(Unit);
      using Unit::Unit;
   };

   ///                                                                        
   ///   Abstract physical world unit                                         
   ///                                                                        
   struct World : Physical {
      LANGULUS(PRODUCER) PhysicalModule;
      LANGULUS_BASES(Physical);
      using Physical::Physical;
   };
   
   ///                                                                        
   ///   Abstract physical instance unit                                      
   ///                                                                        
   struct Instance : Physical {
      LANGULUS(PRODUCER) World;
      LANGULUS_BASES(Physical);
      using Physical::Physical;

      NOD() virtual bool  Cull(const Math::LOD&) const noexcept = 0;
      NOD() virtual Math::Level GetLevel() const noexcept = 0;
      NOD() virtual Math::Mat4  GetModelTransform(const Math::LOD&) const noexcept = 0;
      NOD() virtual Math::Mat4  GetModelTransform(const Math::Level& = {}) const noexcept = 0;
      NOD() virtual Math::Mat4  GetViewTransform(const Math::LOD&) const noexcept = 0;
      NOD() virtual Math::Mat4  GetViewTransform(const Math::Level& = {}) const noexcept = 0;
   };

} // namespace Langulus::A

namespace Langulus::CT
{

   /// A concept for any kind of a physical unit                              
   template<class T>
   concept Physical = DerivedFrom<T, A::Physical>;

} // namespace Langulus::CT
