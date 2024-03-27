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
#include "../Entity/Module.hpp"


namespace Langulus::A::UI
{

   ///                                                                        
   ///   Abstract UI module                                                   
   ///                                                                        
   struct Module : virtual A::Module {
      LANGULUS_BASES(A::Module);
      Module() : Resolvable {this}, A::Module {nullptr} {}
   };

   ///                                                                        
   ///   Abstract UI unit                                                     
   ///                                                                        
   struct Unit : virtual A::Unit {
      LANGULUS(PRODUCER) UI::Module;
      LANGULUS_BASES(A::Unit);
      Unit() : Resolvable {this} {}
   };

   ///                                                                        
   ///   Abstract UI system                                                   
   ///                                                                        
   struct System : virtual UI::Unit {
      LANGULUS_BASES(UI::Unit);
      System() : Resolvable {this} {}
   };

   ///                                                                        
   ///   Abstract UI button                                                   
   ///                                                                        
   struct Button : virtual UI::Unit {
      LANGULUS(PRODUCER) UI::System;
      LANGULUS_BASES(UI::Unit);
      Button() : Resolvable {this} {}
   };

   ///                                                                        
   ///   Abstract UI text field                                               
   ///                                                                        
   struct Text : virtual UI::Unit {
      LANGULUS(PRODUCER) UI::System;
      LANGULUS_BASES(UI::Unit);
      Text() : Resolvable {this} {}
   };

   ///                                                                        
   ///   Abstract UI input field                                              
   ///                                                                        
   struct Input : virtual UI::Unit {
      LANGULUS(PRODUCER) UI::System;
      LANGULUS_BASES(UI::Unit);
      Input() : Resolvable {this} {}
   };

} // namespace Langulus::A::UI