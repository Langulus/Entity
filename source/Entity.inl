#pragma once
#include "Entity.hpp"

#define ENTITY_VERBOSE_SELF(a)				//Logger::Verbose() << this << ": "<< a
#define ENTITY_VERBOSE(a)						//Logger::Append() << a
#define ENTITY_CREATION_VERBOSE_SELF(a)	//Logger::Verbose() << this << ": " <<a
#define ENTITY_SELECTION_VERBOSE_SELF(a)	//Logger::Verbose() << this << ": " <<a

namespace Langulus::Entity
{
	
	/// Execute verb in the hierarchy														
	///	@tparam SEEK - where in the hierarchy to execute							
	///	@param verb - the verb to execute												
	///	@return true if succesfully executed											
	template<SeekStyle SEEK>
	bool Entity::DoInHierarchy(Verb& verb) {
		// Execute in owners directly													
		if constexpr (SEEK & SeekStyle::Here) {
			Do(verb);
			if (verb.IsDone())
				return true;
		}

		// Execute in parents up to root, if requested							
		if constexpr (SEEK & SeekStyle::Above) {
			if (mOwner && mOwner->DoInHierarchy<SeekStyle::UpToHere>(verb))
				return true;
		}

		// Execute in children, if requested										
		if constexpr (SEEK & SeekStyle::Below) {
			for (auto& child : mChildren) {
				if (child.DoInHierarchy<SeekStyle::DownFromHere>(verb))
					return true;
			}
		}

		return false;
	}

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
	///	@return a container filled with the matches									
	template<CT::Unit T, SeekStyle SEEK>
	TAny<const Decay<T>*> Entity::GatherUnits() const {
		return GatherUnits<SEEK>(MetaData::Of<Decay<T>>());
	}
	
	/// Collects all units of the given type inside the hierarchy					
	///	@tparam SEEK - where in the hierarchy are we seeking in?					
	///	@param meta - the units to seek for												
	///	@return the gathered units that match the type								
	template<SeekStyle SEEK>
	TAny<const Unit*> Entity::GatherUnits(DMeta meta) const {
		TAny<const Unit*> result;

		// Seek here if requested														
		if constexpr (SEEK & SeekStyle::Here) {
			for (auto unitList : mUnits) {
				for (auto unit : unitList.mValue) {
					if (unit->CastsTo(meta))
						result << unit;
				}
			}
		}

		// Seek in parents up to root, if requested								
		if constexpr (SEEK & SeekStyle::Above) {
			if (mOwner) {
				auto inParents = mOwner->GatherUnits<SeekStyle::UpToHere>(meta);
				result.SmartPush(Abandon(inParents));
			}
		}

		// Seek children, if requested												
		if constexpr (SEEK & SeekStyle::Below) {
			for (const auto& child : mChildren) {
				auto inChildren = child.GatherUnits<SeekStyle::DownFromHere>(meta);
				result.SmartPush(Abandon(inChildren));
			}
		}

		return result;
	}

	/// Find a specific unit, searching into the hierarchy							
	///	@tparam SEEK - where in the hierarchy are we seeking in?					
	///	@param meta - the unit to seek for												
	///	@param offset - which of the matches to return								
	///	@return the found unit, or nullptr if no such unit was found			
	template<SeekStyle SEEK>
	Unit* Entity::SeekUnit(DMeta meta, const Index& offset) {
		Unit* result = nullptr;

		// Seek here if requested														
		if constexpr (SEEK & SeekStyle::Here) {
			result = GetUnit(meta, offset);
			if (result)
				return result;
		}

		// Seek in parents up to root, if requested								
		if constexpr (SEEK & SeekStyle::Above) {
			if (mOwner) {
				result = mOwner->SeekUnit<SeekStyle::UpToHere>(meta, offset);
				if (result)
					return result;
			}
		}

		// Seek children, if requested												
		if constexpr (SEEK & SeekStyle::Below) {
			for (auto& child : mChildren) {
				result = child.SeekUnit<SeekStyle::DownFromHere>(meta, offset);
				if (result)
					return result;
			}
		}

		return nullptr;
	}

