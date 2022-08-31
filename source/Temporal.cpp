#include "Entity.hpp"
#include "Runtime.hpp"
#include "CFlow.hpp"
#include "MissingFlow.hpp"

#define PC_CFLOW_VERBOSE(a) pcLogVerbose << a
#define PC_CFLOW_VERBOSE_TAB(a) ScopedTab tab; pcLogVerbose << a << tab

namespace Langulus::Entity
{

	Pasts FindPastPoints(Any&);

	/// Construct as a sub-flow																
	///	@param parent - flow producer and parent										
	CFlow::CFlow(CFlow* parent)
		: AUnit{ MetaData::Of<CFlow>() }
		, TProducedFrom{ parent } {
		ClassValidate();
	}

	/// This will essentially fork the parent flow, if any							
	///	@return the cloned flow																
	CFlow CFlow::Clone() const {
		CFlow clone;
		// Note, that the producer and owners are not cloned or copied		
		clone.mFrequencyStack = mFrequencyStack.Clone();
		clone.mTimeStack = mTimeStack.Clone();
		clone.mPriorityStack = mPriorityStack.Clone();
		clone.mTimeFromInit = mTimeFromInit;
		clone.mDuration = mDuration;
		return clone;
	}

	/// Reset progress for the priority stack												
	void CFlow::Reset() {
		mTimeFromInit.Reset({});
		PC_CFLOW_VERBOSE("Reset");
	}
	
	/// Compare two flows																		
	///	@param other - the flow to compare with										
	///	@return true if both flows are the same										
	bool CFlow::operator == (const CFlow& other) const {
		return mFrequencyStack == other.mFrequencyStack
			&& mTimeStack == other.mTimeStack
			&& mPriorityStack == other.mPriorityStack;
	}

	/// Check if flow contains anything executable										
	///	@return true if flow contains at least one verb								
	bool CFlow::IsValid() const {
		return !mFrequencyStack.IsEmpty()
			|| !mTimeStack.IsEmpty()
			|| !mPriorityStack.IsEmpty();
	}

	/// Dump the contents of the flow to the log											
	void CFlow::Dump() const {
		pcLogSelfVerbose << mPriorityStack;
		pcLogSelfVerbose << mTimeStack;
		pcLogSelfVerbose << mFrequencyStack;
	}

	/// Remove all missing states inside all output's subpacks						
	///	@param output - [in/out] the pack to fully iterate							
	void ResetPointState(Block& output) {
		output.ForEachDeep<false>([](Block& part) {
			part.ToggleState(DState::Missing, false);
			part.ToggleState(DState::Polar, false);
			part.ToggleState(DState::Positive, false);
			EitherDoThis
				part.ForEach([](Verb& v) {
					ResetPointState(v.GetSource());
					ResetPointState(v.GetArgument());
					ResetPointState(v.GetOutput());
				})
			OrThis
				part.ForEach([](Trait& v) {
					ResetPointState(v);
				})
			OrThis
				part.ForEach([](Construct& v) {
					ResetPointState(v.GetAll());
				});
		});
	}

	/// Remove all filters, polarization and missingness								
	/// Strictly typed contents will be finalized										
	///	@param scope - [in/out] the scope to prepare									
	void PrepareForExecution(Block& scope) {
		scope.ForEachDeep<false>([](Any& part) {
			if (part.IsMissing()) {
				if (part.IsDeep() && part.GetCount() == 2) {
					// Check if filter is of a single type and whether			
					// contents match that type or not								
					Any& filter = part.Get<Any>(0);
					Any content = part.Get<Any>(1);

					if (filter.GetCount() == 1 && !content.Is(filter.Get<DataID>())) {
						pcLogVerbose << "Finalizing " << content << " to " << filter.Get<DataID>();
						auto finalized = Any::From(filter.Get<DataID>().GetMeta());
						finalized.Allocate(1, true);
						auto catenator = Verb::From<Verbs::Catenate>({}, content);
						if (Verb::DispatchFlat(finalized, catenator, false)) {
							pcLogVerbose << "Finalized " << content << " to " << finalized;
							content = pcMove(finalized);
						}
					}

					part = pcMove(content);
				}
				else if (!part.IsDeep())
					part.Reset();

				part.ToggleState(DState::Missing, false);
				part.ToggleState(DState::Polar, false);
				part.ToggleState(DState::Positive, false);
			}

			EitherDoThis
				part.ForEach([](Verb& v) {
					PrepareForExecution(v.GetSource());
					PrepareForExecution(v.GetArgument());
					PrepareForExecution(v.GetOutput());
				})
			OrThis
				part.ForEach([](Trait& v) {
					PrepareForExecution(v);
				})
			OrThis
				part.ForEach([](Construct& v) {
					PrepareForExecution(v.GetAll());
				});
		});
	}

