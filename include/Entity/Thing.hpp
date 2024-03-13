///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "../../source/Thing.hpp"
#include "../../source/Thing.inl"
#include "../../source/Thing-Gather.inl"
#include "../../source/Thing-Seek.inl"
#include "../../source/Pin.inl"
#include "../../source/Runtime.hpp"


namespace Langulus
{

   using Thing = Entity::Thing;
   using Runtime = Entity::Runtime;
   template<class T>
   using Pin = Entity::Pin<T>;

} // namespace Langulus
