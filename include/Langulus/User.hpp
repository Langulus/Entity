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
   ///   Abstract user module                                                 
   ///                                                                        
   struct UserModule : virtual Module {
      LANGULUS_BASES(Module);
      UserModule() : Resolvable {this}, Module {nullptr} {}
   };

   ///                                                                        
   ///   Abstract user unit                                                   
   ///                                                                        
   struct User : virtual Unit {
      LANGULUS(PRODUCER) UserModule;
      LANGULUS_BASES(Unit);
      User() : Resolvable {this} {}
   };

} // namespace Langulus::A