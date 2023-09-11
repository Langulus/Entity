///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Pinnable.hpp"

#define TEMPLATE() template<class T>
#define PINNED() Pinnable<T>

namespace Langulus::Entity
{

   /// Descriptor-constructor for base type                                   
   ///   @param desc - the descriptor to use                                  
   TEMPLATE() LANGULUS(INLINED)
   PINNED()::Pinnable(const Neat& neat) requires (CT::DescriptorMakable<T>)
      : T {neat} {}

   /// Check if the value is pinned                                           
   ///   @return true if value is pinned                                      
   TEMPLATE() LANGULUS(INLINED)
   bool PINNED()::IsPinned() const noexcept {
      return mLocked;
   }

   /// Pin the value, it will no longer be overwritable by environment        
   TEMPLATE() LANGULUS(INLINED)
   void PINNED()::Pin() noexcept {
      mLocked = true;
   }

   /// Unpin the value, it now can be overwritten by environment              
   TEMPLATE() LANGULUS(INLINED)
   void PINNED()::Unpin() noexcept {
      mLocked = false;
   }
   
} // namespace namespace Langulus::Entity

#undef TEMPLATE
#undef PINNED
