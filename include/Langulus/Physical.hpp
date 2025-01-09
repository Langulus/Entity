///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Entity/Thing.hpp"
#include <Math/LOD.hpp>
#include <Math/Level.hpp>
#include <Math/Matrix.hpp>
#include <Math/Color.hpp>


namespace Langulus::A
{

   ///                                                                        
   ///   Abstract physical module                                             
   ///                                                                        
   struct PhysicalModule : virtual Module {
      LANGULUS_BASES(Module);
      PhysicalModule() : Resolvable {this}, Module {nullptr} {}
   };

   ///                                                                        
   ///   Abstract physical unit                                               
   ///                                                                        
   struct Physical : virtual Unit {
      LANGULUS_BASES(Unit);
      Physical() : Resolvable {this} {}
   };

   ///                                                                        
   ///   Abstract physical world unit                                         
   ///                                                                        
   struct World : virtual Physical {
      LANGULUS(PRODUCER) PhysicalModule;
      LANGULUS_BASES(Physical);
      World() : Resolvable {this} {}
   };
   
   ///                                                                        
   ///   Abstract physical instance unit                                      
   ///                                                                        
   struct Instance : virtual Physical {
      LANGULUS(PRODUCER) World;
      LANGULUS_BASES(Physical);
      Instance() : Resolvable {this} {}

      using LOD   = Math::LOD;
      using Level = Math::Level;
      using Mat4  = Math::Mat4;
      using RGBA  = Math::RGBA;

      virtual bool Cull(const LOD&) const noexcept = 0;
      virtual auto GetLevel() const noexcept -> Level = 0;
      virtual auto GetModelTransform(const LOD&) const noexcept -> Mat4 = 0;
      virtual auto GetModelTransform(const Level& = {}) const noexcept -> Mat4 = 0;
      virtual auto GetViewTransform(const LOD&) const noexcept -> Mat4 = 0;
      virtual auto GetViewTransform(const Level& = {}) const noexcept -> Mat4 = 0;
      virtual auto GetColor() const noexcept -> RGBA = 0;
   };

} // namespace Langulus::A

namespace Langulus::CT
{

   /// A concept for any kind of a physical unit                              
   template<class T>
   concept Physical = DerivedFrom<T, A::Physical>;

} // namespace Langulus::CT
