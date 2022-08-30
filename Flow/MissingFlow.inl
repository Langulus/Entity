#define VERBOSE_MISSING_POINT(a) pcLogVerbose << a
#define VERBOSE_MISSING_POINT_TAB(a) ScopedTab tab; pcLogVerbose << a << tab

namespace PCFW
{
   
	/// Filter and push content to this point												
	///	@param content - the content to filter and insert							
	///	@return true if anything was pushed												
	template<bool ATTEMPT, bool CLONE>
	bool MissingPoint::FilterAndInsert(Any& content, const TFunctor<Pasts(Any&)>& FindPastPoints) {
		if (content.IsDeep()) {
			// Don't allow deep content to be reset at once!					
			// Nest to prevent this														
			SuccessTrap atLeastOneSuccess;
			if (content.IsOr()) {
				content.ForEach([&](Any& subcontent) {
					atLeastOneSuccess = FilterAndInsert<ATTEMPT, true>(subcontent, FindPastPoints);
				});

				if constexpr (!ATTEMPT)
					content.Reset();
			}
			else {
				content.ForEach([&](Any& subcontent) {
					atLeastOneSuccess = FilterAndInsert<ATTEMPT, CLONE>(subcontent, FindPastPoints);
					if constexpr (!ATTEMPT && !CLONE) {
						if (subcontent.IsEmpty())
							content.Remove(&subcontent);
					}
				});
			}

			return atLeastOneSuccess;
		}

		if (mChargeFor) {
			if constexpr (!ATTEMPT) {
				// Special case when integrating implicit charges				
				// Always succeeds, because it is not mandatory					
				real mass = mChanges ? mChargeFor->mCharge.mMass : 0;
				auto interpreter =
					Verb::From<Verbs::Interpret>({}, DataID::Of<real>).ShortCircuit(false);
				if (Verb::DispatchDeep(content, interpreter)) {
					interpreter.GetOutput().ForEachDeep(
						[&mass](const real& n) noexcept {
							mass = pcConcat(mass, n);
						}
					);

					VERBOSE_MISSING_POINT(ccPurple 
						<< "Charge changed (via " << content << "): ");
					VERBOSE_MISSING_POINT(ccPurple 
						<< " - was " << *mChargeFor);
					mChargeFor->mCharge.mMass = mass;
					++mChanges;
					VERBOSE_MISSING_POINT(ccPurple 
						<< " - now " << *mChargeFor);
					content.Reset();
				}
			}

			// Charge always succeeds													
			return true;
		}

		if (GetFilter().IsEmpty()) {
			// No filter, just push														
			if (!content.Is<Verb>()) {
				// Try interpreting scope as verbs and try pushing them		
				// If that fails just push the scope itself						
				auto interpreter =
					Verb::From<Verbs::Interpret>({}, DataID::Of<Verb>).ShortCircuit(false);
				if (Verb::DispatchDeep(content, interpreter)) {
					Any inserted;
					if (Insert<ATTEMPT, CLONE>(*this, interpreter.GetOutput(), inserted, FindPastPoints)) {
						if constexpr (!ATTEMPT) {
							AddContent(inserted);
							content.Reset();
						}
						return true;
					}
				}
			}
				
			// Scope doesn't seem to be made of verbs, so just push			
			Any inserted;
			if (Insert<ATTEMPT, CLONE>(*this, content, inserted, FindPastPoints)) {
				if constexpr (!ATTEMPT) {
					AddContent(inserted);
					content.Reset();
				}
				return true;
			}

			return false;
		}

		// If filters are available, interpret source as requested			
		// Always push a verb filter if there isn't one yet					
		auto& filters = GetFilter();
		VERBOSE_MISSING_POINT_TAB("Satisfying filter " << filters 
			<< " by interpreting " << content);

		auto interpreter = Verb::From<Verbs::Interpret>({}, filters).ShortCircuit(false);
		if (Verb::DispatchDeep(content, interpreter)) {
			// If results in verb skip insertion, delay for unfiltered		
			bool skip = false;
			interpreter.GetOutput().ForEachDeep([&skip](const Verb&) {
				skip = true;
				return false;
			});

			if (skip)
				return false;

			Any inserted;
			if (Insert<ATTEMPT, CLONE>(*this, interpreter.GetOutput(), inserted, FindPastPoints)) {
				if constexpr (!ATTEMPT) {
					AddContent(inserted);
					content.Reset();
				}
				return true;
			}
		}

		return false;
	}
		
