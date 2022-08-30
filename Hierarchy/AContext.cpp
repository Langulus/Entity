#include "AContext.hpp"

namespace PCFW
{

	/// Stringify the context (shows class type and a hexed pointer)				
	AContext::operator Debug() const {
		GASM result;
		result += ClassToken();
		result += GASM::OpenScope;
		#if LANGULUS_DEBUG() && !LANGULUS_PARANOID()
			result += pcToHex(this);
		#else
			result += "...";
		#endif
		result += GASM::CloseScope;
		return result;
	}

	/// Convert to GASM																			
	AContext::operator GASM() const {
		GASM result;
		result += ClassToken();
		result += GASM::OpenScope;
		result += "<TODO serialize descriptor>";
		result += GASM::CloseScope;
		return result;
	}

	/// Wrap this context instance in a static memory block							
	/// The availability of this function is reflected via pcIsResolvable		
	/// You can invoke this function via BlocK::GetElementResolved()				
	///	@return the static memory block representing this context instance	
	Block AContext::GetBlock() const SAFE_NOEXCEPT() {
	#if LANGULUS_SAFE()
		LinkedBase base;
		if (mClass->GetBase<AContext>(0, base)) {
			auto offsetThis = pcN2P(pcP2N(this) - base.mStaticBase.mLocalOffset);
			return Block{ DState::Static, mClass, 1, const_cast<void*>(offsetThis) };
		}
		else TODO();
	#else
		LinkedBase base;
		mClass->GetBase<AContext>(0, base);
		auto offsetThis = pcN2P(pcP2N(this) - base.mStaticBase.mLocalOffset);
		return Block{ DState::Static, mClass, 1, const_cast<void*>(offsetThis) };
	#endif
	}

} // namespace PCFW
