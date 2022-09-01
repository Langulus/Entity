#include "Unit.hpp"
#include "Entity.hpp"

namespace Langulus::Entity
{

	/// Move unit																					
	///	@param other - the unit to move													
	Unit::Unit(Unit&& other) noexcept
		: Resolvable {Forward<Resolvable>(other)}
		, mOwners {Move(other.mOwners)} {
		// Replace the owner's unit pointer with the new one					
		for (auto owner : mOwners)
			owner->ReplaceUnit(&other, this);
	}

	/// Manual construction																		
	///	@param classid - type of the unit												
	Unit::Unit(DMeta classid) noexcept
		: Resolvable {classid} {}

	/// AUnit destruction																		
	Unit::~Unit() {
		for (auto owner : mOwners)
			Decouple(owner);
	}

	/// Default unit selection simply relays to the owner								
	///	@param verb - the selection verb													
	void Unit::Select(Verb& verb) {
		for (auto owner : mOwners)
			owner->Select(verb);
	}

	/// Get the runtime																			
	///	@attention assumes units are correctly coupled and coupling to			
	///				  different runtimes is never allowed by Entity					
	///	@return a pointer to the runtime, if available								
	Runtime* Unit::GetRuntime() noexcept {
		for (auto owner : mOwners)
			return owner->GetRuntime();
		return nullptr;
	}

	/// Move operator																				
	///	@param other - unit to move														
	Unit& Unit::operator = (Unit&& other) noexcept {
		Resolvable::operator = (Forward<Resolvable>(other));
		mOwners = Move(other.mOwners);

		// Update all coupled owners													
		for (auto owner : mOwners)
			owner->ReplaceUnit(&other, this);
		return *this;
	}
	
	/// Couple the component with an entity												
	/// This will call refresh to all units in that entity on next frame			
	///	@param entity - the entity to couple with										
	void Unit::Couple(Entity* entity) {
		if (!entity)
			return;
		entity->AddUnit(this);
	}

	/// Decouple the component from an entity												
	/// This will call refresh to all units in that entity on next frame			
	///	@param entity - the entity to decouple with									
	void Unit::Decouple(Entity* entity) {
		if (!entity)
			return;
		entity->RemoveUnit(this);
	}

	/// Replace one owner instance with another (used when moving entities)		
	///	@attention assumes both pointers are different, and not nullptr		
	///	@attention - internal function, should be called from Entity			
	///	@param replaceThis - owner to replace											
	///	@param withThis - entity to replace it with									
	void Unit::ReplaceOwner(Entity* replaceThis, Entity* withThis) {
		const auto found = mOwners.Find(replaceThis);
		if (found)
			mOwners[found] = withThis;
	}

} // namespace Langulus::Entity
