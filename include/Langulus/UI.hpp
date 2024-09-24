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
   ///   Abstract UI module                                                   
   ///                                                                        
   struct UIModule : virtual A::Module {
      LANGULUS_BASES(A::Module);
      UIModule() : Resolvable {this}, A::Module {nullptr} {}
   };

   ///                                                                        
   ///   Abstract UI unit                                                     
   ///                                                                        
   struct UIUnit : virtual A::Unit {
      LANGULUS(PRODUCER) UIModule;
      LANGULUS_BASES(A::Unit);
      UIUnit() : Resolvable {this} {}
   };

   ///                                                                        
   ///   Abstract UI system                                                   
   ///                                                                        
   struct UISystem : virtual UIUnit {
      LANGULUS_BASES(UIUnit);
      UISystem() : Resolvable {this} {}
   };

   ///                                                                        
   ///   Abstract UI button                                                   
   ///                                                                        
   struct UIButton : virtual UIUnit {
      LANGULUS(PRODUCER) UISystem;
      LANGULUS_BASES(UIUnit);
      UIButton() : Resolvable {this} {}
   };

   ///                                                                        
   ///   Abstract UI text field                                               
   ///                                                                        
   struct UILabel : virtual UIUnit {
      LANGULUS(PRODUCER) UISystem;
      LANGULUS_BASES(UIUnit);
      UILabel() : Resolvable {this} {}
   };

   ///                                                                        
   ///   Abstract UI input field                                              
   ///                                                                        
   struct UIInput : virtual UIUnit {
      LANGULUS(PRODUCER) UISystem;
      LANGULUS_BASES(UIUnit);
      UIInput() : Resolvable {this} {}
   };

} // namespace Langulus::A