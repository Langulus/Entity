#pragma once
#include "../Hierarchy/Entity.hpp"

namespace PCFW
{

	/// Different usage styles for TFactory												
	enum class FactoryUsage {
		Default,		// Default factories aggregate items upon creation		
		Unique		// Unique factories never duplicate items (a set)		
	};


	///																								
	///	A FACTORY																				
	///																								
	///	Basically a templated container used to contain, produce, but most	
	/// importantly reuse memory. The last is available only if T is a			
	/// reference-counted type. The factory is used in almost all modules.		
	///																								
	///	By specifying a FactoryUsage::Unique usage, you're essentially making
	/// a set of the produced resources, never duplicating same creations.		
	///	It is highly recommended in such cases, to make the produced item		
	/// hashable and implement a satisfyingly fast compare operators, to avoid	
	/// huge overheads.																			
	///																								
	template<Referencable T, FactoryUsage USAGE = FactoryUsage::Default>
	class TFactory : private TAny<T>, NOT_DEEP {
		static_assert(!pcIsAbstract<T>, "Can't have a factory for abstract items");
		using BASE = TAny<T>;

		REFLECT_MANUALLY(TFactory) {
			static GASM name, info;
			if (name.IsEmpty()) {
				name += "FactoryFor";
				name += DataID::Reflect<T>()->GetToken();
				name = name.StandardToken();
				info += "a factory for ";
				info += DataID::Reflect<T>()->GetToken();
				if constexpr (USAGE == FactoryUsage::Unique)
					info += " (a set)";
			}

			auto reflection = RTTI::ReflectData::From<ME>(name, info);
			//reflection.mIsDeep = true;
			reflection.template SetAbilities<ME>(
				REFLECT_CONVERSIONS(Debug));
			reflection.template SetBases<ME>(
				REFLECT_BASE(BASE));
			return reflection;
		}

	public:
		/// Factories can't be default-, move- or copy-constructed					
		/// We must guarantee that mFactoryOwner is always valid, and move is	
		/// allowed only via assignment, on a previously initialized factory		
		/// This is needed, because elements must be remapped to a new valid		
		/// owner																					
		TFactory() = delete;
		TFactory(const TFactory&) = delete;
		TFactory(TFactory&&) = delete;

		TFactory(ProducerOf<T>*);
		TFactory& operator = (TFactory&&) noexcept;

		using BASE::operator[];
		using BASE::Find;
		using BASE::begin;
		using BASE::end;
		using BASE::IsEmpty;

	public:
		using BASE::Owns;
		void Reset();
		TAny<T>& Decay();

		template<class TOVERRIDE = T, class... ADDITIONAL_ARGUMENTS>
		void CreateInner(const Any&, Any&, ADDITIONAL_ARGUMENTS&& ...);
		template<class TOVERRIDE = T, class... ADDITIONAL_ARGUMENTS>
		TOVERRIDE* CreateInner(const Any&, ADDITIONAL_ARGUMENTS&& ...);

		template<class TOVERRIDE = T, class... ADDITIONAL_ARGUMENTS>
		TOVERRIDE* Merge(TOVERRIDE&&, ADDITIONAL_ARGUMENTS&& ...);

		template<class TOVERRIDE = T, class... ADDITIONAL_ARGUMENTS>
		void Create(Verb&, ADDITIONAL_ARGUMENTS&& ...);
		void Select(Verb&);

		explicit operator Debug() const;

	private:
		template<class TOVERRIDE = T, class... ADDITIONAL_ARGUMENTS>
		void CreateConstruct(real, const Construct&, Any&, ADDITIONAL_ARGUMENTS&& ...);
		NOD() pcptr GetFreeIndex();
		template<class TOVERRIDE = T>
		void Remove(TOVERRIDE*);
		template<class TOVERRIDE = T, class... ADDITIONAL_ARGUMENTS>
		NOD() TOVERRIDE* Produce(ADDITIONAL_ARGUMENTS&& ...);
		void Trim();

		pcptr mValid = 0;
		pcptr mFirstInvalid = 0;
		ProducerOf<T>* mFactoryOwner;
	};

	template<Referencable T>
	using TFactoryUnique = TFactory<T, FactoryUsage::Unique>;

} // namespace PCFW

#include "TFactory.inl"