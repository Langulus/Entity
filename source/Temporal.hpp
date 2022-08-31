#pragma once
#include "Unit.hpp"

namespace Langulus::Entity
{

	///																								
	///	Temporal flow																			
	///																								
	///	This controller keeps track of contexts, and integrates verbs to		
	/// the current one. It gives temporality to anything, by providing the		
	/// main time gradient. It registers all actions, such as frequent ones,	
	/// and automatically executes them on	update. It has buckets for verbs		
	/// that occur at specific time or period. An analogy for a flow is a git	
	/// repo where you record all changes (executed actions). You can,			
	/// therefore, fork, branch, etc., with the addition of periodic commits.	
	///																								
	class Temporal  {
	public:
		Temporal(const Temporal&) = delete;
		Temporal(Temporal* parent = nullptr);
		Temporal(Temporal&&) noexcept = default;

		Temporal& operator = (Temporal&&) noexcept = default;
		Temporal& operator = (const Temporal&) = delete;

	public:
		NOD() bool operator == (const Temporal&) const;

		NOD() Temporal Clone() const;
		NOD() bool IsValid() const;

		void Dump() const;
		void Merge(const Temporal&);
		bool Push(const Any&);

		void Reset();
		void Update(Block&, Time);
		void Execute(Block&, Time timeOffset = {}, Time timePeriod = {});

	private:
		// Increments on each call to Update()										
		TimePoint mPreviousTime;
		TimePoint mCurrentTime;

		// Flow duration in moments, due to verbs added to time stacks		
		Time mDuration {};

		// Priority stack																	
		Any mPriorityStack;
		// Verb temporal stack - flows that trigger at given time			
		THashMap<Time, Temporal*> mTimeStack;
		// Verb frequency stack - flows that trigger periodically			
		THashMap<Time, Temporal*> mFrequencyStack;
	};

} // namespace Langulus::Entity

