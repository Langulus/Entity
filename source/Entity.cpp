#include "Entity.hpp"
#include "Runtime.hpp"

namespace Langulus::Entity
{

	/// Construct as a child																	
	///	@param parent - the owner of the entity										
	Entity::Entity(Entity* parent) noexcept
		: Resolvable {MetaData::Of<Entity>()}
		, mOwner {parent}
		, mRuntime {parent ? parent->GetRuntime() : nullptr} {}

	/// Move constructor																			
	///	@param other - move that entity													
	Entity::Entity(Entity&& other) noexcept
		: Resolvable {Forward<Resolvable>(other)}
		, mOwner {Move(other.mOwner)}
		, mChildren {Move(other.mChildren)}
		, mUnits {Move(other.mUnits)}
		, mTraits {Move(other.mTraits)}
		, mRuntime {Move(other.mRuntime)} {
		// Remap children																	
		for (auto child : mChildren)
			child->mOwner = this;

		// Remap components																
		for (auto unit : mUnits)
			unit->ReplaceOwner(&other, this);
	}

	/// Compare two entities																	
	///	@param other - entity to compare with											
	///	@return true if entities are functionally same								
	bool Entity::operator == (const Entity& other) const {
		return mChildren == other.mChildren && mUnits == other.mUnits && mTraits == other.mTraits;
	}

	/// Convert to text, by writing a short name or address							
	Entity::operator Debug() const {
		const auto name = GetName();
		if (!name.IsEmpty()) {
			Debug result;
			result += "#";
			result += name;
			return result;
		}
		else return Resolvable::operator Debug();
	}

	/// Dump the entity's hierarchy in log													
	void Entity::DumpHierarchy() const {
		const auto tab = Logger::Verbose() << "** " << *this << Logger::Tabs {};

		if (!mTraits.IsEmpty()) {
			Logger::Verbose() << ".. contains " << mTraits.GetCount() << " traits";
			for (auto& trait : mTraits)
				Logger::Verbose() << ". " << trait;
		}

		if (!mUnits.IsEmpty()) {
			Logger::Verbose() << "++ contains " << mUnits.GetCount() << " units";
			for (auto unit : mUnits)
				Logger::Verbose() << "+ " << unit;
		}

		if (!mChildren.IsEmpty()) {
			Logger::Verbose() << "** contains " << mChildren.GetCount() << " child entities";
			for (auto child : mChildren)
				child->DumpHierarchy();
		}
	}

	/// Execute a piece of code in the closest possible flow driver				
	///	@param code - the code to execute												
	///	@return the results of the code													
	Any Entity::RunCode(const Flow::Code& code) {
		if (code.IsEmpty())
			return {};

		// Execute code																	
		Any parsed = code.Parse();
		Any context {GetBlock()};
		Any output;
		if (!Flow::Execute(context, parsed, output)) {
			Logger::Error() << "DoGASM failed to execute " << parsed;
			return {};
		}

		return output;
	}

	/// Find an AIAD component that can process speech and interpret text		
	///	@param text - text to execute														
	///	@return true if the text was successfully executed							
	Any Entity::RunSpeech(const Text& text) {
		if (text.IsEmpty())
			return true;

		//TODO do this with higher level functions, entity should never know anything about units
		// Find an AI interpreter and a flow										
		/*auto mind = SeekUnit<AUnitAI>(SeekStyle::UpToHere);
		if (!mind) {
			pcLogSelfError << "No AI unit found - can't parse natural speech: \"" << text << "\"";
			return false;
		}

		// Interpret the text into a flow											
		auto flow = SeekUnit<CFlow>(SeekStyle::UpToHere);
		if (!flow) {
			pcLogSelfError << "No CFlow unit found - can't build temporal flows";
			return false;
		}

		mind->DoAIAD(text, *flow);

		// Execute the flow																
		auto classBlock = GetBlock();
		flow->Execute(classBlock);
		return true;*/
	}

