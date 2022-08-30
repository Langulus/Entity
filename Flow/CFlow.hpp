#pragma once
#include "../Hierarchy/AUnit.hpp"

namespace PCFW
{

	///																								
	///	UNIVERSAL FLOW																			
	///																								
	/// This controller keeps track of contexts, and integrates verbs to			
	/// the current one. It gives temporality to anything, by providing the		
	/// main time gradient. It registers all actions, such as frequent ones,	
	/// and automatically executes them on	update. It has buckets for verbs		
	/// that occur at specific time or period. An analogy for a flow is a git	
	/// repo where you record all changes (executed actions). You can,			
	/// therefore, fork, branch, etc.														
	///																								
	class LANGULUS_MODULE(FRAMEWORK) CFlow : public AUnit, public TProducedFrom<CFlow> {
		REFLECT(CFlow);
	public:
		CFlow(const CFlow&) = delete;
		CFlow(CFlow* parent = nullptr);
		CFlow(CFlow&&) noexcept = default;

		CFlow& operator = (CFlow&&) noexcept = default;
		CFlow& operator = (const CFlow&) = delete;

	public:
		NOD() bool operator == (const CFlow&) const;

		NOD() ME Clone() const;
		NOD() bool IsValid() const;

		void Dump() const;
		void Merge(const CFlow&);
		bool Push(const Any&);

		void Reset();
		void Update(Block&, PCTime);
		void Execute(Block&, PCTime timeOffset = {}, PCTime timePeriod = {});

		NOD() inline auto& GetPriorityStack() noexcept { return mPriorityStack; }
		NOD() inline auto& GetPriorityStack() const noexcept { return mPriorityStack; }

	private:
		// Increments on each call to Update()										
		TGrad2<PCTime> mTimeFromInit;
		// Flow duration in moments, due to verbs added to time stacks		
		PCTime mDuration = 0;

		// Priority stack																	
		Any mPriorityStack;
		// Verb temporal stack - flows that trigger at given time			
		TMap<PCTime, CFlow*> mTimeStack;
		// Verb frequency stack - flows that trigger periodically			
		TMap<PCTime, CFlow*> mFrequencyStack;
	};

} // namespace PCFW

