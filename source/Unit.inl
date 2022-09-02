#pragma once
#include "Unit.hpp"

namespace Langulus::Entity
{

	/// Get the list of unit owners															
	///	return the owners																		
	inline const TAny<Entity*>& Unit::GetOwners() const noexcept {
		return mOwners;
	}

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

	/// Get a trait by scanning owners and other units									
	///	@return the trait																		
	template<CT::Trait T, SeekStyle SEEK>
	Trait Unit::SeekTrait() const {
		Trait output;
		static const auto data = MetaTrait::Of<T>();

		bool satisfied {};
		for (auto context : mOwners) {
			const bool found = context->SeekTrait<SEEK>(data, output);
			if (found && satisfied) {
				Logger::Warning() << "Multiple " << data << " traits found in hierarchy";
				Logger::Warning() << "Each sequential trait will overwrite the result";
			}

			satisfied |= found;
		}

		return output;
	}

	/// Get a value from inside a trait by scanning owners and other units		
	///	@param trait - the trait to seek for											
	///	@param value - [out] value to set if trait was found						
	///	@return true if anything was written to value								
	template<SeekStyle SEEK, CT::Data D>
	bool Unit::SeekValue(TMeta trait, D& value) const {
		bool satisfied {};
		for (auto context : mOwners) {
			bool found = context->SeekValue<SEEK>(trait, value);
			if (found && satisfied) {
				Logger::Warning() << "Multiple " << trait << " traits found in hierarchy";
				Logger::Warning() << "Each sequential trait will overwrite the result";
			}

			satisfied |= found;
		}

		return satisfied;
	}

	/// Get a value from inside a trait by scanning owners and other units		
	///	@param value - [out] value to set if trait was found						
	///	@return true if anything was written to value								
	template<SeekStyle SEEK, CT::Trait T, CT::Data D>
	bool Unit::SeekValue(D& value) const {
		return SeekValue<SEEK, D>(MetaTrait::Of<T>(), value);
	}

} // namespace Langulus::Entity