///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Common.hpp"
#include <Entity/Event.hpp>
#include <Math/Vector.hpp>
#include <Math/Verbs/Add.hpp>


SCENARIO("Testing Event", "[event]") {
   (void) MetaDataOf<Event>();
   (void) MetaDataOf<EventState>();
   (void) MetaTraitOf<Traits::X>();
   (void) MetaTraitOf<Traits::Y>();
   (void) MetaVerbOf<Verbs::Add>();

   GIVEN("The script: Create Event(Keys::A, EventState::Begin, Data(X(666), Y(999))),"
                           " Event(Keys::D, EventState::Begin, Data(X(666), Y(999)))"
   ) {
      const Code code = "Create("
         " Event(Keys::A, EventState::Begin, Data(X(666),   Y(999))),"
         " Event(Keys::D, EventState::End,   Data(X(666+1), Y(999+1))))";

      const Many required = Verbs::Create { Many::Wrap<Event>(
         Keys::A {EventState::Begin, Many::Wrap<Trait>(Traits::X {666_real}, Traits::Y { 999_real})},
         Keys::D {EventState::End,   Many::Wrap<Trait>(Traits::X {667_real}, Traits::Y {1000_real})}
      )};

      WHEN("Parsed") {
         const auto parsed = code.Parse();
         DumpResults(code, parsed, required);
         REQUIRE(parsed == required);
      }
   }

   GIVEN("The script: Create A(EventState::Begin, Data(X(666), Y(999))),"
                           " D(EventState::Begin, Data(X(666), Y(999)))"
   ) {
      const Code code = "Create("
         " A(EventState::Begin, Data(X(666),   Y(999))),"
         " D(EventState::End,   Data(X(666+1), Y(999+1))))";

      const Many required = Verbs::Create {
         Keys::A {EventState::Begin, Many::Wrap<Trait>(Traits::X {666_real}, Traits::Y { 999_real})},
         Keys::D {EventState::End,   Many::Wrap<Trait>(Traits::X {667_real}, Traits::Y {1000_real})}
      };

      WHEN("Parsed") {
         const auto parsed = code.Parse();
         DumpResults(code, parsed, required);
         REQUIRE(parsed == required);
      }
   }
}