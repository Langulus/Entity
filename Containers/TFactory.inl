#define VERBOSE_FACTORIES(a) //pcLogSelfVerbose << a
#define TEMPLATE() template<Referencable T, FactoryUsage USAGE>

namespace PCFW
{

	/// Default construction																	
	///	@param owner - the producer of the components inside the factory		
	TEMPLATE() TFactory<T, USAGE>::TFactory(ProducerOf<T>* owner)
		: BASE {}
		, mFactoryOwner {owner} {}

	/// Move-assignment remaps all elements to the new instance owner				
	///	@attention notice how mFactoryOwner never changes here					
	///	@param other - the factory to move												
	TEMPLATE() TFactory<T, USAGE>& TFactory<T, USAGE>::operator = (TFactory&& other) noexcept {
		// Move elements																	
		BASE::operator = (pcForward<BASE>(other));
		mValid = other.mValid;
		mFirstInvalid = other.mFirstInvalid;
		other.mValid = other.mFirstInvalid = 0;

		// Remap all elements to the new producer									
		if (!IsEmpty()) {
			for (auto& item : *this)
				item.SetProducer(mFactoryOwner);
			VERBOSE_FACTORIES("Remapping to " << mFactoryOwner);
		}
		return *this;
	}

	/// Stringify the factory																	
	TEMPLATE() TFactory<T, USAGE>::operator Debug() const {
		GASM result;
		result += DataID::Reflect<ME>()->GetToken();
		result += GASM::OpenScope;
		result += TAny<T>::GetCount();
		result += " elements";
		result += GASM::CloseScope;
		return result;
	}

	/// Reset the factory																		
	TEMPLATE() void TFactory<T, USAGE>::Reset() {
		mValid = mFirstInvalid = 0;
		BASE::Reset();
		VERBOSE_FACTORIES("Reset");
	}

	/// Reset the factory																		
	TEMPLATE() TAny<T>& TFactory<T, USAGE>::Decay() {
		return *static_cast<TAny<T>*>(this);
	}

	/// Get the first reusable slot															
	///	@return the index of the first unused slot inside factory memory		
	TEMPLATE() pcptr TFactory<T, USAGE>::GetFreeIndex() {
		// First check if a free slot is available								
		if (mValid < BASE::mCount) {
			auto i = mFirstInvalid;
			for (; i < BASE::mCount; ++i) {
				if ((*this)[i].GetReferences() == 0) {
					mFirstInvalid = i;
					return i;
				}
			}

			// If this is reached, then no invalids found						
			mFirstInvalid = BASE::mCount;
		}

		return BASE::mCount;
	}

	/// Trim invalid entries at the end of the bucket, saving memory				
	TEMPLATE() void TFactory<T, USAGE>::Trim() {
		// Check how many entries are invalid at the back						
		auto t = BASE::mCount;
		pcptr trim = 0;
		while (t > 0) {
			if ((*this)[t - 1].GetReferences() > 0)
				break;
			++trim;
			--t;
		}

		if (trim == 0)
			return;

		// Check if trimming removes everything									
		VERBOSE_FACTORIES("Trimming " << trim << " elements out of " << BASE::GetCount());
		VERBOSE_FACTORIES("Factory references before trimming: " << BASE::GetBlockReferences());
		if (trim == BASE::GetCount()) {
			Reset();
			return;
		}

		// Reallocate smaller memory. This will correct count auto			
		BASE::mCount -= trim;
		if (mFirstInvalid > BASE::mCount)
			mFirstInvalid = BASE::mCount;

		VERBOSE_FACTORIES("Factory references after trimming: " << BASE::GetBlockReferences());
	}

	/// Remove item from the bucket															
	/// Does nothing if memory is outside the factory									
	///	@param item - pointer of the item to remove 									
	///					  pointer will be invalid after removal						
	TEMPLATE() template<class TOVERRIDE>
	void TFactory<T, USAGE>::Remove(TOVERRIDE* item) {
		if (!BASE::Owns(item))
			return;

		const auto index = (pcP2N(item) - pcP2N(BASE::mRaw)) / sizeof(TOVERRIDE);
		switch (item->GetReferences()) {
		case 0:
			return;
		case 1:
			VERBOSE_FACTORIES("Removing " << item << " (with index " << index << ")");
			item->~TOVERRIDE();
			--mValid;
			if (mFirstInvalid > index)
				mFirstInvalid = index;
			Trim();
			break;
		default:
			throw Except::BadAllocation("Can't destroy element that is in use");
		}
	}