	/// Advance the flow - moves time forward, executes stacks						
	///	@param context - the context to execute any verbs							
	///	@param dt - delta time																
	void CFlow::Update(Block& context, PCTime dt) {
		if (IsClassDisabled())
			return;

		if (mTimeFromInit.Current() == 0) {
			// If we're at the beginning of time - prepare for execution	
			// This will omit any compilation-stage junk that remains in	
			// the priority stack - we no longer need it							
			PrepareForExecution(mPriorityStack);

			// Now execute the priority stack										
			// It will have two products:												
			//																					
			// 1. The outputs of the execution, that is, the verb outputs	
			//		and data propagation that happens in the stack				
			//		The outputs are short-lived, and may contain some			
			//		results to queries and other temporary data					
			//																					
			//	2. The context of the execution, that changes in its own		
			//		way, and will later be used to link the next pushes		
			//		to this flow. The context transcends a flow exection and	
			//		propagates to the next push										
			//																					
			Any output;
			Any localContext { context };
			if (!Verb::ExecuteScope(localContext, mPriorityStack, output)) {
				// oh-oh - an error occurs												
				throw Except::BadOperation(pcLogSelfError << "Update failed");
			}

			// Propagate the context to the next update							
			if (localContext.GetRaw() == context.GetRaw() && localContext.GetCount() == context.GetCount()) {
				mPriorityStack = Verb::From<Verbs::Do>(
					localContext,
					Any().MakeRight().MakeMissing()
				).SetPriority(8); //TODO contexts on multiple levels can be implemented via this priority modifier! Neat!
			}
			else {
				mPriorityStack = Verb::From<Verbs::Do>(
					Any::WrapOr(localContext, context),
					Any().MakeRight().MakeMissing()
				).SetPriority(8); //TODO contexts on multiple levels can be implemented via this priority modifier! Neat!
			}

			PC_CFLOW_VERBOSE(ccPurple 
				<< "Flow after execution " << mPriorityStack);
		}

		if (dt == 0) {
			// Avoid updating anything else, if no time had passed			
			return;
		}

		// Advance the global cycler for the flow									
		mTimeFromInit.Update() += dt;

		// Execute flows that occur periodically									
		mFrequencyStack.ForEachPair([&](PCTime& time, CFlow& flow) {
			if (flow.IsClassDisabled())
				return;

			// Accumulate time in each flow											
			flow.mTimeFromInit.Update() += dt;

			// Check if it is time to execute a periodic flow					
			if (flow.mTimeFromInit.Current() >= time) {
				// Rewind the flow so that it executes again						
				const auto remainder = flow.mTimeFromInit.Current() - time;
				flow.Reset();

				// ... and execute														
				flow.Update(context, remainder);
			}
		});

		// Execute flows that occur after a given point in time				
		mTimeStack.ForEachPair([&](PCTime& time, CFlow& flow) {
			if (time > mTimeFromInit.Current())
				return false;
			if (flow.IsClassDisabled())
				return true;

			// Update the time flow														
			// It might have periodic flows inside									
			flow.Update(context, dt);
			return true;
		});
	}

	/// Execute the whole flow in a specific context									
	///	@param context - context to execute in											
	///	@param timeOffset - time offset to set before execution					
	///	@param timePeriod - the period to execute										
	void CFlow::Execute(Block& context, const PCTime timeOffset, const PCTime timePeriod) {
		mTimeFromInit = timeOffset;
		Update(context, timePeriod);
	}

	/// Merge a flow																				
	///	@param other - the flow to merge with this one								
	void CFlow::Merge(const CFlow& other) {
		mPriorityStack.SmartPush(other.mPriorityStack);
		other.mTimeStack.ForEachPair([&](const PCTime& time, const CFlow& flow) {
			auto found = mTimeStack.FindKey(time);
			if (found.IsSpecial()) {
				// Create a new flow for that time								
				auto newFlow = Ptr<CFlow>::New(this);
				mTimeStack.Add(time, newFlow.Get());
				found = mTimeStack.GetCount() - 1;
			}

			mTimeStack.GetValue(pcptr(found.mIndex))->Merge(flow);
		});
	}