	/// Update all children's CRuntime/CFlow components								
	/// This is used for the purpose of a main loop, but also supports sub		
	/// systems and multiple system instantiations with different environments	
	///	@param dt - delta time that has passed between updates					
	void Entity::Update(Time dt) {
		// Keep a flow inside the runtime?
		/*
		auto thisBlock = GetBlock();
		for (auto unit : mUnits) {
			if (unit->IsClassIrrelevant())
				continue;

			if (unit->ClassIs<CFlow>()) {
				// Update every flow														
				// This will execute all verbs up to current	time, as well	
				// as verbs with specific time and frequency						
				static_cast<CFlow*>(unit)->Update(thisBlock, dt);
			}
			else if (unit->ClassIs<CRuntime>()) {
				// Update every system													
				// This will also update all modules that	are instantiated	
				// by the system component												
				static_cast<CRuntime*>(unit)->Update(dt);
			}
		}

		// Refresh all units if requested											
		if (mRefreshRequired) {
			mRefreshRequired = false;
			for (auto unit : mUnits)
				unit->Refresh();
		}

		// Nest update inside all children											
		for (auto child : mChildren) {
			if (child->IsClassIrrelevant())
				continue;
			child->Update(dt);
		}*/
	}

	/// Reset the entity, clearing all children, units, traits						
	void Entity::Reset() {
		mChildren.Clear();
		mUnits.Clear();
		mTraits.Clear();
	}

	/// Check if this entity is other, or some kind of parent of it				
	///	@param other - the entity instance to test									
	///	@return true if this entity is either the other, or related upwards	
	bool Entity::IsFamilyOf(const Entity& other) const {
		if (this == &other)
			return true;

		auto parent = other.mOwner.Get();
		while (parent) {
			if (parent == this)
				return true;

			parent = parent->mOwner.Get();
		}

		return false;
	}

	/// Get a trait by type																		
	///	@param id - trait id																	
	///	@param output - [out] result of the search									
	///	@param offset - offset of result to use										
	///	@return true if anything was found												
	bool Entity::GetTrait(TMeta id, Trait& output, const Index& offset) {
		return GetTrait(Trait(id), output, offset);
	}

	/// Get a trait from the entity's trait list											
	///	@param id - the trait to search for												
	///	@param offset - the index of the trait we seek								
	///	@return a pointer to the trait, or nullptr if not found					
	Trait* Entity::GetLocalTrait(TMeta id, const Index& offset) {
		pcptr idx = static_cast<pcptr>(offset);
		pcptr matches = 0;
		for (auto& trait : mTraits) {
			if (trait.GetTraitMeta() == id) {
				if (idx == matches)
					return &trait;
				++matches;
			}
		}

		return nullptr;
	}

	/// Get a trait from the entity's trait list	(const)								
	///	@param id - the trait to search for												
	///	@param offset - the index of the trait we seek								
	///	@return a pointer to the trait, or nullptr if not found					
	const Trait* Entity::GetLocalTrait(TMeta id, const Index& offset) const {
		return const_cast<Entity&>(*this).GetLocalTrait(id, offset);
	}

	/// Get a trait by trait contents														
	///	@param id - trait to match															
	///	@param output - [out] result of the search									
	///	@param offset - offset of result to use										
	///	@return true if anything was found												
	bool Entity::GetTrait(const Trait& id, Trait& output, const Index& offset) {
		if (id == utInvalid || id.TraitIs<Traits::Trait>()) {
			// Type is invalid, so use offset as index							
			if (offset.IsValid() && offset < mTraits.GetCount()) {
				output = mTraits[offset];
				return true;
			}

			return false;
		}
		else if (id == Traits::System::ID) {
			// Get the nearest system component										
			auto system = GetRuntime(offset);
			if (system) {
				output = Trait{ id.GetTraitMeta(), Any{system} };
				return true;
			}

			return false;
		}
		else if (id == Traits::Unit::ID) {
			// Get a component															
			auto unit = GetUnit(nullptr, offset);
			if (unit) {
				output = Trait{ id.GetTraitMeta(), Any{unit} };
				return true;
			}

			return false;
		}
		else if (id == Traits::Child::ID) {
			// Get a child entity														
			auto child = GetChild(offset);
			if (child) {
				output = Trait{ id.GetTraitMeta(), Any{child} };
				return true;
			}

			return false;
		}

		// If type is valid, seek the offsetted trait							
		output = id;

		// First check traits															
		if (offset.IsSpecial())
			throw Except::BadAccess(pcLogSelfError << "Can't get trait with offset " << offset);

		auto found = GetLocalTrait(id.GetTraitMeta(), offset);
		if (found) {
			output = *found;
			return true;
		}

		// Then check each unit's static traits									
		pcptr idx = static_cast<pcptr>(offset);
		for (auto unit : mUnits) {
			auto t = unit->GetBlock().GetMember(id.GetTraitMeta(), idx);
			if (t.IsAllocated()) {
				output = t;
				return true;
			}
		}

		// Finally, check this entity's static traits							
		auto t = GetBlock().GetMember(id.GetTraitMeta(), idx);
		if (t.IsAllocated()) {
			output = t;
			return true;
		}

		return false;
	}

