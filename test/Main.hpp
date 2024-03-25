///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include <Langulus.hpp>

using namespace Langulus;

#define CATCH_CONFIG_ENABLE_BENCHMARKING


/// A unit implementation for testing                                         
class TestUnit1 final : public A::Unit {
public:
   LANGULUS(ABSTRACT) false;
   LANGULUS_BASES(Unit);
   IF_LANGULUS_MANAGED_MEMORY(LANGULUS(POOL_TACTIC) RTTI::PoolTactic::Type);

   TestUnit1()
      : Resolvable {MetaOf<TestUnit1>()} {}

   TestUnit1(Describe&& describe)
      : Resolvable {MetaOf<TestUnit1>()} {
      Couple(*describe);
   }

   ~TestUnit1() {
      Logger::Verbose(this, ": destroying...");
   }

   void Refresh() {}
};

/// A unit implementation for testing                                         
class TestUnit2 final : public A::Unit {
public:
   LANGULUS(ABSTRACT) false;
   LANGULUS_BASES(Unit);
   IF_LANGULUS_MANAGED_MEMORY(LANGULUS(POOL_TACTIC) RTTI::PoolTactic::Type);

   TestUnit2()
      : Resolvable {MetaOf<TestUnit2>()} {}

   TestUnit2(Describe&& describe)
      : Resolvable {MetaOf<TestUnit2>()} {
      Couple(*describe);
   }

   ~TestUnit2() {
      Logger::Verbose(this, ": destroying...");
   }

   void Refresh() {}
};