	/// Find past points inside a scope														
	///	@param scope - the scope to analyze												
	///	@return the collection of past points											
	Pasts FindPastPoints(Any& scope) {
		Pasts result;
		const auto pushToPasts = [&]() {
			// Check if scope is a missing future									
			if (!scope.IsRight() && scope.IsMissing()) {
				auto newPoint = Ptr<MissingPoint>::New(real(0), &scope);
				result << newPoint.Get();
			}
		};

		if (scope.IsEmpty()) {
			pushToPasts();
			return result;
		}

		if (scope.IsDeep()) {
			// Scope is deep																
			// There is no escape from this scope once entered					
			scope.ForEach([&](Any& subscope) {
				auto pastPoints = FindPastPoints(subscope);
				if (!pastPoints.IsEmpty())
					result.InsertBlock(pcMove(pastPoints));
			});

			pushToPasts();
			return result;
		}

		// If this is reached, then scope is flat									
		Pasts localResult;
		EitherDoThis
			scope.ForEach([&](Verb& verb) {
				// Nest in each verb output											
				localResult = FindPastPoints(verb.GetOutput());
				if (!localResult.IsEmpty())
					result.InsertBlock(pcMove(localResult));

				// Nest in each verb argument											
				localResult = FindPastPoints(verb.GetArgument());
				if (!localResult.IsEmpty())
					result.InsertBlock(pcMove(localResult));

				// Nest in each verb source											
				localResult = FindPastPoints(verb.GetSource());
				if (!localResult.IsEmpty())
					result.InsertBlock(pcMove(localResult));
			})
		OrThis
			scope.ForEach([&](Trait& trait) {
				// Nest inside traits													
				localResult = FindPastPoints(trait);
				if (!localResult.IsEmpty())
					result.InsertBlock(pcMove(localResult));
			})
		OrThis
			scope.ForEach([&](Construct& construct) {
				// Nest inside constructs												
				localResult = FindPastPoints(construct.GetAll());
				if (!localResult.IsEmpty()) {
					result.InsertBlock(pcMove(localResult));

					// Each construct also implicitly seeks charge from past	
					//TODO this is a pretty radical heuristic - only constructs with missing arguments 
					// search for implicit mass, but what if you want to make a number of constructs, that are explicitly defined in the ontology?
					// works for now...
					auto missingNumber = Ptr<Any>::New(ANumber::ID);
					missingNumber->MakeMissing().MakeLeft();
					auto newPoint = Ptr<MissingPoint>::New(real(0), missingNumber, &construct);
					result << newPoint.Get();
				}
			});

		pushToPasts();
		return result;
	}

	#define PC_FIND_FUTURE_VERBOSE(a) 