	/// Get a component (by type id)															
	/// If type is nullptr searches only by offset										
	/// If type is not nullptr, gets the offseth matching unit						
	///	@param type - the type of the unit												
	///	@param offset - the unit index													
	///	@return the unit if found, or nullptr if not									
	AUnit* Entity::GetUnit(DMeta type, const Index& offset) {
		if (!type && offset.IsValid())
			return mUnits[offset];

		// If type is valid, seek the offsetted component						
		if (offset.IsReverse()) {
			Index matches = -1;
			for (AUnit* unit : mUnits.Reverse()) {
				if (unit->ClassInterpretsAs(type)) {
					if (offset == matches)
						return unit;
					--matches;
				}
			}
		}
		else {
			Index matches = 0;
			for (AUnit* unit : mUnits) {
				if (unit->ClassInterpretsAs(type)) {
					if (offset == matches)
						return unit;
					++matches;
				}
			}
		}

		return nullptr;
	}

	/// Get a component (by name)																
	///	@param name - the type name of the unit										
	///	@param offset - the unit index													
	///	@return the unit if found, or nullptr if not									
	AUnit* Entity::GetUnit(const Text& name, const Index& offset) {
		return GetUnit(PCMEMORY.GetMetaData(name), offset);
	}

	/// Get a child by index																	
	///	@param id - the index to pick														
	///	@return the child entity, or nullptr of none was found					
	Entity* Entity::GetChild(const Index& id) {
		return mChildren[id];
	}

	/// Get child by name and offset (searches for utName in children)			
	///	@param name - name to seek															
	///	@param offset - offset to seek													
	///	@return the child entity, or nullptr of none was found					
	Entity* Entity::GetChild(const Text& name, const Index& offset) {
		Index matches = 0;
		for (auto child : mChildren) {
			if (child->GetName() == name) {
				if (matches == offset)
					return child;
				++matches;
			}
		}

		return nullptr;
	}

	/// Do a cascading runtime shortcut reset												
	void Entity::ResetRuntime(CRuntime* newrt) {
		mRuntime = newrt;
		for (auto child : mChildren)
			child->ResetRuntime(newrt);
	}

	/// Add a new unit to the entity 														
	/// You can duplicate units, as long as they're unique memorywise				
	///	@param unit - the unit to add														
	///	@return the number of added units												
	pcptr Entity::AddUnit(AUnit* unit) {
		if (mUnits.Find(unit) != uiNone)
			return 0;

		// Log self before unit being added, it might change name			
		ENTITY_VERBOSE_SELF("");
		unit->mOwners << this;
		mUnits << unit;
		unit->Reference(1);

		// Do the second creation stage for the unit								
		unit->Refresh();
		
		// And notify the owner, that all their components should be		
		// notified about the newly added unit										
		mRefreshRequired = true;

		ENTITY_VERBOSE(mUnits.Last() << " added");
		ENTITY_VERBOSE(" (now " << *this << ")");

		if (unit->ClassIs<CRuntime>())
			ResetRuntime(dynamic_cast<CRuntime*>(unit));
		return 1;
	}

	/// Remove a unit from the entity 														
	///	@param unit - unit to remove from the entity									
	///	@return the number of removed units												
	pcptr Entity::RemoveUnit(AUnit* unit) {
		ENTITY_VERBOSE_SELF(unit << " removed");
		const auto removed = mUnits.Remove(unit);
		if (removed > 0) {
			if (unit->ClassIs<CRuntime>())
				ResetRuntime();

			unit->Reference(-1);

			// Log self before unit being removed, it might change name		
			unit->mOwners.Remove(this);

			// Notify all other units in the owner of the change				
			mRefreshRequired = true;
			ENTITY_VERBOSE(" (now " << *this << ")");
		}

		return removed;
	}

	/// Replace one unit instance with another. Used when moving units			
	///	@param replaceThis - the unit to replace										
	///	@param withThis - the unit to replace with									
	void Entity::ReplaceUnit(AUnit* replaceThis, AUnit* withThis) {
		if (replaceThis == withThis || !replaceThis || !withThis)
			return;

		auto found = mUnits.Find(replaceThis);
		if (found.IsSpecial())
			return;
		
		auto element = mUnits[pcptr(found.mIndex)];
		element->Reference(-1);
		element = withThis;
		withThis->Reference(1);
	}

