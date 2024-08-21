///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Common.hpp"
#include <Flow/Temporal.hpp>


SCENARIO("Temporal flow", "[temporal]") {
   static Allocator::State memoryState;

   ///                                                                        
   /// The following tests rely on this ontology sequence                     
   const auto HI = "create Thing(Name(`<unnamed user>`))"_parse;
   Logger::Info("HI           -> ", Logger::Cyan, HI);

   const auto COMMA = "(number? >< number??) or (? Conjunct!4 ??)"_parse;
   Logger::Info("COMMA        -> ", Logger::Cyan, COMMA);

   const auto DOT = "? Conjunct!8 ??"_parse;
   Logger::Info("DOT          -> ", Logger::Cyan, DOT);

   const auto MY = "?.Thing(Name).??"_parse;
   Logger::Info("MY           -> ", Logger::Cyan, MY);

   const auto NAME = "Name"_parse;
   Logger::Info("NAME         -> ", Logger::Cyan, NAME);

   const auto IS = "? = ??"_parse;
   Logger::Info("IS           -> ", Logger::Cyan, IS);

   const auto APOSTROPHE_S = "(? = ??) or (?.Thing(Name)) or (?.??)"_parse;
   Logger::Info("APOSTROPHE_S -> ", Logger::Cyan, APOSTROPHE_S);

   const auto MAKE = "? create ??"_parse;
   Logger::Info("MAKE         -> ", Logger::Cyan, MAKE);

   const auto AA = "`a`, single"_parse;
   Logger::Info("AA           -> ", Logger::Cyan, AA);

   const auto GAME = "`game`, Thing(Temporal, Name(`<unnamed game>`))"_parse;
   Logger::Info("GAME         -> ", Logger::Cyan, GAME);

   const auto CALLED = "? create Name(a::text??)"_parse;
   Logger::Info("CALLED       -> ", Logger::Cyan, CALLED);


   GIVEN("An empty temporal flow") {
      auto root = Thing::Root();
      auto& flow = root.GetFlow();

      WHEN("Code is pushed to the flow") {
         flow->Push(HI, COMMA, " ", MY, " ", NAME, " ", IS, " Dimo", DOT);
         flow->Push("let", APOSTROPHE_S, " ", MAKE, " ", AA, " ", GAME, " ", CALLED, " t", "e", "st", GAME);
      }
   }

   REQUIRE(memoryState.Assert());
}
