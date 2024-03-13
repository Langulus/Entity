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
   struct Module : A::Module {
      LANGULUS_BASES(A::Module);
      using A::Module::Module;
   };

   ///                                                                        
   ///   Abstract UI unit                                                     
   ///                                                                        
   struct Unit : A::Unit {
      LANGULUS(PRODUCER) UI::Module;
      LANGULUS_BASES(A::Unit);
      using A::Unit::Unit;
   };

   ///                                                                        
   ///   Abstract UI system                                                   
   ///                                                                        
   struct System : UI::Unit {
      LANGULUS_BASES(UI::Unit);
      using UI::Unit::Unit;
   };

   ///                                                                        
   ///   Abstract UI button                                                   
   ///                                                                        
   struct Button : UI::Unit {
      LANGULUS(PRODUCER) UI::System;
      LANGULUS_BASES(UI::Unit);
      using UI::Unit::Unit;
   };

   ///                                                                        
   ///   Abstract UI text field                                               
   ///                                                                        
   struct Text : UI::Unit {
      LANGULUS(PRODUCER) UI::System;
      LANGULUS_BASES(UI::Unit);
      using UI::Unit::Unit;
   };

   ///                                                                        
   ///   Abstract UI input field                                              
   ///                                                                        
   struct Input : UI::Unit {
      LANGULUS(PRODUCER) UI::System;
      LANGULUS_BASES(UI::Unit);
      using UI::Unit::Unit;
   };

} // namespace Langulus::A::UI