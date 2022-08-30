#include "MissingFlow.hpp"

#define VERBOSE_MISSING_POINT(a) //pcLogVerbose << a
#define VERBOSE_MISSING_POINT_TAB(a) //ScopedTab tab; pcLogVerbose << a << tab

namespace PCFW
{
   
	/// Check if the future point branches out											
	///	@return true if the future point is a branch									
	bool MissingPoint::IsFork() const noexcept {
		return mPack->Is<Fork>();
	}

	/// Get the filter part of the point													
	///	@return a reference to the filter part											
	const Filter& MissingPoint::GetFilter() const SAFE_NOEXCEPT() {
		SAFETY(if(IsFork()) throw Except::BadFlow(
			"Point is a fork, and you must handle it explicitly"));

		if (mPack->IsMissing()) {
			const auto& filter = mPack->IsDeep() ? mPack->Get<Any>(0) : *mPack;
			SAFETY(if(!filter.IsEmpty() && !filter.Is<DataID>()) 
				throw Except::BadFlow(pcLogError << "Bad filter format"));
			return pcReinterpret<Filter>(filter);
		}

		static const Filter fallback{};
		return fallback;
	}

	/// Get the filter part of the point													
	///	@param content - the content to push											
	///	@return a reference to the filter part											
	NOD() inline bool MissingPoint::Accepts(const Block& content) const SAFE_NOEXCEPT() {
		if (GetFilter().IsEmpty())
			return true;

		for (auto& type : GetFilter()) {
			if (content.InterpretsAs(type.GetMeta()))
				return true;
		}

		return false;
	}

	/// Get the content part of the point													
	///	@return a reference to the filter part											
	NOD() inline const Block& MissingPoint::GetContent() const SAFE_NOEXCEPT() {
		SAFETY(if(IsFork()) throw Except::BadFlow(pcLogError <<
			"Point is a fork, and you must handle it explicitly"));

		static const Block fallback{};
		if (mPack->IsMissing())
			return mPack->IsDeep() ? mPack->Get<Any>(1) : fallback;
		return *mPack;
	}

	/// Get the content part of the point													
	///	@return a reference to the filter part											
	NOD() inline bool MissingPoint::HasRelevantContent() const SAFE_NOEXCEPT() {
		auto& content = GetContent();
		if (content.IsEmpty() || GetFilter().IsEmpty())
			return false;

		bool satisfied = false;
		content.ForEachDeep([&](const Block& b) {
			if (Accepts(b))
				satisfied = true;
			return !satisfied;
		});

		return satisfied;
	}

	/// Push content to the point - always clones										
	///	@param content - the content to push											
	void MissingPoint::AddContent(Any& content) {
		SAFETY(if(IsFork()) throw Except::BadFlow(pcLogError <<
			"Point is a fork, and you must handle it explicitly"));

		// This is reached only if !ATTEMPT											
		if (mPriority == noPriority) {
			VERBOSE_MISSING_POINT(ccPurple
				<< "Point changed (no priority):");
		}
		else {
			VERBOSE_MISSING_POINT(ccPurple
				<< "Point changed (priority " << mPriority << "):");
		}

		VERBOSE_MISSING_POINT(ccPurple << " - was " << mPack);

		if (mPack->IsMissing()) {
			// Deepen the point if not deep yet, so that we keep filter		
			if (!mPack->IsDeep())
				mPack->Deepen<Any>(false);

			if (mPack->GetCount() == 1)
				*mPack << Any{};

			SAFETY(if (mPack->GetCount() != 2) TODO());
			mPack->Get<Any>(1) << pcMove(content);
		}
		else {
			// Doesn't have a filter, so just push									
			*mPack << pcMove(content);
		}

		VERBOSE_MISSING_POINT(ccPurple << " - now " << mPack);
	}

	/// Collapse the point, clearing contents, polarity, filters					
	void MissingPoint::Collapse() {
		SAFETY(if(IsFork()) throw Except::BadFlow(pcLogError <<
			"Point is a fork, and you must handle it explicitly"));

		if (mPack->IsMissing()) {
			if (mPack->IsRight()) {
				// Remove only content if dedicated future						
				if (mPack->IsDeep() && mPack->GetCount() == 2)
					mPack->RemoveIndex(1);
			}
			else if (mPack->IsLeft()) {
				// Remove everything if dedicated past								
				mPack->Reset();
			}
			else {
				// Remove only content if unspecified polarity					
				if (mPack->IsDeep() && mPack->GetCount() == 2)
					mPack->RemoveIndex(1);
			}
		}
		else mPack->Reset();
	}

	/// Add a branch, making this future point a fork									
	///	@return the new branch																
	Futures& MissingPoint::AddBranch() {
		{
			VERBOSE_MISSING_POINT_TAB(ccYellow << "CREATING BRANCH IN: ");
			Dump();
		}

		if (!IsFork()) {
			// Turn a normal point to a fork											
			Fork fork;
			fork.mRoot = mPack;
			fork.mIdentity = pcMove(*mPack);
			fork.mDedicatedIdentity = true;
			mPack->Reset();
			mPack->MakeOr();
			mPack = Ptr<Any>::New(pcMove(fork));
		}

		// At this point, this future point is a fork							
		// Clone the identity, pushing it to the root							
		Fork& fork = mPack->Get<Fork>();
		if (!fork.mDedicatedIdentity) {
			// Make sure that we have a dedicated identity before				
			// making any changes														
			fork.mIdentity = fork.mIdentity.Clone();
			fork.mDedicatedIdentity = true;
		}

		*fork.mRoot << fork.mIdentity.Clone();

		// Push the new branch, interfacing that cloned identity				
		Futures branch;
		auto newPoint = Ptr<MissingPoint>::New(mPriority, fork.mRoot->As<Any*>(uiLast));
		branch << newPoint.Get();
		fork.mBranches << pcMove(branch);

		{
			VERBOSE_MISSING_POINT_TAB(ccYellow << "RESULTING BRANCHES: ");
			Dump();
		}

		return mPack->Get<Fork>().mBranches.Last();
	}

	/// Dump the contents of the point in the log										
	void MissingPoint::Dump() const {
		if (IsFork()) {
			for (auto& branch : mPack->Get<Fork>().mBranches) {
				VERBOSE_MISSING_POINT_TAB(ccYellow << "BRANCH: ");
				for (auto p : branch)
					p->Dump();
			}
		}
		else {
			if (mPriority == noPriority) {
				VERBOSE_MISSING_POINT(ccPurple 
					<< "FUTURE (no priority): ");
			}
			else {
				VERBOSE_MISSING_POINT(ccPurple 
					<< "FUTURE (priority " << mPriority << "): ");
			}

			pcLog << *mPack;
		}
	}

} // namespace PCFW