///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Pin.hpp"
#include "Runtime.hpp"
#include "Hierarchy.hpp"
#include "Unit.hpp"
#include <Flow/Verbs/Create.hpp>
#include <Flow/Verbs/Select.hpp>

LANGULUS_DEFINE_TRAIT(Runtime,
   "Accesses the runtime of a hierarchy of Things");
LANGULUS_DEFINE_TRAIT(Unit,
   "Accesses units (components) of Things");


namespace Langulus::Entity
{

   using UnitList = TMany<A::Unit*>;
   using UnitMap = TUnorderedMap<DMeta, TMany<A::Unit*>>;
   using TraitMap = TUnorderedMap<TMeta, TraitList>;


   ///                                                                        
   ///   Thing                                                                
   ///                                                                        
   /// The primary composable type. Its functionality comes from its units    
   /// and children/owner's units. The Thing is an aggregate of traits,       
   /// units, and subthings.                                                  
   ///                                                                        
   class Thing final
      : public Resolvable
      , public Referenced
      , public SeekInterface<Thing>
   {
      LANGULUS(ABSTRACT) false;
      LANGULUS(PRODUCER) Thing;
      LANGULUS(POOL_TACTIC) RTTI::PoolTactic::Type;
      LANGULUS_BASES(Resolvable);
      LANGULUS_VERBS(Verbs::Create, Verbs::Select);

   protected:
      LANGULUS_API(ENTITY) void ResetRuntime(Runtime*);
      LANGULUS_API(ENTITY) void ResetFlow(Temporal*);
      LANGULUS_API(ENTITY) void Teardown();

      // The order of members is critical!                              
      // Runtime should be destroyed last, hence it is the first member 
      Pin<Ref<Runtime>> mRuntime;
      // Temporal flow                                                  
      Pin<Ref<Temporal>> mFlow;
      // Hierarchy                                                      
      Hierarchy mChildren;
      // Units indexed by concrete type, in order of addition           
      UnitList mUnitsList;
      // Units indexed by all their relevant reflected bases            
      UnitMap mUnitsAmbiguous;
      // Traits                                                         
      TraitMap mTraits;
      // Hierarchy requires an update                                   
      bool mRefreshRequired {};
      // The entity's parent                                            
      Ref<Thing> mOwner;

      template<Seek = Seek::HereAndAbove>
      NOD() Many CreateData(const Construct&);

      template<class T>
      void CreateInner(Verb&, const T&);

   public:
      LANGULUS_API(ENTITY) Thing();
      LANGULUS_API(ENTITY) Thing(Describe&&);
      LANGULUS_API(ENTITY) Thing(Thing*, const Many& = {});
      LANGULUS_API(ENTITY) Thing(Thing&&) noexcept;
      LANGULUS_API(ENTITY) Thing(Cloned<Thing>&&);
      LANGULUS_API(ENTITY) Thing(Abandoned<Thing>&&);
      LANGULUS_API(ENTITY)~Thing();

      template<bool CREATE_FLOW = true>
      NOD() static Thing Root(CT::String auto&&...);

      // Shallow copy is disabled, you should be able only to clone,    
      // move, or abandon                                               
      Thing(const Thing&) = delete;
      auto operator = (auto) = delete;

      NOD() LANGULUS_API(ENTITY)
      bool RequiresRefresh() const noexcept;

      NOD() LANGULUS_API(ENTITY)
      auto GetRuntime() const noexcept -> const Pin<Ref<Runtime>>&;

      NOD() LANGULUS_API(ENTITY)
      auto GetFlow() const noexcept -> const Pin<Ref<Temporal>>&;

      LANGULUS_API(ENTITY) void Do(Verb&);
      LANGULUS_API(ENTITY) void Select(Verb&);
      LANGULUS_API(ENTITY) void Create(Verb&);

      template<Seek = Seek::HereAndAbove, CT::VerbBased V>
      V& RunIn(V&);
      template<CT::VerbBased V>
      V& Run(V&);

      LANGULUS_API(ENTITY) Many Say(const Text&);
      LANGULUS_API(ENTITY) Many Run(const Code&);

      LANGULUS_API(ENTITY) bool Update(Time);
      LANGULUS_API(ENTITY) void Refresh(bool force = false);
      LANGULUS_API(ENTITY) void Reset();

      NOD() LANGULUS_API(ENTITY)
      bool operator == (const Thing&) const;

      NOD() LANGULUS_API(ENTITY)
      explicit operator Text() const;

   public:
      ///                                                                     
      ///   Hierarchy management                                              
      ///                                                                     
      LANGULUS_API(ENTITY)
      auto CreateRuntime() -> Runtime*;

      LANGULUS_API(ENTITY)
      auto CreateFlow() -> Temporal*;

      template<class...T>
      auto CreateChild(T&&...) -> Ref<Thing>;

      template<bool TWOSIDED = true>
      Count AddChild(Thing*);
      template<bool TWOSIDED = true>
      Count RemoveChild(Thing*);

      LANGULUS_API(ENTITY)
      auto LoadMod(const Token&, const Many& = {}) -> A::Module*;

      NOD() LANGULUS_API(ENTITY)
      auto GetOwner() const noexcept -> const Ref<Thing>&;

      NOD() LANGULUS_API(ENTITY)
      auto GetChildren() const noexcept -> const Hierarchy&;

      NOD() LANGULUS_API(ENTITY)
      auto GetChild(Index = 0) -> Thing*;

      NOD() LANGULUS_API(ENTITY)
      auto GetChild(Index = 0) const -> const Thing*;

      NOD() LANGULUS_API(ENTITY)
      auto GetNamedChild(const Token&, Index = 0) -> Thing*;

      NOD() LANGULUS_API(ENTITY)
      auto GetNamedChild(const Token&, Index = 0) const -> const Thing*;

      LANGULUS_API(ENTITY)
      void DumpHierarchy() const;

   public:
      ///                                                                     
      ///   Unit management                                                   
      ///                                                                     
      template<bool TWOSIDED = true>
      Count AddUnit(A::Unit*);
      template<bool TWOSIDED = true>
      Count RemoveUnit(A::Unit*);

      template<CT::Unit, class...A>
      Many CreateUnit(A&&...);
      template<CT::Unit...>
      Many CreateUnits();

      #if LANGULUS_FEATURE(MANAGED_REFLECTION)
         template<class...A>
         Many CreateUnitToken(const Token&, A&&...);
      #endif

      template<CT::Unit = A::Unit, bool TWOSIDED = true>
      Count RemoveUnits();

      NOD() LANGULUS_API(ENTITY)
      auto HasUnits(DMeta) const -> Count;
      template<CT::Unit> NOD()
      auto HasUnits() const -> Count;

      NOD() LANGULUS_API(ENTITY)
      auto GetUnits() const noexcept -> const UnitList&;
      NOD() LANGULUS_API(ENTITY)
      auto GetUnitsMap() const noexcept -> const UnitMap&;

      NOD() LANGULUS_API(ENTITY)
      auto GetUnitMeta(DMeta, Index = 0)       ->       A::Unit*;
      NOD() LANGULUS_API(ENTITY)
      auto GetUnitMeta(DMeta, Index = 0) const -> const A::Unit*;

      NOD() LANGULUS_API(ENTITY)
      auto GetUnitExt(DMeta, const Many&, Index = 0)       ->       A::Unit*;
      NOD() LANGULUS_API(ENTITY)
      auto GetUnitExt(DMeta, const Many&, Index = 0) const -> const A::Unit*;

      template<CT::Unit T = A::Unit> NOD()
      auto GetUnit(Index = 0) -> Decay<T>*;
      template<CT::Unit T = A::Unit> NOD()
      auto GetUnit(Index = 0) const -> const Decay<T>*;

      #if LANGULUS_FEATURE(MANAGED_REFLECTION)
         NOD() LANGULUS_API(ENTITY)
         auto GetUnitMeta(const Token&, Index = 0) const -> A::Unit const*;
         NOD() LANGULUS_API(ENTITY)
         auto GetUnitMeta(const Token&, Index = 0) -> A::Unit*;

         template<CT::Unit T> NOD()
         auto GetUnitAs(const Token&, Index = 0) -> Decay<T>*;
      #endif

   private:
      LANGULUS_API(ENTITY) void AddUnitBases(A::Unit*, DMeta);
      LANGULUS_API(ENTITY) void RemoveUnitBases(A::Unit*, DMeta);

   public:
      ///                                                                     
      ///   Trait management                                                  
      ///                                                                     
      LANGULUS_API(ENTITY) auto AddTrait(Trait) -> Trait*;

      LANGULUS_API(ENTITY) Count RemoveTrait(TMeta);
      LANGULUS_API(ENTITY) Count RemoveTrait(Trait);

      NOD() LANGULUS_API(ENTITY)
      Count HasTraits(TMeta) const;
      NOD() LANGULUS_API(ENTITY)
      Count HasTraits(const Trait&) const;

      NOD() LANGULUS_API(ENTITY)
      auto GetTraits() const noexcept -> const TraitMap&;
      NOD() LANGULUS_API(ENTITY)
      auto GetTrait(TMeta, Index = 0) const -> Trait;
      NOD() LANGULUS_API(ENTITY)
      auto GetTrait(TMeta, Index = 0) -> Trait;
      NOD() LANGULUS_API(ENTITY)
      auto GetTrait(const Trait&, Index = 0) const -> Trait;
      NOD() LANGULUS_API(ENTITY)
      auto GetTrait(const Trait&, Index = 0) -> Trait;
      template<CT::TraitBased = Trait> NOD()
      auto GetTrait(Index = 0) -> Trait;

      NOD() LANGULUS_API(ENTITY)
      auto GetLocalTrait(TMeta, Index = 0) const -> const Trait*;
      NOD() LANGULUS_API(ENTITY)
      auto GetLocalTrait(TMeta, Index = 0) -> Trait*;
      template<CT::TraitBased = Trait> NOD()
      auto GetLocalTrait(Index = 0) -> Trait*;
      template<CT::TraitBased = Trait> NOD()
      auto GetLocalTrait(Index = 0) const -> Trait const*;

      LANGULUS_API(ENTITY)
      void SetName(const Text&);

      NOD() LANGULUS_API(ENTITY)
      Text GetName() const;

      ///                                                                     
      ///   Seek                                                              
      ///                                                                     
      using SeekInterface::SeekUnit;
      using SeekInterface::SeekUnitAux;
      using SeekInterface::SeekUnitExt;
      using SeekInterface::SeekUnitAuxExt;
      using SeekInterface::SeekTrait;
      using SeekInterface::SeekTraitAux;
      using SeekInterface::SeekValue;
      using SeekInterface::SeekValueAux;

      template<Seek = Seek::HereAndAbove> NOD()
      auto SeekUnit(DMeta, Index = 0) -> A::Unit*;
      template<Seek = Seek::HereAndAbove> NOD()
      auto SeekUnitAux(const Many&, DMeta, Index = 0) -> A::Unit*;
      template<Seek = Seek::HereAndAbove> NOD()
      auto SeekUnitExt(DMeta, const Many&, Index = 0) -> A::Unit*;
      template<Seek = Seek::HereAndAbove> NOD()
      auto SeekUnitAuxExt(DMeta, const Many&, const Many&, Index = 0) -> A::Unit*;

      template<Seek = Seek::HereAndAbove> NOD()
      auto SeekTrait(TMeta, Index = 0) -> Trait;
      template<Seek = Seek::HereAndAbove> NOD()
      auto SeekTraitAux(const Many&, TMeta, Index = 0) -> Trait;

      template<Seek = Seek::HereAndAbove>
      bool SeekValue(TMeta, CT::Data auto&, Index = 0) const;
      template<Seek = Seek::HereAndAbove>
      bool SeekValueAux(TMeta, const Many&, CT::Data auto&, Index = 0) const;

      ///                                                                     
      ///   Gather                                                            
      ///                                                                     
      using SeekInterface::GatherUnits;
      using SeekInterface::GatherUnitsExt;
      using SeekInterface::GatherTraits;

      template<Seek = Seek::HereAndAbove>
      auto GatherUnits(DMeta) -> TMany<A::Unit*>;
      template<Seek = Seek::HereAndAbove>
      auto GatherUnitsExt(DMeta, const Many&) -> TMany<A::Unit*>;

      template<Seek = Seek::HereAndAbove>
      auto GatherTraits(TMeta) -> TraitList;

      template<CT::Data D, Seek = Seek::HereAndAbove>
      auto GatherValues() const -> TMany<D>;
   };

} // namespace Langulus::Entity