#pragma once
#include "AUnit.hpp"

namespace Langulus::Entity::A
{

	/// Get a value from inside a trait by scanning owners and other units		
	///	@param value - [out] value to set if trait was found						
	///	@return true if anything was written to value								
	template<CT::Trait T, CT::Data D>
	bool Unit::SeekValue(D& value) const {
		return SeekValue<D>(MetaTrait::Of<T>(), value);
	}

} // namespace Langulus::Entity::A