	/// Helper for inserting content to a flow point									
	///	@param context - [in/out] the context we use for integration			
	///						- May be consumed													
	///	@param content - [in/out] the content to insert								
	///						- May have past-points integrated within context		
	///	@param output - [out] the resulting output									
	///	@return true if anything was pushed												
	template<bool ATTEMPT, bool CLONE>
	bool MissingPoint::Insert(MissingPoint& context, Any& content, Any& output, const TFunctor<Pasts(Any&)>& FindPastPoints) {
		if (content.IsDeep()) {
			if (!content.IsOr()) {
				// Nest AND																	
				// Subsequent insertions are allowed to consume contexts		
				FailureTrap failure;
				content.ForEach([&](Any& b) {
					failure = Insert<ATTEMPT, CLONE>(context, b, output, FindPastPoints);
					return !failure;
				});

				return !failure;
			}
			else {
				// Nest OR																	
				// Subsequent insertions are NOT allowed to consume			
				// contexts - the whole context is reset after all				
				// branches have been processed										
				SuccessTrap success;
				auto localOutput = Any::FromStateOf(content);
				content.ForEach([&](Any& b) {
					success = Insert<ATTEMPT, true>(context, b, localOutput, FindPastPoints);
				});

				if constexpr (!ATTEMPT) {
					if (success && !localOutput.IsEmpty()) {
						context.Collapse();
						if (localOutput.GetCount() == 1)
							output.InsertBlock(localOutput);
						else
							output << pcMove(localOutput);
					}
				}

				return success;
			}
		}

		// If reached, then content is flat											
		// We must either clone or move the original content					
		Any localContent;
		if constexpr (!ATTEMPT) {
			if constexpr (CLONE)
				localContent = content.Clone();
			else
				localContent = pcMove(content);
		}
		else localContent = content;

		// First we have to integrate the insertion, by filling any			
		// past points it has, with the available context						
		auto pasts = FindPastPoints(localContent);
		Ptr<Any> pastContent;
		if (!pasts.IsEmpty()) {
			// Get the relevant context we'll be integrating past with		
			if (context.mPack->IsMissing()) {
				if (context.mPack->IsDeep() && context.mPack->GetCount() > 1)
					pastContent = context.mPack->Get<Any*>(1);
			}
			else pastContent = context.mPack;

			if (!pastContent || pastContent->IsEmpty()) {
				// If no content available for past - fail						
				// Unless there's already relevant content						
				if (context.HasRelevantContent())
					return true;

				if constexpr (!ATTEMPT) {
					VERBOSE_MISSING_POINT(ccRed
						<< "Can't past-integrate with empty past: " << localContent);
				}

				return false;
			}
		}

		for (auto past : pasts) {
			// Push past to the point													
			// This is the point at which context might be consumed			
			if (past->FilterAndInsert<ATTEMPT, false>(*pastContent, FindPastPoints))
				continue;
				
			// Nothing relevant found in past, so we skip inserting			
			// Unless there's already relevant content							
			if (past->mChargeFor || context.HasRelevantContent())
				continue;

			// This is optimization that minimizes branching a lot			
			if constexpr (!ATTEMPT) {
				VERBOSE_MISSING_POINT(ccRed
					<< "Can't past-integrate: " << localContent);
				VERBOSE_MISSING_POINT(ccRed
					<< "With: " << pastContent);
				VERBOSE_MISSING_POINT(ccRed
					<< "Requires: " << past->GetFilter());
			}

			return false;
		}

		if constexpr (!ATTEMPT)
			output << pcMove(localContent);
		return true;
	}

} // namespace PCFW

#undef VERBOSE_MISSING_POINT
#undef VERBOSE_MISSING_POINT_TAB
