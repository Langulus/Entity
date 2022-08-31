#pragma once
#include "Unit.hpp"

#define ENTITY_VERBOSE_SELF(a) //pcLogSelfVerbose << a
#define ENTITY_VERBOSE(a) //pcLog << a
#define ENTITY_CREATION_VERBOSE_SELF(a) //pcLogSelfVerbose << a
#define ENTITY_SELECTION_VERBOSE_SELF(a) //pcLogSelfVerbose << a

namespace Langulus::Entity
{

	///																								
	///	Entity																					
	///																								
	///	The primary composed type. Its functionality comes from its units		
	/// and children/owner's units. An entity is an aggregate of traits,			
	/// units, and other entities.															
	///																								
	class Entity : public Resolvable {
	protected:
		void ResetRuntime(Runtime* = nullptr);
		Any CreateDependencies(DMeta);

		// The entity's parent															
		Ptr<Entity> mOwner;
		//	Hierarchy																		
		TAny<Entity*> mChildren;
		// Units																				
		THashMap<DMeta, TAny<Unit*>> mUnits;
		// Traits																			
		THashMap<TMeta, TAny<Trait>> mTraits;
		// Runtime shortcut																
		Ptr<Runtime> mRuntime;

		// Hierarchy requires an update												
		bool mRefreshRequired = false;

	public:
		Entity(Entity* = nullptr) noexcept;
		Entity(const Entity&) = delete;
		Entity(Entity&&) noexcept;

		void Do(Verb&);
		void Select(Verb&);
		void Create(Verb&);

		template<SeekStyle SEEK = SeekStyle::UpToHere>
		bool DoInHierarchy(Verb&);

		Any RunSpeech(const Text&);
		Any RunCode(const Flow::Code&);

		void Update(Time);
		void Reset();

		NOD() bool operator == (const Entity&) const;

		NOD() explicit operator Debug() const;

	public:
		///																							
		///	UNIT MANAGEMENT																	
		///																							
		Count AddUnit(Unit*);
		Count RemoveUnit(Unit*);

		template<CT::Unit T>
		Count RemoveUnits();

		void ReplaceUnit(Unit*, Unit*);
		NOD() Count HasUnits(DMeta) const;
		NOD() Any GatherUnits(SeekStyle, DMeta) const;

		template<CT::Unit T>
		NOD() TAny<const Decay<T>*> GatherUnits(SeekStyle) const;

		void CreateUnitInner(const Construct&, TAny<Unit*>&);
		Unit* CreateUnitFromToken(const Text&);

		template<CT::Unit T, class ... A>
		Decay<T>* CreateUnit(A&&...);

		NOD() Unit* SeekUnit(SeekStyle, DMeta, const Index& = IndexFirst);
		NOD() const Unit* SeekUnit(SeekStyle, DMeta, const Index& = IndexFirst) const;

		template<CT::Unit T>
		NOD() const Decay<T>* SeekUnit(SeekStyle, const Index& = IndexFirst) const;

		template<CT::Unit T>
		NOD() Decay<T>* SeekUnit(SeekStyle, const Index& = IndexFirst);

		NOD() const Unit* GetUnit(DMeta, const Index& = IndexFirst) const;
		NOD() Unit* GetUnit(DMeta, const Index& = IndexFirst);

		template<CT::Unit T>
		NOD() Decay<T>* GetUnit(const Index& = IndexFirst);

		template<CT::Unit T>
		NOD() const Decay<T>* GetUnit(const Index& = IndexFirst) const;

		NOD() const Unit* GetUnit(const Text&, const Index& = IndexFirst) const;
		NOD() Unit* GetUnit(const Text&, const Index& = IndexFirst);

		template<CT::Unit T>
		NOD() Decay<T>* GetUnitT(const Text&, const Index& = IndexFirst);


		///																							
		///	TRAIT MANAGEMENT																	
		///																							
		Trait* AddTrait(const Trait&);

		template<CT::Trait, CT::Data D>
		Trait* AddTrait(const D&);

		void RemoveTrait(TMeta);
		void RemoveTrait(const Trait&);

		NOD() Count HasTraits(TMeta) const;
		NOD() Count HasTraits(const Trait&) const;

		bool SeekTrait(SeekStyle, TMeta, Trait&, const Index& = IndexFirst) const;
		bool SeekTrait(SeekStyle, TMeta, Trait&, const Index& = IndexFirst);

		bool GetTrait(TMeta, Trait&, const Index& = IndexFirst) const;
		bool GetTrait(TMeta, Trait&, const Index& = IndexFirst);
		bool GetTrait(const Trait&, Trait&, const Index& = IndexFirst) const;
		bool GetTrait(const Trait&, Trait&, const Index& = IndexFirst);

		Trait* GetLocalTrait(TMeta, const Index& = IndexFirst);
		const Trait* GetLocalTrait(TMeta, const Index& = IndexFirst) const;

		template<CT::Trait>
		Trait* GetLocalTrait(const Index& = IndexFirst);

		template<CT::Trait>
		const Trait* GetLocalTrait(const Index& = IndexFirst) const;

		template<CT::Data T>
		bool SeekValue(SeekStyle, TMeta, T&) const;

		template<CT::Trait, CT::Data D>
		bool SeekValue(SeekStyle, D&) const;

		NOD() Runtime* GetRuntime(const Index& = IndexFirst);

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

} // namespace Langulus::Entity

#include "Entity.inl"