	/// Find future points inside a scope													
	///	@param scope - the scope to analyze												
	///	@param priority - the priority under which the scope falls				
	///	@param priorityFeedback - [in/out] tracks priority change forwards	
	///	@return the collection of future points, paired with verb priority	
	Futures FindFuturePoints(Block& scope, const real priority, real& priorityFeedback) {
		auto localPriorityFeedback = Charge::MinPriority;
		Futures result;
		const auto pushToFutures = [&]() {
			// Check if scope is a missing future									
			if (!scope.IsLeft() && scope.IsMissing()) {
				PC_FIND_FUTURE_VERBOSE(pcLogInput 
					<< "Pushed future point (priority " 
					<< priority << "): " << scope);
				auto newPoint = Ptr<MissingPoint>::New(priority, pcReinterpret<Any>(&scope));
				result << newPoint.Get();
			}
		};

		if (scope.IsEmpty()) {
			priorityFeedback = priority;
			pushToFutures();
			return result;
		}

		if (scope.IsDeep()) {
			// Scope is deep																
			// There is no escape from this scope once entered					
			if (scope.IsOr() && scope.GetCount() > 1) {
				PC_FIND_FUTURE_VERBOSE(ScopedTab tab; pcLogSpecial 
					<< "Scanning deep fork for future points (<= priority " 
					<< priority << "): " << scope << tab);

				// DEEP OR																	
				// An OR scope represents branches inside the flow				
				// Future points in those are combined under one priority	
				Fork fork;
				scope.ForEach([&](Any& branch) {
					auto branchFuturePoints = 
						FindFuturePoints(branch, priority, localPriorityFeedback);
					if (!branchFuturePoints.IsEmpty())
						fork.mBranches << pcMove(branchFuturePoints);
					else {
						// Even if no future point is available in the		
						// branch we are obliged to provide the branch		
						// itself as a future point								
						auto newPoint = Ptr<MissingPoint>::New(priority, &branch);
						branchFuturePoints << newPoint.Get();
						fork.mBranches << pcMove(branchFuturePoints);
					}
				});

				if (!fork.mBranches.IsEmpty()) {
					fork.mRoot = pcReinterpret<Any>(&scope);
					fork.mIdentity = pcReinterpret<Any>(scope);
					fork.mDedicatedIdentity = false;
					PC_FIND_FUTURE_VERBOSE(pcLogInput 
						<< "Fork with " << fork.mBranches.GetCount() 
						<< " branches inserted at priority " << priority);
					auto newPoint = Ptr<MissingPoint>::New(priority, Ptr<Any>::New(pcMove(fork)));
					result << newPoint.Get();
				}
			}
			else {
				PC_FIND_FUTURE_VERBOSE(ScopedTab tab; pcLogSpecial 
					<< "Scanning scope for future points (<= priority " 
					<< priority << "): " << scope << tab);

				// DEEP AND																	
				// Scan scope in reverse and push futures to result			
				scope.ForEachRev([&](Any& subscope) {
					auto branchFuturePoints = 
						FindFuturePoints(subscope, priority, localPriorityFeedback);
					if (!branchFuturePoints.IsEmpty())
						result.InsertBlock(pcMove(branchFuturePoints));
				});

				if (priority != noPriority && priority > localPriorityFeedback && localPriorityFeedback != Charge::MinPriority) {
					// The end of an AND scope is always considered a future	
					// if priority changed on the boundary							
					PC_FIND_FUTURE_VERBOSE(pcLogInput 
						<< "Pushed priority-boundary future point due to transition from priority " 
						<< priority << " to " << localPriorityFeedback << ": " << scope);
					auto newPoint = Ptr<MissingPoint>::New(priority, pcReinterpret<Any>(&scope));
					result << newPoint.Get();
					priorityFeedback = priority;
				}
			}

			if (localPriorityFeedback > priorityFeedback)
				priorityFeedback = localPriorityFeedback;
			pushToFutures();
			return result;
		}

		// If this is reached, then scope is flat									
		if (scope.IsOr() && scope.GetCount() > 1) {
			ScopedTab tab;
			pcLogSpecial 
				<< "Scanning flat fork for future points (<= priority " 
				<< priority << "): " << scope << tab;

			// FLAT OR																		
			Fork fork;
			for (pcptr i = 0; i < scope.GetCount(); ++i) {
				auto branch = scope.GetElementResolved(i);
				auto branchFuturePoints = FindFuturePoints(
					branch, priority, localPriorityFeedback);
				if (!branchFuturePoints.IsEmpty())
					fork.mBranches << pcMove(branchFuturePoints);
			}

			if (!fork.mBranches.IsEmpty()) {
				fork.mRoot = pcReinterpret<Any>(&scope);
				fork.mDedicatedIdentity = true;
				PC_FIND_FUTURE_VERBOSE(pcLogInput 
					<< "Fork with " << fork.mBranches.GetCount() 
					<< " branches inserted at priority " << priority);
				auto newPoint = Ptr<MissingPoint>::New(priority, Ptr<Any>::New(pcMove(fork)));
				result << newPoint.Get();
			}
		}
		else {
			// FLAT AND																		
			EitherDoThis
				scope.ForEachRev([&](Verb& verb) {
					const auto nextp = verb.GetPriority();
					localPriorityFeedback = pcMax(nextp, localPriorityFeedback);
					Futures localResult;

					// Nest in each verb output										
					real unusedFeedback = priority;
					localResult = FindFuturePoints(
						verb.GetOutput(), nextp, unusedFeedback);
					if (!localResult.IsEmpty())
						result.InsertBlock(pcMove(localResult));

					// Nest in each verb argument										
					localResult = FindFuturePoints(
						verb.GetArgument(), nextp, unusedFeedback);
					if (!localResult.IsEmpty())
						result.InsertBlock(pcMove(localResult));

					// Nest in each verb source										
					localResult = FindFuturePoints(
						verb.GetSource(), nextp, unusedFeedback);
					if (!localResult.IsEmpty())
						result.InsertBlock(pcMove(localResult));
				})
			OrThis
				scope.ForEachRev([&](Trait& trait) {
					// Nest inside traits												
					auto localResult = FindFuturePoints(
						trait, priority, localPriorityFeedback);
					if (!localResult.IsEmpty())
						result.InsertBlock(pcMove(localResult));
				})
			OrThis
				scope.ForEachRev([&](Construct& construct) {
					// Nest inside constructs											
					auto localResult = FindFuturePoints(
						construct.GetAll(), priority, localPriorityFeedback);
					if (!localResult.IsEmpty())
						result.InsertBlock(pcMove(localResult));
				});
		}

		if (localPriorityFeedback > priorityFeedback)
			priorityFeedback = localPriorityFeedback;

		pushToFutures();
		return result;
	}

