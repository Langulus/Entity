#pragma once
#include "Unit.hpp"

namespace Langulus::Entity
{

	/// Execute verb in all owners															
	///	@param verb - the verb to execute in all owners								
	///	@param seek - where in the hierarchy to execute								
	///	@return true if succesfully executed											
	template<SeekStyle SEEK>
	bool Unit::DoInHierarchy(Verb& verb) {
		if (mOwners.IsEmpty()) {
			Logger::Warning() << "No owners available for executing " << verb;
			return false;
		}

		bool success {};
		for (auto owner : mOwners)
			success |= owner->DoInHierarchy<SEEK>(verb);
		return success;
	}

	/// Get a value from inside a trait by scanning owners and other units		
	///	@param value - [out] value to set if trait was found						
	///	@return true if anything was written to value								
	template<CT::Trait T, CT::Data D>
	bool Unit::SeekValue(D& value) const {
		return SeekValue<D>(MetaTrait::Of<T>(), value);
	}

} // namespace Langulus::Entity