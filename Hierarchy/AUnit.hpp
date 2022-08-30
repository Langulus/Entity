#pragma once
#include "AContext.hpp"

namespace PCFW
{

	///																								
	///	A templated producible item														
	///																								
	template<Dense T>
	class TProducedFrom {
	protected:
		Own<T*> mProducer;

	public:
		using Producer = T;

		TProducedFrom() = delete;
		TProducedFrom(T*) noexcept;
		TProducedFrom(const TProducedFrom&) noexcept = default;
		TProducedFrom(TProducedFrom&&) noexcept = default;

		TProducedFrom& operator = (const TProducedFrom&) noexcept = default;
		TProducedFrom& operator = (TProducedFrom&&) noexcept;

		PC_SET(Producer);
	};

	/// A producible type is one, that has valid GetProducer() method				
	template<class T>
	concept Producible = requires (pcDecay<T> a) { { a.GetProducer() } -> NotSame<void>; };

	template<class T>
	using ProducerOf = Conditional<Producible<T>, pcDecay<typename pcDecay<T>::Producer>, void>;

	class Entity;
	class CRuntime;

	///																								
	///	An abstract unit																		
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AUnit : public AContext, public Referenced  {
	protected:
		// Entities that are coupled with this unit								
		TAny<Entity*> mOwners;

	public:
		REFLECT(AUnit);
		friend class Entity;
		/// A unit can not be default-created, nor be shallowly copied				
		AUnit() = delete;
		AUnit(const AUnit&) = delete;
		AUnit& operator = (const AUnit&) = delete;

		/// A unit can only be moved or created with a class override				
		AUnit(DMeta) noexcept;
		AUnit(AUnit&&) noexcept;
		AUnit& operator = (AUnit&&) noexcept;
		~AUnit();
		
		PC_VERB(Select);

	public:
		virtual void Refresh() {}

		PC_SET(Owners);

		NOD() CRuntime* GetRuntime(const Index& = uiFirst);

		template<RTTI::ReflectedTrait TRAIT>
		Trait SeekTrait() const;

		template<RTTI::ReflectedData DATA>
		bool SeekValue(TMeta, DATA&) const;

		template<RTTI::ReflectedTrait TRAIT, RTTI::ReflectedData DATA>
		bool SeekValue(DATA&) const;

		bool DoInHierarchy(Verb&, SeekStyle = SeekStyle::UpToHere);
		void Couple(Entity*);
		void Decouple(Entity*);
		void ReplaceOwner(Entity*, Entity*);
	};

	/// Any type that inherits AUnit is considered a unit								
	template<class T>
	concept Unit = pcHasBase<T, AUnit>;

} // namespace PCFW

#include "AUnit.inl"
