///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Entity/Thing.hpp"
#include "../Entity/Module.hpp"


namespace Langulus::A
{

   ///                                                                        
   ///   Abstract artificial intelligence module                              
   ///                                                                        
   struct AIModule : virtual Module {
      LANGULUS_BASES(AIModule);
      AIModule() : Resolvable {this}, Module {nullptr} {}
   };

   ///                                                                        
   ///   Abstract artificial intelligence unit                                
   ///                                                                        
   struct AIUnit : virtual Unit {
      LANGULUS(PRODUCER) AIModule;
      LANGULUS_BASES(Unit);
      AIUnit() : Resolvable {this} {}
   };

   ///                                                                        
   ///   Abstract mind                                                        
   ///                                                                        
   struct Mind : virtual AIUnit {
      LANGULUS_BASES(AIUnit);
      Mind() : Resolvable {this} {}
   };

} // namespace Langulus::A