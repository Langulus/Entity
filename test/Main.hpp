#pragma once
#include <Entity/External.hpp>

using namespace Langulus;
using namespace Langulus::Entity;

#define CATCH_CONFIG_ENABLE_BENCHMARKING

class TestUnit1 : public Unit {
public:
   LANGULUS(ABSTRACT) false;
   LANGULUS_BASES(Unit);

   TestUnit1(const Any& a = {})
      : Unit {MetaOf<TestUnit1>(), a} {}

   void Refresh() final {}
};

class TestUnit2 : public Unit {
public:
   LANGULUS(ABSTRACT) false;
   LANGULUS_BASES(Unit);

   TestUnit2(const Any& a = {})
      : Unit {MetaOf<TestUnit2>(), a} {}

   void Refresh() final {}
};