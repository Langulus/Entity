///																									
/// Langulus::Entity																				
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>							
///																									
/// Distributed under GNU General Public License v3+									
/// See LICENSE file, or https://www.gnu.org/licenses									
///																									
#pragma once
#include "Resolvable.hpp"

namespace Langulus::Entity
{

	/// Constructor																				
	///	@attention type is assumed derived from Resolvable 						
	///	@param type - type of the resolvable											
	inline Resolvable::Resolvable(DMeta type) noexcept
		: mClassType {type}
		, mClassOffset {0} {
		// Precalculate offset															
		RTTI::Base base;
		UNUSED() bool found = mClassType->GetBase<Resolvable>(0, base);
		SAFETY(if (!found)
			Throw<Except::Construct>("Unrelated type provided to Resolvable"));
		const_cast<Offset&>(mClassOffset) = base.mOffset;
	}

	/// Get the class meta																		
	///	@return the meta data for the class type										
	constexpr DMeta Resolvable::GetType() const noexcept {
		return mClassType;
	}

	/// Get the class name token																
	///	@return the token																		
	inline Token Resolvable::GetToken() const noexcept {
		return mClassType->mToken;
	}

	/// Check if context interprets as a type												
	///	@param type - the type to check for												
	///	@return true if this context can be dynamically interpreted to type	
	inline bool Resolvable::CastsTo(DMeta type) const noexcept {
		return mClassType->CastsTo(type);
	}

	/// Check if context interprets as a static type									
	///	@tparam T - the type to check for												
	///	@return true if this context can be dynamically interpreted as T		
	template<CT::Data T>
	bool Resolvable::CastsTo() const {
		return mClassType->CastsTo<T>();
	}

	/// Check if context is an exact type													
	///	@param type - the type to check for												
	///	@return true if this context can be dynamically interpreted to type	
	inline bool Resolvable::Is(DMeta type) const noexcept {
		return mClassType->Is(type);
	}

	/// Check if context is an exact static type											
	///	@tparam T - the type to check for												
	///	@return true if this context can be dynamically interpreted as T		
	template<CT::Data T>
	inline bool Resolvable::Is() const {
		return mClassType->Is<T>();
	}

	/// Stringify the context (shows class type and an identifier)					
	inline Resolvable::operator Debug() const {
		Flow::Code result;
		result += GetToken();
		result += Flow::Code::OpenScope;
		#if !LANGULUS(PARANOID) && LANGULUS(DEBUG)
			// We're not paranoid, so directly dump the memory address		
			result += Text {fmt::format("{:X}", reinterpret_cast<intptr_t>(this))};
		#else
			// Obfuscate the pointer, by hashing it								
			result += Text {fmt::format("{:X}", HashNumber(reinterpret_cast<intptr_t>(this)))};
		#endif
		result += Flow::Code::CloseScope;
		return result;
	}

	/// Wrap this context instance in a static memory block							
	/// The availability of this function is reflected via CT::Resolvable		
	/// You can invoke this function via Block::GetElementResolved()				
	///	@return the static memory block representing this instance				
	inline Block Resolvable::GetBlock() const noexcept {
		// 'this' pointer points to Resolvable object, so we need to		
		// compensate this, by offsetting 'this' by the relative class		
		// type offset. I like to live dangerously <3							
		auto thisint = reinterpret_cast<Offset>(this);
		auto offsetd = reinterpret_cast<void*>(thisint - mClassOffset);
		return Block {DataState::Static, mClassType, 1, offsetd};
	}

	/// Invoke a verb on the resolved type													
	///	@tparam DISPATCH - whether to allow custom dispatchers					
	///	@tparam DEFAULT - whether to allow default/stateless verbs on fail	
	///	@tparam V - type of verb to run (deducible)									
	///	@param verb - the verb to execute in this resolved type					
	///	@return true if verb was satisfied												
	template<bool DISPATCH, bool DEFAULT, CT::Verb V>
	bool Resolvable::Run(V& verb) {
		auto environment = GetBlock();
		return Flow::DispatchFlat<false, DISPATCH, DEFAULT>(environment, verb);
	}

} // namespace Langulus::Entity