	/// Produce a default item																	
	///	@param arguments - additional arguments for TOVERRIDE constructor		
	///	@return the pointer at the emplaced item										
	TEMPLATE() template<class TOVERRIDE, class... ADDITIONAL_ARGUMENTS>
	TOVERRIDE* TFactory<T, USAGE>::Produce(ADDITIONAL_ARGUMENTS&& ... arguments) {
		static_assert(sizeof(TOVERRIDE) == sizeof(T),
			"Size mismatch");
		static_assert(pcHasBase<TOVERRIDE, T>, 
			"Type override must inherit factory's original type");
		static_assert(ConstructibleWith<TOVERRIDE, decltype(mFactoryOwner), ADDITIONAL_ARGUMENTS...>,
			"Type is not constructible with the provided arguments");

		const auto fi = GetFreeIndex();
		if (fi < BASE::mCount) {
			// The free index is in range, so no reallocation required		
			// Attempt placement															
			VERBOSE_FACTORIES("Reusing old index " << fi);
			auto memoryPoint = &BASE::template Get<TOVERRIDE>(fi);
			auto result = new (memoryPoint) TOVERRIDE {mFactoryOwner, pcForward<ADDITIONAL_ARGUMENTS>(arguments)...};
			if (fi <= mFirstInvalid)
				++mFirstInvalid;
			++mValid;
			return result;
		}

		// If this is reached, then all entries are valid						
		// Allocate one more																
		VERBOSE_FACTORIES("Allocating new index " << BASE::mCount);
		BASE::Allocate(BASE::mCount + 1);
		auto memoryPoint = &BASE::template Get<TOVERRIDE>(BASE::mCount);
		auto result = new (memoryPoint) TOVERRIDE {mFactoryOwner, pcForward<ADDITIONAL_ARGUMENTS>(arguments)...};
		++BASE::mCount;
		mFirstInvalid = mValid = BASE::mCount;
		return result;
	}

	/// Merge/insert an element inside the factory										
	///	@tparam TOVERRIDE - the type override											
	///							  (must be binary compatible with T)					
	///	@param temporary - the entity to move inside									
	///	@param additional_arguments - additional arguments for TOVERRIDE		
	///	@return the owned instance															
	TEMPLATE() template<class TOVERRIDE, class... ADDITIONAL_ARGUMENTS>
	TOVERRIDE* TFactory<T, USAGE>::Merge(TOVERRIDE&& temporary, ADDITIONAL_ARGUMENTS&&... additional_arguments) {
		if constexpr (USAGE == FactoryUsage::Unique) {
			static_assert(Comparable<TOVERRIDE>,
				"Type must be comparable in order to check for uniqueness, so "
				" implement == and != operators");

			// Check if the newly produced item matches any of the			
			// available. Define your own == and != operators for the type	
			// to control the similarity												
			for (auto& item : *this) {
				auto typed = dynamic_cast<TOVERRIDE*>(&item);
				if (&temporary == typed || !typed || typed->GetReferences() == 0 || *typed != temporary)
					continue;

				// Match found, remove the temporary, we don't need it		
				VERBOSE_FACTORIES(ccGreen << "Reused unique " << typed);
				Remove<TOVERRIDE>(&temporary);
				return typed;
			}
		}

		if (!BASE::Owns(&temporary)) {
			// Emplace a new item inside factory, if the argument is not	
			// inside factory																
			auto result = Produce<TOVERRIDE>(pcForward<ADDITIONAL_ARGUMENTS>(additional_arguments)...);

			// Move-assign the initialized instance onto the new one			
			// Everything should be remapped accordingly if your				
			// move-assignment operator is written correctly					
			*result = pcForward<TOVERRIDE>(temporary);
			VERBOSE_FACTORIES(ccYellow << "Produced " << result);
			return result;
		}

		// If this is reached, then nothing was found - the instance is	
		// already in the factory, so just return it								
		return &temporary;
	}

	/// Satisfy a creation request, discarding any sideproducts						
	///	@param arguments - construction arguments										
	///	@param additional_arguments - additional arguments for TOVERRIDE		
	///	@return the new instance															
	TEMPLATE() template<class TOVERRIDE, class... ADDITIONAL_ARGUMENTS>
	TOVERRIDE* TFactory<T, USAGE>::CreateInner(const Any& arguments, ADDITIONAL_ARGUMENTS&&... additional_arguments) {
		static_assert(sizeof(TOVERRIDE) == sizeof(T),
			"Size mismatch");
		static_assert(pcHasBase<TOVERRIDE, T>,
			"Type override must inherit factory's original type");

		// Forward any constructs to the element's creation					
		auto result = Produce<TOVERRIDE>(pcForward<ADDITIONAL_ARGUMENTS>(additional_arguments)...);
		Any sideproducts;
		if (!arguments.IsEmpty()) {
			Any resultingBlock {result->GetBlock()};
			Verb::DefaultCreateInner(resultingBlock, arguments, sideproducts);
		}

		// Insert/reuse																	
		return Merge<TOVERRIDE>(pcMove(*result), pcForward<ADDITIONAL_ARGUMENTS>(additional_arguments)...);
	}