	/// Find a unit by type and index from the hierarchy								
	///	@param seek - where to seek for the unit										
	///	@param unit - the type of the unit to seek for								
	///	@param offset - the index of the unit to return								
	///	@return the unit if found, or nullptr otherwise								
	template<SeekStyle SEEK>
	const Unit* Entity::SeekUnit(DMeta unit, const Index& offset) const {
		return const_cast<Entity*>(this)->SeekUnit<SEEK>(unit, offset);
	}

	/// Find a unit by index and static type from the hierarchy (const)			
	///	@param seek - where to seek for the unit										
	///	@param offset - the index of the unit to return								
	///	@return the unit if found, or nullptr otherwise								
	template<CT::Unit T, SeekStyle SEEK>
	const Decay<T>* Entity::SeekUnit(const Index& offset) const {
		return static_cast<const Decay<T>*>(
			SeekUnit<SEEK>(MetaData::Of<Decay<T>>(), offset));
	}

	/// Find a unit by index and static type from the hierarchy						
	///	@param seek - where to seek for the unit										
	///	@param offset - the index of the unit to return								
	///	@return the unit if found, or nullptr otherwise								
	template<CT::Unit T, SeekStyle SEEK>
	Decay<T>* Entity::SeekUnit(const Index& offset) {
		return static_cast<Decay<T>*>(
			SeekUnit<SEEK>(MetaData::Of<Decay<T>>(), offset));
	}

	/// Create a unit by static type and arguments, relying on producers			
	/// in the hierarchy																			
	///	@return the newly created unit if able to create it						
	template<CT::Unit T, class... A>
	Decay<T>* Entity::CreateUnit(A&&...arguments) {
		TAny<Unit*> produced;
		auto request = Construct::From<Decay<T>>();
		if constexpr (sizeof...(arguments) > 0)
			(request << ... << Forward<A>(arguments));

		CreateUnitInner(request, produced);
		SAFETY(if (produced.IsEmpty())
			Throw<Except::Construct>("Bad unit creation in Entity::CreateUnit"));
		return produced.As<Decay<T>*>();
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

	template<SeekStyle SEEK, CT::Data T>
	bool Entity::SeekValue(TMeta trait, T& value) const {
		Trait found;
		if (!SeekTrait<SEEK>(trait, found) || found.IsEmpty())
			return false;

		value = found.AsCast<T>();
		return true;
	}

	template<SeekStyle SEEK, CT::Trait T, CT::Data D>
	bool Entity::SeekValue(D& value) const {
		static const auto tmeta = MetaTrait::Of<T>();
		Trait found;
		if (!SeekTrait<SEEK>(tmeta, found) || found.IsEmpty())
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

	/// Find a trait, searching into the hierarchy (const)							
	///	@param seek - direction to search at											
	///	@param var - the trait to search for											
	///	@param output - found traits go there											
	///	@param offset - the offset to apply												
	///	@return true if anything matching was found									
	template<SeekStyle SEEK>
	bool Entity::SeekTrait(TMeta var, Trait& output, const Index& offset) const {
		// Seek here if requested														
		if constexpr (SEEK & SeekStyle::Here) {
			if (GetTrait(var, output))
				return true;
		}

		// Seek in parents up to root, if requested								
		if constexpr (SEEK & SeekStyle::Above) {
			if (mOwner && mOwner->SeekTrait<SeekStyle::UpToHere>(var, output, offset))
				return true;
		}

		// Seek children, if requested												
		if constexpr (SEEK & SeekStyle::Below) {
			for (const auto& child : mChildren) {
				if (child.SeekTrait<SeekStyle::DownFromHere>(var, output, offset))
					return true;
			}
		}

		return false;
	}

	/// Find a trait, searching into the hierarchy										
	///	@param seek - direction to search at											
	///	@param var - the trait to search for											
	///	@param output - found traits go there											
	///	@param offset - the offset to apply												
	///	@return true if anything matching was found									
	template<SeekStyle SEEK>
	bool Entity::SeekTrait(TMeta var, Trait& output, const Index& offset) {
		return const_cast<const Entity*>(this)->SeekTrait<SEEK>(var, output, offset);
	}

} // namespace Langulus::Entity

#undef ENTITY_VERBOSE_SELF
#undef ENTITY_VERBOSE
#undef ENTITY_CREATION_VERBOSE_SELF
#undef ENTITY_SELECTION_VERBOSE_SELF