	/// Push a scope of elements to the flow, branching if required, removing	
	/// old futures if consumed by past requests											
	///	@param futures - the future points, where scopes can be inserted		
	///	@param scope - the scope to push													
	///	@param branchOut - whether or not to push to a branch						
	///	@return true if at least one element been pushed successfully			
	template<bool ATTEMPT, bool CLONE>
	bool InnerPush(Futures& futures, const Block& content, bool branchOut) {
		if (content.IsEmpty())
			return false;

		SuccessTrap done;
		if (content.IsDeep()) {
			// Content is deep															
			if (content.IsOr() && content.GetCount() > 1) {
				// Content is deep OR													
				// First do a dry-run to check how many branches we'll need	
				TAny<pcptr> anticipatedBranches;
				pcptr counter = 0;
				content.ForEach([&](const Block& subscope) {
					if (InnerPush<true, true>(futures, subscope, branchOut))
						anticipatedBranches << counter;
					++counter;
				});

				if (anticipatedBranches.IsEmpty())
					return false;

				if (anticipatedBranches.GetCount() == 1) {
					// Only one success is anticipated, so we do not need		
					// any branching - push everything conventionally to		
					// the future point. This saves a LOT of overhead.			
					InnerPush<ATTEMPT, CLONE>(futures, 
						content.Get<Block>(anticipatedBranches[0]), branchOut);
				}
				else for (auto& index : anticipatedBranches) {
					// Make future point a fork, if not a fork yet,				
					// pushing each subpack to a separate branch					
					auto& branch = content.Get<Block>(anticipatedBranches[index]);
					PC_CFLOW_VERBOSE_TAB(ccYellow << "INSERTING BRANCH: " << branch);
					InnerPush<ATTEMPT, true>(futures, branch, true);
				}

				done = true;
			}
			else {
				// Content is deep AND													
				// It is safe to push each subscope individually				
				content.ForEach([&](const Block& subscope) {
					done = InnerPush<ATTEMPT, CLONE>(futures, subscope, branchOut);
				});
			}

			return done;
		}

		// If this is reached, then scope is flat									
		if (content.IsOr() && content.GetCount() > 1) {
			// Scope is flat OR															
			// First do a dry-run to check how many branches we'll need		
			TAny<pcptr> anticipatedBranches;
			for (pcptr i = 0; i < content.GetCount(); ++i) {
				if (InnerPush<true, true>(futures, content.GetElementResolved(i), branchOut))
					anticipatedBranches << i;
			};

			if (anticipatedBranches.IsEmpty())
				return false;

			if (anticipatedBranches.GetCount() == 1) {
				// Only one success is anticipated, so we do not need			
				// any branching - push everything conventionally to the		
				// future point. This saves a LOT of overhead.					
				InnerPush<ATTEMPT, CLONE>(futures, 
					content.GetElement(anticipatedBranches[0]), branchOut);
			}
			else for (auto& index : anticipatedBranches) {
				// Make future point a fork, if not a fork yet,					
				// pushing each subpack to a separate branch						
				auto branch = content.GetElement(anticipatedBranches[index]);
				PC_CFLOW_VERBOSE_TAB(ccYellow << "INSERTING BRANCH: " << branch);
				InnerPush<ATTEMPT, true>(futures, branch, true);
			}

			return true;
		}
		
		// Scope is flat AND																
		// It is safe to just push each verb individually						
		for (pcptr i = 0; i < content.GetCount(); ++i) {
			auto element = content.GetElementResolved(i);
			for (auto future : futures) {
				if (element.Is<Verb>()) {
					// Skip future points that are below a verb's priority	
					if (future->mPriority < element.Get<Verb>().GetPriority())
						continue;
				}

				if (!branchOut || ATTEMPT) {
					if (future->IsFork()) {
						// Multicast in parallel futures...							
						for (auto& branch : future->mPack->Get<Fork>().mBranches) {
							done = InnerPush<ATTEMPT, true>(branch, element, false);
						}
					}
					else {
						// Push the element												
						Any wrappedElement { element };
						done = future->FilterAndInsert<ATTEMPT, CLONE>(wrappedElement, FindPastPoints);
					}
				}
				else {
					// Push element in a new branch...								
					auto& branch = future->AddBranch();
					done = InnerPush<ATTEMPT, true>(branch, element, false);
				}

				// Break on success, take care of the next element				
				if (done)
					break;
			}
		}

		return done;
	}

