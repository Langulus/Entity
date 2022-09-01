#pragma once
#include "Common.hpp"

namespace Langulus::Entity
{

	///																								
	///	Abstract context																		
	///																								
	/// Holds a reflected class type and context state									
	///																								
	struct Resolvable {
		LANGULUS(ABSTRACT) true;
	protected:
		DMeta mClassType;
		Offset mClassOffset;

	public:
		Resolvable() = delete;
		Resolvable(const Resolvable&) noexcept = default;
		Resolvable(Resolvable&&) noexcept = default;
		Resolvable(DMeta) noexcept;

		Resolvable& operator = (const Resolvable&) noexcept = default;
		Resolvable& operator = (Resolvable&&) noexcept = default;

		NOD() bool CastsTo(DMeta) const noexcept;
		template<CT::Data T>
		NOD() bool CastsTo() const;

		NOD() bool Is(DMeta) const noexcept;
		template<CT::Data T>
		NOD() bool Is() const;

		NOD() Token GetToken() const noexcept;
		NOD() constexpr DMeta GetType() const noexcept;
		NOD() Block GetBlock() const noexcept;
		template<bool DISPATCH = true, bool DEFAULT = true, CT::Verb V>
		NOD() bool Run(V& verb);

		template<CT::Index INDEX>
		NOD() Block GetMember(TMeta, const INDEX&) noexcept;
		template<CT::Index INDEX>
		NOD() Block GetMember(TMeta, const INDEX&) const noexcept;

		NOD() explicit operator Debug() const;
	};
	
} // namespace Langulus::Entity

#include "Resolvable.inl"