	/// Satisfy a creation request															
	///	@param arguments - construction arguments										
	///	@param products - [out] results go here										
	///	@param additional_arguments - additional arguments for TOVERRIDE		
	TEMPLATE() template<class TOVERRIDE, class... ADDITIONAL_ARGUMENTS>
	void TFactory<T, USAGE>::CreateInner(const Any& arguments, Any& products, ADDITIONAL_ARGUMENTS&&... additional_arguments) {
		static_assert(sizeof(TOVERRIDE) == sizeof(T),
			"Size mismatch");
		static_assert(pcHasBase<TOVERRIDE, T>,
			"Type override must inherit factory's original type");

		// Forward any constructs to the element's creation					
		auto result = Produce<TOVERRIDE>(pcForward<ADDITIONAL_ARGUMENTS>(additional_arguments)...);
		Any sideproducts;
		if (!arguments.IsEmpty()) {
			Any resultingBlock {result->GetBlock() };
			Verb::DefaultCreateInner(resultingBlock, arguments, sideproducts);
		}

		// Insert/reuse																	
		result = Merge(pcMove(*result), pcForward<ADDITIONAL_ARGUMENTS>(additional_arguments)...);
		if (0 == products.SmartPush<Any>(result))
			pcLogSelfWarning << "Lost products: " << result;

		// If delegation occured, then output sideproducts, too				
		if (!sideproducts.IsEmpty() && 0 == products.SmartPush<Any>(sideproducts))
			pcLogSelfWarning << "Lost sideproducts: " << sideproducts;
	}

	/// Satisfy a construct																		
	///	@param mass - creation energy														
	///	@param construct - the construct to use										
	///	@param output - [out] gather results here										
	///	@param arguments - additional arguments for TOVERRIDE construction	
	TEMPLATE() template<class TOVERRIDE, class... ADDITIONAL_ARGUMENTS>
	void TFactory<T, USAGE>::CreateConstruct(real mass, const Construct& construct, Any& output, ADDITIONAL_ARGUMENTS&&... arguments) {
		const auto count = pcptr(construct.GetCharge().mMass * mass);
		static const auto meta = DataID::Reflect<T>();
		if (!count || !meta->InterpretsAs(construct.GetMeta()))
			return;

		if constexpr (USAGE == FactoryUsage::Unique) {
			// Each of the instances will be the same, so make one		
			CreateInner<TOVERRIDE>(construct.GetAll(), output, pcForward<ADDITIONAL_ARGUMENTS>(arguments)...);
		}
		else {
			// Satisfy all the requested count									
			for (pcptr i = 0; i < count; ++i)
				CreateInner<TOVERRIDE>(construct.GetAll(), output, arguments...);
		}
	}

	/// Satisfy a creation verb if factory type is compatible						
	///	@param verb - verb to satisfy														
	///	@param arguments - additional arguments for TOVERRIDE						
	TEMPLATE() template<class TOVERRIDE, class... ADDITIONAL_ARGUMENTS>
	void TFactory<T, USAGE>::Create(Verb& verb, ADDITIONAL_ARGUMENTS&&... arguments) {
		static_assert(sizeof(TOVERRIDE) == sizeof(T),
			"Size mismatch");
		static_assert(pcHasBase<TOVERRIDE, T>,
			"Type override must inherit factory's original type");

		if (verb.GetArgument().IsEmpty())
			return;

		// Scan the request																
		Any output;
		verb.GetArgument().ForEachDeep([&](const Block& group) {
			EitherDoThis
				group.ForEach([&](const Construct& c) {
					CreateConstruct(verb.GetMass(), c, output, arguments...);
				})
			OrThis
				group.ForEach([&](const DataID& t) {
					CreateConstruct(verb.GetMass(), Construct(t), output, arguments...);
				})
			OrThis
				group.ForEach([&](const MetaData* t) {
					CreateConstruct(verb.GetMass(), Construct(t), output, arguments...);
				});
		});

		verb << pcMove(output);
	}

	/// Satisfy a selection verb																
	///	@param verb - verb to satisfy														
	TEMPLATE() void TFactory<T, USAGE>::Select(Verb&) {
		TODO();
	}

} // namespace PCFW

#undef TEMPLATE