	/// Push a scope of verbs and data to the flow										
	///	@param scope - the scope to analyze and push									
	///	@return true if anything was pushed to the flow								
	bool CFlow::Push(const Any& scope) {
		if (scope.IsEmpty())
			return true;

		PC_CFLOW_VERBOSE_TAB(this << ": Pushing: " << scope);

		// Collect all future points inside the priority stack				
		// those points might or might not have filters;						
		// all the points are paired with a priority if contained in		
		// verbs																				
		real smallestPriority = noPriority;
		auto futures = FindFuturePoints(mPriorityStack, noPriority, smallestPriority);

		// Always push entire priority stack as the last option				
		auto newPoint = Ptr<MissingPoint>::New(noPriority, &mPriorityStack);
		futures << newPoint.Get();

		#if LANGULUS_DEBUG()
			PC_CFLOW_VERBOSE(ccPurple << "=================");
			for (const auto future : futures)
				future->Dump();
		#endif

		return InnerPush<false, false>(futures, scope, false);
	}


	/// Input package to an explicit time/period inside this flow					
	///	@param moment - time point															
	///	@param period - period point														
	///	@param verb - the verb to push													
	///	@return true if anything was pushed to the flow								
	/*bool CFlow::PushVerb(real moment, real period, const Verb& verb) {
		if (moment < 0) {
			pcLogSelfError << "Trying to push at negative time: " << verb;
			return false;
		}

		if (period < 0) {
			pcLogSelfError << "Trying to push at period time: " << verb;
			return false;
		}

		const auto momentInt = static_cast<pcu64>(moment);
		const auto periodInt = static_cast<pcu64>(period);

		// Pick the correct flow														
		CFlow* flow = this;
		if (momentInt != 0) {
			// The moment is different from now, so we push the package to	
			// a specific temporal stack flow										
			const auto time_index = mTimeStack.FindKey(momentInt);
			if (time_index == uiNone) {
				// A new flow needs to be added on the temporal stack			
				mTimeStack.Add(momentInt, CFlow(this));
				mTimeStack.Sort(uiSmallest);
				flow = &mTimeStack[momentInt];
				PC_CFLOW_VERBOSE(ccPurple << "Created time group for @" << momentInt << ": " << flow);

				// Save the new duration												
				if (momentInt > mDuration)
					mDuration = momentInt;
			}
			else {
				flow = &mTimeStack[momentInt];
				PC_CFLOW_VERBOSE(ccPurple << "Reused time group for @" << momentInt << ": " << flow);
			}
		}

		if (periodInt != 0) {
			// The period is different from once, so we push the package	
			// to a specific frequency stack flow									
			const auto period_index = flow->mFrequencyStack.FindKey(periodInt);
			if (period_index == uiNone) {
				// A new flow needs to be added on the frequency stack		
				flow->mFrequencyStack.Add(periodInt, CFlow(this));
				flow = &flow->mFrequencyStack[periodInt];
				PC_CFLOW_VERBOSE(ccPurple << "Created frequency group for ^" << periodInt << ": " << flow);
			}
			else {
				flow = &mFrequencyStack[static_cast<PCTime>(periodInt)];
				PC_CFLOW_VERBOSE(ccPurple << "Reused frequency group for ^" << periodInt << ": " << flow);
			}
		}

		PC_CFLOW_VERBOSE_TAB("Pushing verb: " << verb);

		// Pick scope, integrate and push											
		Any wrappedVerb = verb;
		if (flow->Multicast(wrappedVerb)) {
			PC_CFLOW_VERBOSE(ccCyan << "Current flow after verb push: " << flow->GetPriorityStack());
			return true;
		}

		return false;
	}*/

} // namespace PCFW
