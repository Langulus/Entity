///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Event.hpp"


namespace Langulus
{

   /// Construct from internal type                                           
   ///   @param state - the state to set                                      
   LANGULUS(INLINED)
   constexpr EventState::EventState(const Type& state) noexcept
      : mState {state} {}

   /// Check if the event isn't default (isn't immediate)                     
   LANGULUS(INLINED)
   constexpr EventState::operator bool() const noexcept {
      return mState != Enum::Point;
   }

   /// Operator for combining states (nondestructively)                       
   ///   @param state - the state combination to add                          
   ///   @return the combined state                                           
   LANGULUS(INLINED)
   constexpr EventState EventState::operator + (const EventState& state) const noexcept {
      return {mState | state.mState};
   }

   /// Operator for excluding states (nondestructively)                       
   ///   @param state - the state combination to remove                       
   ///   @return the combined state                                           
   LANGULUS(INLINED)
   constexpr EventState EventState::operator - (const EventState& state) const noexcept {
      return {mState & (~state.mState)};
   }

   /// Operator for adding (destructively) states to this one                 
   ///   @param state - the state combination to add                          
   ///   @return a reference to this state                                    
   LANGULUS(INLINED)
   constexpr EventState& EventState::operator += (const EventState& state) noexcept {
      mState |= state.mState;
      return *this;
   }

   /// Operator for removing (destructively) states from this one             
   ///   @param state - the state combination to remove                       
   ///   @return a reference to this state                                    
   LANGULUS(INLINED)
   constexpr EventState& EventState::operator -= (const EventState& state) noexcept {
      mState &= ~state.mState;
      return *this;
   }

   /// Operator for checking if a combination of states is enabled            
   ///   @param state - the state combination to check                        
   ///   @return true if all the provided states are enabled                  
   LANGULUS(INLINED)
   constexpr bool EventState::operator & (const EventState& state) const noexcept {
      return state == (mState & state.mState);
   }

   /// Operator for checking if a combination of states is disabled           
   ///   @param state - the state combination to check                        
   ///   @return true if all the provided states are disabled                 
   LANGULUS(INLINED)
   constexpr bool EventState::operator % (const EventState& state) const noexcept {
      return 0 == (mState & state.mState);
   }

   /// Is this event immediate (has no beginning and end)                     
   LANGULUS(INLINED)
   constexpr bool EventState::IsPoint() const noexcept {
      return mState == Enum::Point;
   }

   /// Is this the beginning of an event?                                     
   LANGULUS(INLINED)
   constexpr bool EventState::IsBegin() const noexcept {
      return mState == Enum::Begin;
   }

   /// Is this the end of a started event?                                    
   LANGULUS(INLINED)
   constexpr bool EventState::IsEnd() const noexcept {
      return mState == Enum::End;
   }

   /// Reset the state                                                        
   LANGULUS(INLINED)
   constexpr void EventState::Reset() noexcept {
      mState = 0;
   }


   /// Default event construction creates a new timestamp                     
   LANGULUS(INLINED)
   Event::Event()
      : mTimestamp {SteadyClock::Now()} {}

   /// Refer-construction                                                     
   ///   @param other - event properties and payload to refer to              
   LANGULUS(INLINED)
   Event::Event(const Event& other)
      : Event {Refer(other)} {}

   /// Move-construction                                                      
   ///   @param other - event properties and payload to move                  
   LANGULUS(INLINED)
   Event::Event(Event&& other)
      : Event {Move(other)} {}

   /// Descriptor-construction                                                
   ///   @param other - event properties and payload to move                  
   LANGULUS(INLINED)
   Event::Event(Describe desc) : Event {} {
      LANGULUS_ASSERT(desc->ExtractData(mType), Construct, "Invalid event");
      desc->ExtractData(mState);
      desc->ExtractTrait<Traits::Data>(mPayload);
   }

   /// Instantiate an event of a specific type, manually                      
   /// This constructor also generates a new timestamp if not absorbing       
   ///   @param a... - any number of arguments to carry in the event          
   template<class T1, class...TN> requires CT::UnfoldInsertable<T1, TN...>
   Event::Event(T1&& t1, TN&&...tn) : Event {} {
      if constexpr (sizeof...(TN) == 0 and not CT::Array<T1>) {
         using S = IntentOf<decltype(t1)>;
         using T = TypeOf<S>;

         if constexpr (CT::EventBased<T>) {
            mType = DeintCast(t1).mType;
            mState = DeintCast(t1).mState;
            mTimestamp = DeintCast(t1).mTimestamp;
            mPayload = S::Nest(DeintCast(t1).mPayload);
         }
         else mPayload = Many {Forward<T1>(t1)};
      }
      else mPayload = Many {Forward<T1>(t1), Forward<TN>(tn)...};
   }

   /// Refer-assignment                                                       
   ///   @attention will not overwrite timestamp                              
   ///   @param other - the event to refer to                                 
   ///   @return a reference to this event                                    
   LANGULUS(INLINED)
   Event& Event::operator = (const Event& rhs) {
      return operator = (Refer(rhs));
   }

   /// Move-assignment                                                        
   ///   @attention will not overwrite timestamp                              
   ///   @param other - the event to move                                     
   ///   @return a reference to this event                                    
   LANGULUS(INLINED)
   Event& Event::operator = (Event&& rhs) noexcept {
      return operator = (Move(rhs));
   }

   /// Generic assignment. If argument is event, it will be absorbed          
   ///   @attention will not overwrite timestamp, unless absorbing            
   ///   @param rhs - right hand side and intent                              
   ///   @return a reference to this event                                    
   LANGULUS(INLINED)
   Event& Event::operator = (CT::UnfoldInsertable auto&& rhs) {
      using S = IntentOf<decltype(rhs)>;
      using T = TypeOf<S>;

      if constexpr (CT::EventBased<T>) {
         mType = DeintCast(rhs).mType;
         mState = DeintCast(rhs).mState;
         mTimestamp = DeintCast(rhs).mTimestamp;
         mPayload = S::Nest(DeintCast(rhs).mPayload);
      }
      else mPayload = Many {S::Nest(rhs)};
      return *this;
   }

   /// Compare two events                                                     
   ///   @attention will not compare timestamps                               
   ///   @param rhs - the event to compare against                            
   ///   @return true if events are the same                                  
   inline bool Event::operator == (const Event& rhs) const {
      return mType == rhs.mType and mState == rhs.mState
         and mPayload == rhs.mPayload;
   }

} // namespace Langulus