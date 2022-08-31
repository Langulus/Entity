#pragma once
#include "Entity.hpp"

namespace Langulus::Entity
{

	/// Remove all units that are derived from the provided type					
	///	@tparam T - the type of units to remove										
	///	@return the number of removed units												
	template<CT::Unit T>
	Count Entity::RemoveUnits() {
		const auto found = mUnits.FindKeyIndex(MetaData::Of<T>());
		if (found) {
			auto& list = mUnits.GetValue(found);
			for (auto unit : list) {
				ENTITY_VERBOSE_SELF(unit << " removed");
				unit->mOwners.Remove(this);
			}

			const auto removed = list.GetCount();
			mUnits.RemoveIndex(found);
			mRefreshRequired = true;
			return removed;
		}

		return 0;
	}

	/// Gather all units of a specific static type										
	///	@param seek - where to seek for these units									
	///	@return a container filled with the matches									
	template<CT::Unit T>
	TAny<const Decay<T>*> Entity::GatherUnits(SeekStyle seek) const {
		return GatherUnits(seek, MetaData::Of<Decay<T>>());
	}

	/// Create a unit by static type and arguments, relying on producers			
	/// in the hierarchy																			
	///	@return the newly created unit if able to create it						
	template<CT::Unit T, class ... ARGUMENTS>
	Decay<T>* Entity::CreateUnit(ARGUMENTS&&...arguments) {
		TAny<Unit*> produced;
		auto request = Construct::From<Decay<T>>();
		if constexpr (sizeof...(arguments) > 0)
			(request << ... << Forward<ARGUMENTS>(arguments));
		CreateUnitInner(request, produced);
		SAFETY(if (produced.IsEmpty())
			Throw<Except::Construct>("Cad unit creation in Entity::CreateUnit"));
		return produced.As<Decay<T>*>();
	}

	/// Find a unit by type and index from the hierarchy								
	///	@param seek - where to seek for the unit										
	///	@param unit - the type of the unit to seek for								
	///	@param offset - the index of the unit to return								
	///	@return the unit if found, or nullptr otherwise								
	inline const Unit* Entity::SeekUnit(SeekStyle seek, DMeta unit, const Index& offset) const {
		return const_cast<Entity*>(this)->SeekUnit(seek, unit, offset);
	}

	/// Find a unit by index and static type from the hierarchy (const)			
	///	@param seek - where to seek for the unit										
	///	@param offset - the index of the unit to return								
	///	@return the unit if found, or nullptr otherwise								
	template<CT::Unit T>
	const Decay<T>* Entity::SeekUnit(SeekStyle seek, const Index& offset) const {
		return static_cast<const Decay<T>*>(
			SeekUnit(seek, MetaData::Of<Decay<T>>(), offset));
	}

	/// Find a unit by index and static type from the hierarchy						
	///	@param seek - where to seek for the unit										
	///	@param offset - the index of the unit to return								
	///	@return the unit if found, or nullptr otherwise								
	template<CT::Unit T>
	Decay<T>* Entity::SeekUnit(SeekStyle seek, const Index& offset) {
		return static_cast<Decay<T>*>(
			SeekUnit(seek, MetaData::Of<Decay<T>>(), offset));
	}

	inline const Unit* Entity::GetUnit(DMeta type, const Index& offset) const {
		return const_cast<Entity*>(this)->GetUnit(type, offset);
	}

	template<CT::Unit T>
	Decay<T>* Entity::GetUnit(const Index& offset) {
		return static_cast<Decay<T>*>(
			GetUnit(MetaData::Of<Decay<T>>(), offset));
	}

	template<CT::Unit T>
	const Decay<T>* Entity::GetUnit(const Index& offset) const {
		return static_cast<const Decay<T>*>(
			GetUnit(MetaData::Of<Decay<T>>(), offset));
	}

	inline const Unit* Entity::GetUnit(const Token& token, const Index& offset) const {
		return const_cast<Entity*>(this)->GetUnit(token, offset);
	}

	template<CT::Unit T>
	Decay<T>* Entity::GetUnitAs(const Token& token, const Index& offset) {
		return static_cast<Decay<T>*>(GetUnit(token, offset));
	}

	template<CT::Trait T, CT::Data D>
	Trait* Entity::AddTrait(const D& data) {
		return AddTrait(Trait::From<T, D>(data));
	}

	inline bool Entity::GetTrait(TMeta trait, Trait& output, const Index& offset) const {
		return const_cast<Entity*>(this)->GetTrait(trait, output, offset);
	}

	template<CT::Data T>
	bool Entity::SeekValue(SeekStyle seek, TMeta trait, T& value) const {
		Trait found;
		if (!SeekTrait(seek, trait, found) || found.IsEmpty())
			return false;

		value = found.AsCast<T>();
		return true;
	}

	template<CT::Trait T, CT::Data D>
	bool Entity::SeekValue(SeekStyle seek, D& value) const {
		static const auto tmeta = MetaTrait::Of<T>();
		Trait found;
		if (!SeekTrait(seek, tmeta, found) || found.IsEmpty())
			return false;

		value = found.AsCast<D>();
		return true;
	}

	inline bool Entity::GetTrait(const Trait& trait, Trait& output, const Index& offset) const {
		return const_cast<Entity*>(this)->GetTrait(trait, output, offset);
	}

	/// Get local trait by static type and offset										
	///	@tparam TRAIT - the trait to search for										
	///	@param offset - the offset of the trait to return							
	///	@return the trait or nullptr if none found									
	template<CT::Trait T>
	Trait* Entity::GetLocalTrait(const Index& offset) {
		return GetLocalTrait(MetaTrait::Of<T>(), offset);
	}

	/// Get local trait by static type and offset										
	///	@tparam TRAIT - the trait to search for										
	///	@param offset - the offset of the trait to return							
	///	@return the trait or nullptr if none found									
	template<CT::Trait T>
	const Trait* Entity::GetLocalTrait(const Index& offset) const {
		return const_cast<Entity&>(*this).GetLocalTrait<T>(offset);
	}

	/// Get child entity by offset															
	///	@param offset - the offset of the entity to return							
	///	@return the child or nullptr if none found									
	inline const Entity* Entity::GetChild(const Index& offset) const {
		return const_cast<Entity*>(this)->GetChild(offset);
	}

	/// Get child entity by name and offset												
	///	@param name - the name trait to search for									
	///	@param offset - the offset of the matching entity to return				
	///	@return the child or nullptr if none found									
	inline const Entity* Entity::GetChild(const Token& name, const Index& offset) const {
		return const_cast<Entity*>(this)->GetChild(name, offset);
	}

	/// Get a trait by scanning owners and other units									
	///	@return the trait																		
	template<CT::Trait T>
	Trait Unit::SeekTrait() const {
		bool satisfied {};
		Trait output;
		static const auto data = MetaTrait::Of<T>();
		for (auto context : mOwners) {
			bool found = context->SeekTrait(SeekStyle::UpToHere, data, output);
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
	template<CT::Data D>
	bool Unit::SeekValue(TMeta trait, D& value) const {
		bool satisfied {};
		for (auto context : mOwners) {
			bool found = context->SeekValue(SeekStyle::UpToHere, trait, value);
			if (found && satisfied) {
				Logger::Warning() << "Multiple " << trait << " traits found in hierarchy";
				Logger::Warning() << "Each sequential trait will overwrite the result";
			}

			satisfied |= found;
		}

		return satisfied;
	}

} // namespace Langulus::Entity
