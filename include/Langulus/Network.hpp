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
   ///   Abstract network module                                              
   ///                                                                        
   struct Network : virtual Module {
      LANGULUS_BASES(Module);
      Network() : Resolvable {this}, Module {nullptr} {}
   };

   ///                                                                        
   ///   Abstract network unit                                                
   ///                                                                        
   struct NetworkUnit : virtual Unit {
      LANGULUS(PRODUCER) Network;
      LANGULUS_BASES(Unit);
      NetworkUnit() : Resolvable {this} {}
   };

   ///                                                                        
   ///   Abstract network client                                              
   ///                                                                        
   struct Client : virtual NetworkUnit {
      LANGULUS_BASES(NetworkUnit);
      Client() : Resolvable {this} {}
   };
   
   ///                                                                        
   ///   Abstract network server                                              
   ///                                                                        
   struct Server : virtual NetworkUnit {
      LANGULUS_BASES(NetworkUnit);
      Server() : Resolvable {this} {}
   };
   
   ///                                                                        
   ///   Abstract network shared object                                       
   ///                                                                        
   struct Shared : virtual NetworkUnit {
      LANGULUS(PRODUCER) NetworkUnit;
      LANGULUS_BASES(NetworkUnit);
      Shared() : Resolvable {this} {}
   };

} // namespace Langulus::A