	/// A fast check whether units of the given type are registered				
	///	@param type - the type of units to seach for									
	///	@return the number of matching units											
	pcptr Entity::HasUnits(DMeta type) const {
		pcptr iterator = 0;
		for (auto unit : mUnits) {
			if (unit->ClassInterpretsAs(type))
				++iterator;
		}

		return iterator;
	}

	/// Collects all units of the given description inside the hierarchy			
	///	@param seek - the seeking style													
	///	@param meta - the unit to seek for												
	///	@return the gathered units															
	Any Entity::GatherUnits(SeekStyle seek, DMeta meta) const {
		Any result;

		// Seek here if requested														
		if (seek & SeekStyle::Here) {
			for (auto unit : mUnits) {
				if (unit->ClassInterpretsAs(meta))
					result << unit;
			}
		}

		// Seek in parents up to root, if requested								
		if (seek & SeekStyle::Backward && mOwner) {
			Any inParents = mOwner->GatherUnits(SeekStyle::UpToHere, meta);
			result.SmartPush(inParents);
		}

		// Seek children, if requested												
		if (seek & SeekStyle::Forward) {
			for (const auto child : mChildren) {
				Any inChildren = child->GatherUnits(SeekStyle::DownFromHere, meta);
				result.SmartPush(inChildren);
			}
		}

		return result;
	}

	/// Find a unit, searching into the hierarchy										
	///	@param seek - the seeking style													
	///	@param meta - the unit to seek for												
	///	@param offset - unit offset to take												
	///	@return the found unit, or nullptr if no such unit was found			
	AUnit* Entity::SeekUnit(SeekStyle seek, DMeta meta, const Index& offset) {
		AUnit* result = nullptr;

		// Seek here if requested														
		if (seek & SeekStyle::Here) {
			result = GetUnit(meta, offset);
			if (result)
				return result;
		}

		// Seek in parents up to root, if requested								
		if (seek & SeekStyle::Backward && mOwner) {
			result = mOwner->SeekUnit(SeekStyle::UpToHere, meta, offset);
			if (result)
				return result;
		}

		// Seek children, if requested												
		if (seek & SeekStyle::Forward) {
			for (auto child : mChildren) {
				result = child->SeekUnit(SeekStyle::DownFromHere, meta, offset);
				if (result)
					return result;
			}
		}

		return nullptr;
	}

	/// Add a new trait to the universal entity 											
	///	@param initializer - trait to shallow copy									
	///	@return the new trait instance													
	Trait* Entity::AddTrait(const Trait& initializer) {
		// Log self before trait being added - it might change name			
		ENTITY_VERBOSE_SELF("");
		mTraits << initializer;
		mRefreshRequired = true;
		ENTITY_VERBOSE(mTraits.Last() << " added");
		ENTITY_VERBOSE(" (now " << *this << ")");
		return &mTraits.Last();
	}

	/// Remove a trait from the universal entity											
	///	@param id - type of trait to remove												
	void Entity::RemoveTrait(TMeta id) {
		RemoveTrait(Trait(id));
	}

	/// Remove a trait from the universal entity											
	///	@param id - type and value to remove											
	void Entity::RemoveTrait(const Trait& id) {
		for (Trait& trait : mTraits) {
			if (trait == id) {
				ENTITY_VERBOSE_SELF(trait << " removed");
				mTraits.Remove(trait);
				mRefreshRequired = true;
				ENTITY_VERBOSE(" (now " << *this << ")");
				return;
			}
		}
	}

	/// A fast check whether traits of the given type are inside entity			
	///	@param id - type of trait to check												
	///	@return the number of matching traits											
	pcptr Entity::HasTraits(TMeta id) const {
		return HasTraits(Trait(id));
	}

	/// A fast check whether traits of the given type and value are included	
	///	@param id - trait to check															
	///	@return the number of matching traits											
	pcptr Entity::HasTraits(const Trait& id) const {
		pcptr counter = 0;
		for (const auto& trait : mTraits)
			counter += (trait == id ? 1 : 0);
		return counter;
	}

