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
      Module()
         : Resolvable {DMeta {}}
         , A::Module {nullptr} {}
   };

   ///                                                                        
   ///   Abstract UI unit                                                     
   ///                                                                        
   struct Unit : virtual A::Unit {
      LANGULUS(PRODUCER) UI::Module;
      LANGULUS_BASES(A::Unit);
      Unit()
         : Resolvable {DMeta {}} {}
   };

   ///                                                                        
   ///   Abstract UI system                                                   
   ///                                                                        
   struct System : virtual UI::Unit {
      LANGULUS_BASES(UI::Unit);
      System()
         : Resolvable {DMeta {}} {}
   };

   ///                                                                        
   ///   Abstract UI button                                                   
   ///                                                                        
   struct Button : virtual UI::Unit {
      LANGULUS(PRODUCER) UI::System;
      LANGULUS_BASES(UI::Unit);
      Button()
         : Resolvable {DMeta {}} {}
   };

   ///                                                                        
   ///   Abstract UI text field                                               
   ///                                                                        
   struct Text : virtual UI::Unit {
      LANGULUS(PRODUCER) UI::System;
      LANGULUS_BASES(UI::Unit);
      Text()
         : Resolvable {DMeta {}} {}
   };

   ///                                                                        
   ///   Abstract UI input field                                              
   ///                                                                        
   struct Input : virtual UI::Unit {
      LANGULUS(PRODUCER) UI::System;
      LANGULUS_BASES(UI::Unit);
      Input()
         : Resolvable {DMeta {}} {}
   };

} // namespace Langulus::A::UI