///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include <Entity/External.hpp>

using namespace Langulus;
using namespace Langulus::Entity;

#define CATCH_CONFIG_ENABLE_BENCHMARKING

class TestUnit1 : public Unit {
public:
   LANGULUS(ABSTRACT) false;
   LANGULUS_BASES(Unit);

   TestUnit1()
      : Unit {MetaOf<TestUnit1>()} {}

   TestUnit1(Describe&& describe)
      : Unit {MetaOf<TestUnit1>(), *describe} {}

   void Refresh() final {}
};

class TestUnit2 : public Unit {
public:
   LANGULUS(ABSTRACT) false;
   LANGULUS_BASES(Unit);

   TestUnit2()
      : Unit {MetaOf<TestUnit2>()} {}

   TestUnit2(Describe&& describe)
      : Unit {MetaOf<TestUnit2>(), *describe} {}

   void Refresh() final {}
};