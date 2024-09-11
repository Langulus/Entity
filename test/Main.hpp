///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include <Langulus.hpp>

using namespace Langulus;


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

/// Dump parse results and requirements                                       
template<class INPUT, class OUTPUT, class REQUIRED>
void DumpResults(const INPUT& in, const OUTPUT& out, const REQUIRED& required) {
   Logger::Special("-------------");
   Logger::Special("Script:   ", in);
   Logger::Special("Parsed:   ", out);
   Logger::Special("Required: ", required);
   Logger::Special("-------------");
}

