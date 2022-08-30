namespace PCFW
{

	/// Get an entity identifier if not PC_PARANOID										
	///	@return a hex of the entity's pointer if not paranoid						
	inline Text Entity::GetID() const {
		#if LANGULUS_DEBUG() && PC_PARANOID == PC_DISABLED
			return pcToHex(this);
		#else
			return "...";
		#endif
	}

	inline bool Entity::operator != (const Entity& other) const {
		return !(*this == other);
	}

	/// Remove all units that are derived from the provided type					
	///	@tparam T - the type of units to remove										
	///	@return the number of removed units												
	template<Unit T>
	pcptr Entity::RemoveUnits() {
		static const auto metaT = MetaData::Of<T>();
		pcptr removed = 0;
		for (pcptr i = 0; i < mUnits.GetCount(); ++i) {
			auto unit = mUnits[i];
			if (unit->ClassInterpretsAs(metaT)) {
				ENTITY_VERBOSE_SELF(*unit << " removed");
				unit->mOwners.Remove(this);
				removed += mUnits.RemoveIndex(i);
				--i;
			}
		}

		if (removed)
			mRefreshRequired = true;
		return removed;
	}

	/// Gather all units of a specific static type										
	///	@param seek - where to seek for these units									
	///	@return a container filled with the matches									
	template<Unit T>
	TAny<const pcDecay<T>*> Entity::GatherUnits(SeekStyle seek) const {
		return GatherUnits(seek, MetaData::Of<pcDecay<T>>());
	}

	/// Create a unit by static type and arguments, relying on producers			
	/// in the hierarchy																			
	///	@return the newly created unit if able to create it						
	template<Unit T, class ... ARGUMENTS>
	pcDecay<T>* Entity::CreateUnit(ARGUMENTS&&...arguments) {
		TAny<AUnit*> produced;
		auto request = Construct::From<pcDecay<T>>();
		if constexpr (sizeof...(arguments) > 0)
			(request << ... << pcForward<ARGUMENTS>(arguments));
		CreateUnitInner(request, produced);
		SAFETY(if (produced.IsEmpty()) throw Except::BadConstruction());
		return produced.As<pcDecay<T>*>();
	}

	/// Find a unit by type and index from the hierarchy								
	///	@param seek - where to seek for the unit										
	///	@param unit - the type of the unit to seek for								
	///	@param offset - the index of the unit to return								
	///	@return the unit if found, or nullptr otherwise								
	inline const AUnit* Entity::SeekUnit(SeekStyle seek, DMeta unit, const Index& offset) const {
		return const_cast<Entity*>(this)->SeekUnit(seek, unit, offset);
	}

	/// Find a unit by index and static type from the hierarchy (const)			
	///	@param seek - where to seek for the unit										
	///	@param offset - the index of the unit to return								
	///	@return the unit if found, or nullptr otherwise								
	template<Unit T>
	const pcDecay<T>* Entity::SeekUnit(SeekStyle seek, const Index& offset) const {
		return static_cast<const pcDecay<T>*>(
			SeekUnit(seek, MetaData::Of<pcDecay<T>>(), offset));
	}

	/// Find a unit by index and static type from the hierarchy						
	///	@param seek - where to seek for the unit										
	///	@param offset - the index of the unit to return								
	///	@return the unit if found, or nullptr otherwise								
	template<Unit T>
	pcDecay<T>* Entity::SeekUnit(SeekStyle seek, const Index& offset) {
		return static_cast<pcDecay<T>*>(
			SeekUnit(seek, MetaData::Of<pcDecay<T>>(), offset));
	}

	inline const AUnit* Entity::GetUnit(DMeta type, const Index& offset) const {
		return const_cast<Entity*>(this)->GetUnit(type, offset);
	}

	template<Unit T>
	pcDecay<T>* Entity::GetUnit(const Index& offset) {
		return static_cast<pcDecay<T>*>(
			GetUnit(MetaData::Of<pcDecay<T>>(), offset));
	}

	template<Unit T>
	const pcDecay<T>* Entity::GetUnit(const Index& offset) const {
		return static_cast<const pcDecay<T>*>(
			GetUnit(MetaData::Of<pcDecay<T>>(), offset));
	}

	inline const AUnit* Entity::GetUnit(const Text& name, const Index& offset) const {
		return const_cast<Entity*>(this)->GetUnit(name, offset);
	}

	template<Unit T>
	pcDecay<T>* Entity::GetUnitT(const Text& name, const Index& offset) {
		return static_cast<pcDecay<T>*>(GetUnit(name, offset));
	}

	template<RTTI::ReflectedTrait TRAIT, RTTI::ReflectedData DATA>
	Trait* Entity::AddTrait(const DATA& data) {
		return AddTrait(Trait::From<TRAIT, DATA>(data));
	}

	inline bool Entity::GetTrait(TMeta trait, Trait& output, const Index& offset) const {
		return const_cast<Entity*>(this)->GetTrait(trait, output, offset);
	}

	template<RTTI::ReflectedData T>
	bool Entity::SeekValue(SeekStyle seek, TMeta trait, T& value) const {
		Trait found;
		if (!SeekTrait(seek, trait, found) || found.IsEmpty())
			return false;

		value = found.AsCast<T>();
		return true;
	}

	template<RTTI::ReflectedTrait TRAIT, RTTI::ReflectedData DATA>
	bool Entity::SeekValue(SeekStyle seek, DATA& value) const {
		static const auto tmeta = TraitID::Of<TRAIT>.GetMeta();
		Trait found;
		if (!SeekTrait(seek, tmeta, found) || found.IsEmpty())
			return false;

		value = found.AsCast<DATA>();
		return true;
	}

	inline bool Entity::GetTrait(const Trait& trait, Trait& output, const Index& offset) const {
		return const_cast<Entity*>(this)->GetTrait(trait, output, offset);
	}

	/// Get local trait by static type and offset										
	///	@tparam TRAIT - the trait to search for										
	///	@param offset - the offset of the trait to return							
	///	@return the trait or nullptr if none found									
	template<RTTI::ReflectedTrait TRAIT>
	Trait* Entity::GetLocalTrait(const Index& offset) {
		return GetLocalTrait(TraitID::Of<TRAIT>.GetMeta(), offset);
	}

	/// Get local trait by static type and offset										
	///	@tparam TRAIT - the trait to search for										
	///	@param offset - the offset of the trait to return							
	///	@return the trait or nullptr if none found									
	template<RTTI::ReflectedTrait TRAIT>
	const Trait* Entity::GetLocalTrait(const Index& offset) const {
		return const_cast<Entity&>(*this).GetLocalTrait<TRAIT>(offset);
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
	inline const Entity* Entity::GetChild(const Text& name, const Index& offset) const {
		return const_cast<Entity*>(this)->GetChild(name, offset);
	}

	/// Get a trait by scanning owners and other units									
	///	@return the trait																		
	template<RTTI::ReflectedTrait TRAIT>
	Trait AUnit::SeekTrait() const {
		SuccessTrap satisfied;
		Trait output;
		static const auto traitMeta = TraitID::Of<TRAIT>.GetMeta();
		for (auto context : mOwners) {
			bool found = context->SeekTrait(SeekStyle::UpToHere, traitMeta, output);
			if (found && satisfied) {
				pcLogSelfWarning << "Multiple " << traitMeta->GetToken() << " traits found in hierarchy";
				pcLogSelfWarning << "Each sequential trait will overwrite the result";
			}

			satisfied = found;
		}

		return output;
	}

	/// Get a value from inside a trait by scanning owners and other units		
	///	@param trait - the trait to seek for											
	///	@param value - [out] value to set if trait was found						
	///	@return true if anything was written to value								
	template<RTTI::ReflectedData DATA>
	bool AUnit::SeekValue(TMeta trait, DATA& value) const {
		SuccessTrap satisfied;
		for (auto context : mOwners) {
			bool found = context->SeekValue(SeekStyle::UpToHere, trait, value);
			if (found && satisfied) {
				pcLogSelfWarning << "Multiple " << trait->GetToken() << " traits found in hierarchy";
				pcLogSelfWarning << "Each sequential trait will overwrite the result";
			}

			satisfied = found;
		}

		return satisfied;
	}

} // namespace PCFW
