#pragma once
#include "../IntegrateFramework.hpp"

namespace PCFW
{

	///																								
	///	CONTEXT STATE																			
	///																								
	class LANGULUS_MODULE(FRAMEWORK) ContextState : public TNamed<ContextState, pcu8> {
		REFLECT(ContextState);
	public:
		enum Enum : pcu8 {
			// A default state															
			// It is basically a valid, mutable, enabled and local context	
			Default = 0,

			// Invalid contexts are recyclable, and marked for deletion		
			Invalid = 1,

			// An immutable context never changes									
			Constant = 2,

			// A shared context will be synchronized in a network				
			Shared = 4,

			// A dynamic context is never optimized								
			Dynamic = 8,

			// A disabled context doesn't update									
			Disabled = 16,

			// A context is marked as busy											
			Busy = 32,

			// Will be automatically saved to a file								
			Persistent = 64,
		};

		/// Get named token																		
		///	@param index - the index to get												
		///	@return the token																	
		static constexpr LiteralText GetToken(pcptr index) {
			constexpr LiteralText names[] = {
				"csDefault",
				"csInvalid",
				"csConstant",
				"csShared",
				"csDynamic",
				"csDisabled",
				"csBusy",
				"csPersistent"
			};

			if (index >= sizeof(names) / sizeof(LiteralText))
				throw Except::BadAccess("Named array out of bounds");
			return names[index];
		}

		using TNamed::TNamed;

		constexpr ContextState(ContextState::Enum initial)
			: TNamed(initial) {}
	};

	using CState = ContextState;

} // namespace PCFW

/// Declare constants for GASM and RTTI													
LANGULUS_DECLARE_CONSTANT(DefaultCS, CState(CState::Default), "Default context state");
LANGULUS_DECLARE_CONSTANT(InvalidCS, CState(CState::Invalid), "Invalid context state");
LANGULUS_DECLARE_CONSTANT(ConstantCS, CState(CState::Constant), "Constant context state");
LANGULUS_DECLARE_CONSTANT(SharedCS, CState(CState::Shared), "Shared context state");
LANGULUS_DECLARE_CONSTANT(DynamicCS, CState(CState::Dynamic), "Dynamic context state");
LANGULUS_DECLARE_CONSTANT(DisabledCS, CState(CState::Disabled), "Disabled context state");
LANGULUS_DECLARE_CONSTANT(BusyCS, CState(CState::Busy), "Busy context state");
LANGULUS_DECLARE_CONSTANT(PersistentCS, CState(CState::Persistent), "Persistent context state");
