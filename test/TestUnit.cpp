///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#include "Main.hpp"
#include <catch2/catch.hpp>

/// See https://github.com/catchorg/Catch2/blob/devel/docs/tostring.md			
CATCH_TRANSLATE_EXCEPTION(::Langulus::Exception const& ex) {
	const Text serialized {ex};
	return ::std::string {Token {serialized}};
}

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