	/// Find a trait, searching into the hierarchy (const)							
	///	@param seek - direction to search at											
	///	@param var - the trait to search for											
	///	@param output - found traits go there											
	///	@param offset - the offset to apply												
	///	@return true if anything matching was found									
	bool Entity::SeekTrait(SeekStyle seek, TMeta var, Trait& output, const Index& offset) const {
		// Seek here if requested														
		if (seek & SeekStyle::Here && GetTrait(var, output))
			return true;

		// Seek in parents up to root, if requested								
		if (seek & SeekStyle::Backward && mOwner) {
			if (mOwner->SeekTrait(SeekStyle::UpToHere, var, output, offset))
				return true;
		}

		// Seek children, if requested												
		if (seek & SeekStyle::Forward) {
			for (const auto child : mChildren) {
				if (child->SeekTrait(SeekStyle::DownFromHere, var, output, offset))
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
	bool Entity::SeekTrait(SeekStyle seek, TMeta var, Trait& output, const Index& offset) {
		return const_cast<const Entity*>(this)->SeekTrait(seek, var, output, offset);
	}

	/// Get a runtime component from the hierarchy										
	///	@param offset - the unit offset													
	///	@return the pointer to the runtime component									
	CRuntime* Entity::GetRuntime(const Index& offset) {
		if (offset == uiFirst && mRuntime)
			return mRuntime;

		auto result = SeekUnit<CRuntime>(SeekStyle::UpToHere, offset);
		if (result && offset == uiFirst)
			mRuntime = result;
		return result;
	}

	/// Add/overwrite entity's name trait													
	///	@param name - the name to set														
	void Entity::SetName(const Text& name) {
		auto found = GetLocalTrait<Traits::Name>();
		if (!found)
			AddTrait<Traits::Name>(name);
		else
			*found = name;
	}

	/// Get entity's name trait, if any														
	/// It can reside in one of the units													
	///	@return the name, or empty string if no such trait was found here		
	Text Entity::GetName() const {
		Text name;
		if (!SeekValue<Traits::Name>(SeekStyle::Here, name))
			return {};
		return name;
	}

	/// Execute verb in hierarchy																
	///	@param doWhat - the verb to execute												
	///	@param seek - where in the hierarchy to execute								
	///	@return true if succesfully executed											
	bool Entity::DoInHierarchy(Verb& doWhat, SeekStyle seek) {
		// Execute in owners directly													
		if (seek & SeekStyle::Here) {
			Do(doWhat);
			if (doWhat.IsDone())
				return true;
		}

		// Execute in parents up to root, if requested							
		if (seek & SeekStyle::Backward && mOwner) {
			mOwner->DoInHierarchy(doWhat, SeekStyle::UpToHere);
			if (doWhat.IsDone())
				return true;
		}

		// Execute in children, if requested										
		if (seek & SeekStyle::Forward) {
			for (auto child : mChildren) {
				child->DoInHierarchy(doWhat, SeekStyle::DownFromHere);
				if (doWhat.IsDone())
					return true;
			}
		}

		return doWhat.IsDone();
	}

	/// Custom dispatcher																		
	///	@param verb - any verb																
	void Entity::Do(Verb& verb) {
		// Dispatch to this first, using reflected and default verbs		
		auto thisBlock = GetBlock();
		if (Verb::DispatchFlat(thisBlock, verb, false, false))
			return;

		// If verb is still not satisfied, multicast to all units			
		Verb verbCopy = verb;
		if (Verb::DispatchFlat(mUnits, verbCopy.ShortCircuit(false)))
			verb << pcMove(verbCopy.GetOutput());
	}

	/// Create/destroy stuff inside entity's context									
	///	@param verb - creation verb														
	void Entity::Create(Verb& verb) {
		if (verb.GetArgument().IsEmpty())
			return;

		TAny<Trait*> createdTraits;
		TAny<AUnit*> createdUnits;
		TAny<AModule*> createdModules;
		TAny<Entity*> createdChildren;

		const auto create = [&](const Construct& construct) {
			const auto count = pcptr(construct.GetCharge().mMass);
			for (pcptr i = 0; i < count; ++i) {
				if (count != 1) {
					ENTITY_CREATION_VERBOSE_SELF(ccYellow 
						<< "Charged creation - creating " << i + 1 << " of " << count);
				}

				if (construct.Is<Entity>())
					CreateChild(construct, createdChildren);
				else if (construct.InterpretsAs<AUnit>())
					CreateUnitInner(construct, createdUnits);
				else if (construct.InterpretsAs<AModule>())
					GetRuntime()->InstantiateModule(construct, createdModules);
			}
		};

		// Scan the request																
		verb.GetArgument().ForEachDeep([&](const Block& group) {
			EitherDoThis
				group.ForEach([this, &createdTraits](const Trait& trait) {
					ENTITY_CREATION_VERBOSE_SELF(
						"Creating: " << ccYellow << trait);
					createdTraits << AddTrait(trait);
				})
			OrThis
				group.ForEach([this, &create](const Construct& construct) {
					if (construct.mCharge.mMass > 0) {
						ENTITY_CREATION_VERBOSE_SELF(
							"Creating: " << ccYellow << construct);
						create(construct);
					}
					else TODO();
				})
			OrThis
				group.ForEach([this, &create](const DataID& type) {
					ENTITY_CREATION_VERBOSE_SELF(
						"Creating: " << ccYellow << type);
					create(Construct(type));
				})
			OrThis
				group.ForEach([this, &create](const MetaData* type) {
					ENTITY_CREATION_VERBOSE_SELF(
						"Creating: " << ccYellow << type->GetToken());
					create(Construct(type));
				});
		});

		// Output and satisfy verb														
		verb << createdTraits.Decay();
		verb << createdUnits.Decay();
		verb << createdModules.Decay();
		verb << createdChildren.Decay();
	}

	/// Pick something from the entity - children, traits, units, modules		
	/// If this entity doesn't satisfy the query, a search will be performed	
	/// in all parents, climbing the hierarchy											
	///	@param verb - the selection verb													
	void Entity::Select(Verb& verb) {
		// Probe every part of the argument and check if it matches			
		TAny<Trait> selectedTraits;
		TAny<AUnit*> selectedUnits;
		TAny<Entity*> selectedEntities;
		bool mismatch = false;

		const auto selectTrait = [this,&selectedTraits,&mismatch](const MetaTrait* trait) {
			Trait found;
			if (!GetTrait(trait, found)) {
				mismatch = true;
				return false;
			}

			selectedTraits << found;
			return true;
		};

		const auto selectUnit = [this,&selectedUnits,&mismatch](const MetaData* type) {
			if (type->Is<Entity>())
				return true;

			auto found = GetUnit(type);
			if (!found) {
				mismatch = true;
				return false;
			}

			selectedUnits << found;
			return true;
		};

		const auto selectConstruct = [&](const Construct& construct) {
			if (construct.Is<Entity>()) {
				// Find an entity containing construct arguments				
				// Start with this one													
				auto selector = Verb::From<Verbs::Select>({}, construct.GetAll());
				Select(selector);
				if (!selector.GetOutput().IsEmpty()) {
					selectedEntities << this;
					return true;
				}
			}
			else if (construct.InterpretsAs<AUnit>()) {
				// Find a unit containing construct arguments					
				if (!selectUnit(construct.GetMeta()))
					return false;

				// selectedComponents has been populated with results			
				// Filter them additionally by construct arguments				
				TAny<AUnit*> filteredSelectedComponents;
				for (auto& unit : selectedUnits) {
					bool localMismatch = false;
					auto unitBlock = unit->GetBlock();
					construct.GetAll().ForEachDeep([&](const Block& part) {
						for (pcptr i = 0; i < part.GetCount(); ++i) {
							auto element = part.GetElementResolved(i);
							auto selector = Verb::From<Verbs::Select>({}, element);
							Verb::DispatchFlat(unitBlock, selector);
							if (selector.GetOutput().IsEmpty()) {
								// Abort on first failure								
								localMismatch = true;
								return false;
							}
						}

						return true;
					});

					if (!localMismatch) {
						// The unit passes the test									
						filteredSelectedComponents << unit;
					}
				}

				// Substitute the selected units with the filtered ones		
				selectedUnits = pcMove(filteredSelectedComponents);
			}

			return false;
		};

		verb.GetArgument().ForEachDeep([&](const Block& part) {
			EitherDoThis
				part.ForEach([&](const Construct& construct) {
					return selectConstruct(construct);
				})
			OrThis
				part.ForEach([&](const Trait& trait) {
					return selectTrait(trait.GetTraitMeta());
				})
			OrThis
				part.ForEach([&](const TraitID& trait) {
					return selectTrait(trait.GetMeta());
				})
			OrThis
				part.ForEach([&](const MetaTrait* trait) {
					return selectTrait(trait);
				})
			OrThis
				part.ForEach([&](const DataID& type) {
					return selectUnit(type.GetMeta());
				})
			OrThis
				part.ForEach([&](const MetaData* type) {
					return selectUnit(type);
				});

			return !mismatch;
		});

		if (!mismatch) {
			// We're not seeking an entity, but components/traits				
			if (!selectedTraits.IsEmpty()) {
				ENTITY_SELECTION_VERBOSE_SELF(ccGreen 
					<< "Trait(s) selected: " << selectedTraits);
				verb << selectedTraits.Decay();
			}

			if (!selectedUnits.IsEmpty()) {
				ENTITY_SELECTION_VERBOSE_SELF(ccGreen 
					<< "Unit(s) selected: " << selectedUnits);
				verb << selectedUnits.Decay();
			}

			if (!selectedEntities.IsEmpty()) {
				ENTITY_SELECTION_VERBOSE_SELF(ccGreen 
					<< "Entity(s) selected: " << selectedEntities);
				verb << selectedEntities.Decay();
			}
		}

		// Climb the hierarchy															
		if (!verb.IsDone() && mOwner)
			mOwner->Select(verb);
	}

	/// Create a child entity via default-construction									
	///	@return the instance of the child												
	Entity* Entity::CreateChild() {
		auto newEntity = Ptr<Entity>::New(this);
		mChildren << newEntity;
		ENTITY_VERBOSE_SELF(newEntity << " added");
		return newEntity;
	}

	/// Create a child entity via move-construction										
	///	@param initializer - the entity to move										
	///	@return the instance of the child												
	Entity* Entity::CreateChild(Entity&& initializer) {
		auto newEntity = Ptr<Entity>::New(pcForward<Entity>(initializer));
		mChildren << newEntity;
		ENTITY_VERBOSE_SELF(newEntity << " added");
		return newEntity;
	}

	/// Create a child entity via dynamic construction									
	///	@param construct - the entity constructor										
	///	@param products - [out] created children go here							
	void Entity::CreateChild(const Construct& construct, TAny<Entity*>& products) {
		// First make the entity														
		auto newEntity = Ptr<Entity>::New(this);
		mChildren << newEntity;

		// Then 'create' inside it														
		Any childBlock { newEntity->GetBlock() };
		UNUSED() Any unusedSideproducts;
		Verb::DefaultCreateInner(childBlock, construct.GetAll(), unusedSideproducts);
		products << newEntity;

		if (unusedSideproducts.IsEmpty())
			ENTITY_VERBOSE_SELF(newEntity << " added");
		else
			ENTITY_VERBOSE_SELF(newEntity << " added, with sideproducts: " << unusedSideproducts);
	}

	/// Add a child																				
	///	@param entity - entity instance to register as a child					
	void Entity::AddChild(Entity* entity) {
		if (mChildren.Find(entity) != uiNone)
			return;

		mChildren << entity;
		entity->mOwner = this;
		entity->mRefreshRequired = true;
	}

	/// Destroy a child that matched pointer												
	///	@attention provided pointer is considered invalid after this call		
	///	@param entity - entity instance to destroy									
	void Entity::RemoveChild(Entity* entity) {
		for (auto child : mChildren) {
			if (child != entity)
				continue;

			ENTITY_VERBOSE_SELF(child << " removed");

			SAFETY(bool trulyRemoved = )
				mChildren.Remove(child);
			SAFETY(if (!trulyRemoved) throw Except::BadDestruction());
			return;
		}
	}

	/// Move an entity as a child to this one, remapping units and hierarchy	
	/// @attention - 'other' is considered invalid after the move, overwrite	
	/// with the returned value to avoid undefined behavior!							
	///	@param other - the entity to move												
	///	@return the moved entity - you should overwrite 'other' with it		
	Entity* Entity::MoveAsChild(Entity* other) {
		if (mChildren.Find(other) != uiNone) {
			// 'other' is already a child - nothing moves						
			return other;
		}

		ENTITY_VERBOSE_SELF(other << " moved in as a child");
		auto newEntity = Ptr<Entity>::New(pcMove(*other));
		mChildren << newEntity;
		newEntity->mOwner->RemoveChild(other);
		// Consider 'other' as an invalid pointer at this point				
		newEntity->mOwner = this;
		newEntity->mRefreshRequired = true;
		return newEntity;
	}

	/// Produce a unit from the entity's hierarchy by the type token				
	///	@param name - name of the unit to produce										
	///	@return the unit instance, or nullptr if none created						
	AUnit* Entity::CreateUnitFromToken(const Text& name) {
		TAny<AUnit*> products;
		CreateUnitInner(Construct(name), products);
		return products[0];
	}

	/// Create all dependencies required for the production of 'type', if such	
	/// don't yet exist the this entity's upwards hierarchy							
	/// They will be added either next to their own producers/modules, or the	
	/// CRuntime component as a fallback													
	///	@param type - the type to build dependencies for							
	///	@return the resulting dependencies (top level)								
	Any Entity::CreateDependencies(DMeta type) {
		auto meta = type->GetProducerMeta();
		if (!meta) {
			throw Except::BadConstruction(pcLogSelfError 
				<< "Can't create " << type
				<< " - type has no defined producer unit/module");
		}

		if (meta->IsChildOf<AUnit>()) {
			TAny<AUnit*> producers;

			// Producer is a unit, search for it in the hierarchy				
			//TODO collect all capable producers in the hierarchy?			
			auto producerFound = SeekUnit(SeekStyle::UpToHere, meta);
			if (producerFound)
				producers << producerFound;
			else {
				// No producer available in the hierarchy, so nest to			
				// produce producer														
				ENTITY_VERBOSE_SELF(ccDarkYellow
					<< "Required producer " << meta << " for creating " << type
					<< " not found in hierarchy - attempting to create it...");
				auto nestedProducers = CreateDependencies(meta);

				// The producer will be added either next							
				// to its own producer, or the CRuntime component				
				const Construct c {meta};
				EitherDoThis
					nestedProducers.ForEach([&](AUnit* unit) {
						unit->GetOwners()[0]->CreateUnitInner(c, producers);
					})
				OrThis
					nestedProducers.ForEach([&](AModule* module) {
						module->GetRuntime()->GetOwners()[0]->CreateUnitInner(c, producers);
					});
			}

			return producers.Decay();
		}
		else if (meta->IsChildOf<AModule>()) {
			// Producer is a module. Let's seek the runtime component,		
			// since it contains the instantiated modules						
			auto runtime = GetRuntime();
			if (!runtime) {
				// No runtime available, so create one at the root entity	
				auto root = this;
				while (root->GetOwner())
					root = root->GetOwner().Get();
				runtime = root->CreateUnit<CRuntime>();
			}

			// Runtime unit is available, poll it for the module				
			TAny<AModule*> producers;
			runtime->InstantiateModule(meta, producers);
			return producers.Decay();
		}

		throw Except::BadConstruction(pcLogSelfError
			<< "Couldn't produce " << type << " from " << meta
			<< " - the producer wasn't found, and couldn't be produced");
	}

	/// Produce unit(s) from the entity's hierarchy										
	///	@param construct - instructions for the creation of the unit(s)		
	///	@param products - [out] the resulting units go here						
	void Entity::CreateUnitInner(const Construct& construct, TAny<AUnit*>& products) {
		if (construct.Is<CRuntime>()) {
			// Produce a runtime component											
			auto system = Ptr<CRuntime>::Create();
			Any systemBlock { system->GetBlock() };
			UNUSED() Any unusedSideproducts;
			Verb::DefaultCreateInner(systemBlock, construct.GetAll(), unusedSideproducts);
			AddUnit(system);
			products << system;
			return;
		}

		if (construct.Is<CFlow>()) {
			// Produce a flow component												
			auto flow = Ptr<CFlow>::Create();
			Any flowBlock { flow->GetBlock() };
			UNUSED() Any unusedSideproducts;
			Verb::DefaultCreateInner(flowBlock, construct.GetAll(), unusedSideproducts);
			AddUnit(flow);
			products << flow;
			return;
		}

		// Find the producer																
		auto unit = construct.GetMeta();
		auto creator = Verb::From<Verbs::Create>({}, &construct);
		auto producers = CreateDependencies(unit);
		Verb::ExecuteVerb(producers, creator);

		// Call constructors for each locally produced unit					
		creator.GetOutput().ForEachDeep([&](Block& group) {
			group.ForEach([this, &products](AUnit* newUnit) {
				AddUnit(newUnit);
				products << newUnit;
			});

			// Delegation might produce more requests, so we nest those		
			group.ForEach([this, &products](const Construct& nestedConstruct) {
				CreateUnitInner(nestedConstruct, products);
			});
		});
	}
	
} // namespace PCFW
