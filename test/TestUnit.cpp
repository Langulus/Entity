///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Common.hpp"


SCENARIO("Testing Unit", "[unit]") {
   WHEN("Dynamically casting to/from unit pointers") {
      TestUnit1 t1;
      TestUnit2 t2;
      A::Unit* t1p = &t1;
      A::Unit* t2p = &t2;

      REQUIRE(reinterpret_cast<void*>(t1p) == reinterpret_cast<void*>(&t1));
      REQUIRE(reinterpret_cast<void*>(t2p) == reinterpret_cast<void*>(&t2));
      REQUIRE(dynamic_cast<TestUnit1*>(t1p) == &t1);
      REQUIRE(dynamic_cast<TestUnit2*>(t1p) == nullptr);
      REQUIRE(dynamic_cast<TestUnit2*>(t2p) == &t2);
      REQUIRE(dynamic_cast<TestUnit1*>(t2p) == nullptr);
   }
}