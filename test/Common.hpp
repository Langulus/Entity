///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           

/// INTENTIONALLY NOT GUARDED                                                 
/// Include this file once in each cpp file, after all other headers          
#include <Langulus/Entity/Thing.hpp>
#include <Langulus/Testing.hpp>


/// A unit implementation for testing                                         
class TestUnit1 final : public A::Unit {
public:
   LANGULUS(ABSTRACT) false;
   LANGULUS_BASES(Unit);
   LANGULUS(POOL_TACTIC) RTTI::PoolTactic::Type;

   TestUnit1() : Resolvable {this} {}

   TestUnit1(Describe&& describe)
      : Resolvable {this} {
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
   LANGULUS(POOL_TACTIC) RTTI::PoolTactic::Type;

   TestUnit2() : Resolvable {this} {}

   TestUnit2(Describe&& describe)
      : Resolvable {this} {
      Couple(*describe);
   }

   ~TestUnit2() {
      Logger::Verbose(this, ": destroying...");
   }

   void Refresh() {}
};