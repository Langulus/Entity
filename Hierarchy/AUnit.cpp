#include "AUnit.hpp"
#include "Entity.hpp"

namespace PCFW
{

	/// Move unit																					
	///	@param other - the unit to move													
	AUnit::AUnit(AUnit&& other) noexcept
		: AContext {pcForward<AContext>(other)}
		, mOwners {pcMove(other.mOwners)} {
		// Replace the owner's unit pointer with the new one					
		for (auto owner : mOwners)
			owner->ReplaceUnit(&other, this);
		other.ClassInvalidate();
	}

	/// Manual construction																		
	///	@param classid - type of the unit												
	AUnit::AUnit(DMeta classid) noexcept
		: AContext {classid} {
		ClassInvalidate();
	}

	/// AUnit destruction																		
	AUnit::~AUnit() {
		for (auto owner : mOwners)
			Decouple(owner);
	}

	/// Default unit selection simply relays to the owner								
	///	@param verb - the selection verb													
	void AUnit::Select(Verb& verb) {
		for (auto owner : mOwners)
			owner->Select(verb);
	}

	/// Execute verb in all owners															
	///	@param doWhat - the verb to execute in all owners							
	///	@param seek - where in the hierarchy to execute								
	///	@return true if succesfully executed											
	bool AUnit::DoInHierarchy(Verb& doWhat, SeekStyle seek) {
		if (mOwners.IsEmpty()) {
			pcLogSelfWarning << "No owners available for executing " << doWhat;
			return false;
		}

		SuccessTrap success;
		for (auto owner : mOwners)
			success = owner->DoInHierarchy(doWhat, seek);

		return success;
	}

	/// Move operator																				
	///	@param other - unit to move														
	AUnit& AUnit::operator = (AUnit&& other) noexcept {
		AContext::operator = (pcForward<AContext>(other));
		mOwners = pcMove(other.mOwners);
		// Update all coupled owners													
		for (auto owner : mOwners)
			owner->ReplaceUnit(&other, this);
		other.ClassInvalidate();
		return *this;
	}
	
	/// Couple the component with an entity												
	/// This will call refresh to all units in that entity on next frame			
	///	@param entity - the entity to couple with										
	void AUnit::Couple(Entity* entity) {
		if (!entity)
			return;
		entity->AddUnit(this);
	}

	/// Decouple the component from an entity												
	/// This will call refresh to all units in that entity on next frame			
	///	@param entity - the entity to decouple with									
	void AUnit::Decouple(Entity* entity) {
		if (!entity)
			return;
		entity->RemoveUnit(this);
	}

	/// Replace one owner instance with another (used when moving entities)		
	///	@param replaceThis - owner to replace											
	///	@param withThis - entity to replace it with									
	void AUnit::ReplaceOwner(Entity* replaceThis, Entity* withThis) {
		if (replaceThis == withThis || !replaceThis || !withThis)
			return;

		auto found = mOwners.Find(replaceThis);
		if (found.IsSpecial())
			return;

		mOwners[pcptr(found.mIndex)] = withThis;
	}

	/// Get the runtime																			
	///	@param index - the index of the runtime we seek								
	///	@return a pointer to the runtime if available								
	CRuntime* AUnit::GetRuntime(const Index& index) {
		if (index == uiFirst && !mOwners.IsEmpty())
			return mOwners[0]->GetRuntime();

		CRuntime* result = nullptr;
		for (auto owner : mOwners) {
			result = owner->GetRuntime(index);
			if (result)
				break;
		}

		return result;
	}

} // namespace PCFW
