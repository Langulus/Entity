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
   ///   Abstract life module                                                 
   ///                                                                        
   struct Life : virtual Module {
      LANGULUS_BASES(Module);
      Life() : Resolvable {this}, Module {nullptr} {}
   };

   ///                                                                        
   ///   Abstract life unit                                                   
   ///                                                                        
   struct LifeUnit : virtual Unit {
      LANGULUS(PRODUCER) Life;
      LANGULUS_BASES(Unit);
      LifeUnit() : Resolvable {this} {}
   };

   ///                                                                        
   ///   Abstract ecosystem                                                   
   ///                                                                        
   struct Ecosystem : virtual LifeUnit {
      LANGULUS_BASES(LifeUnit);
      Ecosystem() : Resolvable {this} {}
   };
   
   ///                                                                        
   ///   Abstract organism                                                    
   ///                                                                        
   struct Organism : virtual LifeUnit {
      LANGULUS(PRODUCER) Ecosystem;
      LANGULUS_BASES(LifeUnit);
      Organism() : Resolvable {this} {}
   };

} // namespace Langulus::A