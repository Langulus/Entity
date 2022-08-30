#pragma once
#include "ContextState.hpp"

namespace PCFW
{

	///																								
	///	ABSTRACT CONTEXT																		
	///																								
	/// Holds a reflected class type, context state, and references				
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AContext {
		// Type of context																
		DMeta mClass = nullptr;
		// Context state																	
		CState mClassState;

	public:
		REFLECT(AContext);
		AContext() = delete;
		constexpr AContext(const AContext&) noexcept = default;
		constexpr AContext(AContext&&) noexcept = default;
		constexpr AContext(DMeta, const CState& = CState::Default) noexcept;

		constexpr ME& operator = (const ME&) noexcept = default;
		constexpr ME& operator = (ME&&) noexcept = default;

		NOD() explicit operator GASM() const;
		NOD() explicit operator Debug() const;

		NOD() const Text ClassToken() const;
		NOD() CState ClassState() const noexcept;
		NOD() CState& ClassState() noexcept;

		NOD() bool ClassInterpretsAs(DMeta) const;
		template<class T>
		NOD() bool ClassInterpretsAs() const;

		NOD() bool ClassIs(DataID) const;
		template<class T>
		NOD() bool ClassIs() const;

		NOD() bool IsClassIrrelevant() const noexcept;
		NOD() bool IsClassDisabled() const noexcept;
		NOD() bool IsClassEnabled() const noexcept;
		NOD() bool IsClassInvalid() const noexcept;
		NOD() bool IsClassValid() const noexcept;

		void ClassValidate() noexcept;
		void ClassInvalidate() noexcept;

		void ClassEnable() noexcept;
		void ClassDisable() noexcept;

		NOD() DMeta ClassMeta() const noexcept;
		NOD() Block GetBlock() const SAFE_NOEXCEPT();
	};
	
} // namespace PCFW

#include "AContext.inl"