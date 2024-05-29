///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Entity/Thing.hpp"
#include "../Entity/Event.hpp"


namespace Langulus::A
{

   ///                                                                        
   ///   Abstract input module                                                
   ///                                                                        
   struct InputModule : virtual Module {
      LANGULUS_BASES(Module);
      InputModule() : Resolvable {this}, Module {nullptr} {}
   };

   ///                                                                        
   ///   Abstract input unit                                                  
   ///                                                                        
   struct Input : virtual Unit {
      LANGULUS_BASES(Unit);
      Input() : Resolvable {this} {}
   };

   ///                                                                        
   ///   Abstract input gatherer                                              
   ///                                                                        
   struct InputGatherer : virtual Input {
      LANGULUS(PRODUCER) InputModule;
      LANGULUS_BASES(Input);

   protected:
      // Accumulated events, dispatched on each Update() to listeners   
      EventList mEventQueue;

   public:
      InputGatherer() : Resolvable {this} {}

      void PushEvent(const Event& e) {
         const auto foundEvent = mEventQueue.FindIt(e.mType);
         if (foundEvent) {
            const auto foundState = foundEvent.mValue->FindIt(e.mState);
            if (foundState) {
               // Event already exists, merge payload                   
               foundState.mValue->mPayload += e.mPayload;
               return;
            }
            foundEvent.mValue->Insert(e.mState, e);
         }
         else {
            mEventQueue.Insert(e.mType);
            auto& newGroup = mEventQueue[e.mType];
            newGroup.Insert(e.mState, e);
         }
      }
   };

   ///                                                                        
   ///   Abstract input listener                                              
   ///                                                                        
   struct InputListener : virtual Input {
      LANGULUS(PRODUCER) InputGatherer;
      LANGULUS_BASES(Input);
      InputListener() : Resolvable {this} {}
   };

} // namespace Langulus::A
