#pragma once
#include "AUnit.hpp"

#define ENTITY_VERBOSE_SELF(a) //pcLogSelfVerbose << a
#define ENTITY_VERBOSE(a) //pcLog << a
#define ENTITY_CREATION_VERBOSE_SELF(a) //pcLogSelfVerbose << a
#define ENTITY_SELECTION_VERBOSE_SELF(a) //pcLogSelfVerbose << a

namespace PCFW
{

	///																								
	///	ENTITY																					
	///																								
	/// An entity can be anything - physical or not. Its functionality comes	
	/// from its components (units) and children/owner components. An entity	
	/// is an aggregate of traits, units, and other entities.						
	///																								
	class LANGULUS_MODULE(FRAMEWORK) Entity : public AContext {
		void ResetRuntime(CRuntime* = nullptr);
		Any CreateDependencies(DMeta);

		// The entity's parent															
		Ptr<Entity> mOwner;
		//	Hierarchy																		
		TAny<Entity*> mChildren;
		// Units																				
		TAny<AUnit*> mUnits;
		// Traits																			
		TAny<Trait> mTraits;
		// Runtime shortcut																
		Ptr<CRuntime> mRuntime;

		// Hierarchy requires an update												
		bool mRefreshRequired = false;

	public:
		REFLECT(Entity);
		Entity(Entity* = nullptr) noexcept;
		Entity(const Entity&) = delete;
		Entity(Entity&&) noexcept;

		PC_VERB(Do);
		PC_VERB(Select);
		PC_VERB(Create);

		bool DoInHierarchy(Verb&, SeekStyle = SeekStyle::UpToHere);
		bool DoAIAD(const Text&);
		Any DoGASM(const GASM&);
		void Update(PCTime);
		void Reset();

		NOD() Text GetID() const;
		PC_GET(Owner);
		PC_GET(Children);
		PC_GET(Units);
		PC_GET(Traits);

		NOD() bool operator == (const Entity&) const;
		NOD() bool operator != (const Entity&) const;

		NOD() explicit operator Debug() const;
		NOD() explicit operator GASM() const;

		///																							
		///	UNIT MANAGEMENT																	
		///																							
		pcptr AddUnit(AUnit*);
		pcptr RemoveUnit(AUnit*);

		template<Unit T>
		pcptr RemoveUnits();

		void ReplaceUnit(AUnit*, AUnit*);
		NOD() pcptr HasUnits(DMeta) const;
		NOD() Any GatherUnits(SeekStyle, DMeta) const;

		template<Unit T>
		NOD() TAny<const pcDecay<T>*> GatherUnits(SeekStyle) const;

		void CreateUnitInner(const Construct&, TAny<AUnit*>&);
		AUnit* CreateUnitFromToken(const Text&);

		template<Unit T, class ... ARGUMENTS>
		pcDecay<T>* CreateUnit(ARGUMENTS&&...);

		NOD() AUnit* SeekUnit(SeekStyle, DMeta, const Index& = uiFirst);
		NOD() const AUnit* SeekUnit(SeekStyle, DMeta, const Index& = uiFirst) const;

		template<Unit T>
		NOD() const pcDecay<T>* SeekUnit(SeekStyle, const Index& = uiFirst) const;

		template<Unit T>
		NOD() pcDecay<T>* SeekUnit(SeekStyle, const Index& = uiFirst);

		NOD() const AUnit* GetUnit(DMeta, const Index& = uiFirst) const;
		NOD() AUnit* GetUnit(DMeta, const Index& = uiFirst);

		template<Unit T>
		NOD() pcDecay<T>* GetUnit(const Index& = uiFirst);

		template<Unit T>
		NOD() const pcDecay<T>* GetUnit(const Index& = uiFirst) const;

		NOD() const AUnit* GetUnit(const Text&, const Index& = uiFirst) const;
		NOD() AUnit* GetUnit(const Text&, const Index& = uiFirst);

		template<Unit T>
		NOD() pcDecay<T>* GetUnitT(const Text&, const Index& = uiFirst);


		///																							
		///	TRAIT MANAGEMENT																	
		///																							
		Trait* AddTrait(const Trait&);

		template<RTTI::ReflectedTrait TRAIT, RTTI::ReflectedData DATA>
		Trait* AddTrait(const DATA&);

		void RemoveTrait(TMeta);
		void RemoveTrait(const Trait&);

		NOD() pcptr HasTraits(TMeta) const;
		NOD() pcptr HasTraits(const Trait&) const;

		bool SeekTrait(SeekStyle, TMeta, Trait&, const Index& = uiFirst) const;
		bool SeekTrait(SeekStyle, TMeta, Trait&, const Index& = uiFirst);

		bool GetTrait(TMeta, Trait&, const Index& = uiFirst) const;
		bool GetTrait(TMeta, Trait&, const Index& = uiFirst);
		bool GetTrait(const Trait&, Trait&, const Index& = uiFirst) const;
		bool GetTrait(const Trait&, Trait&, const Index& = uiFirst);

		Trait* GetLocalTrait(TMeta, const Index& = uiFirst);
		const Trait* GetLocalTrait(TMeta, const Index& = uiFirst) const;

		template<RTTI::ReflectedTrait TRAIT>
		Trait* GetLocalTrait(const Index & = uiFirst);

		template<RTTI::ReflectedTrait TRAIT>
		const Trait* GetLocalTrait(const Index& = uiFirst) const;

		template<RTTI::ReflectedData T>
		bool SeekValue(SeekStyle, TMeta, T&) const;

		template<RTTI::ReflectedTrait TRAIT, RTTI::ReflectedData DATA>
		bool SeekValue(SeekStyle, DATA&) const;

		NOD() CRuntime* GetRuntime(const Index& = uiFirst);

		void SetName(const Text&);
		NOD() Text GetName() const;


		///																							
		///	HIERARCHY MANAGEMENT																
		///																							
		NOD() Entity* CreateChild();
		NOD() Entity* CreateChild(Entity&&);
		void CreateChild(const Construct&, TAny<Entity*>&);
		void AddChild(Entity*);
		void RemoveChild(Entity*);

		NOD() Entity* MoveAsChild(Entity*);

		NOD() Entity* GetChild(const Index&);
		NOD() const Entity* GetChild(const Index&) const;

		NOD() Entity* GetChild(const Text&, const Index&);
		NOD() const Entity* GetChild(const Text&, const Index&) const;

		NOD() bool IsFamilyOf(const Entity&) const;

		void DumpHierarchy() const;
	};

} // namespace PCFW

#include "Entity.inl"