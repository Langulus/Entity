///                                                                           
/// Langulus::Things                                                          
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
#include <Langulus/Verbs/Create.hpp>
#include <Langulus/Verbs/Select.hpp>

LANGULUS_DEFINE_TRAIT(Runtime,
   "Accesses the runtime of a hierarchy of Things");
LANGULUS_DEFINE_TRAIT(Unit,
   "Accesses units (components) of Things");


namespace Langulus::Things
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
      LANGULUS(NAME) "Thing";
      LANGULUS(ABSTRACT) false;
      LANGULUS(PRODUCER) Thing;
      LANGULUS(POOL_TACTIC) RTTI::PoolTactic::Type;
      LANGULUS_BASES(Resolvable);
      LANGULUS_VERBS(Verbs::Create, Verbs::Select);

   protected:
      LANGULUS_API(THINGS) void ResetRuntime(Runtime*);
      LANGULUS_API(THINGS) void ResetFlow(Temporal*);
      LANGULUS_API(THINGS) void Teardown();

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
      Many CreateData(const Construct&);

      template<class T>
      void CreateInner(Verb&, const T&);

   public:
      LANGULUS_API(THINGS) Thing();
      LANGULUS_API(THINGS) Thing(Describe&&);
      LANGULUS_API(THINGS) Thing(Thing*, const Many& = {});
      LANGULUS_API(THINGS) Thing(Thing&&) noexcept;
      LANGULUS_API(THINGS) Thing(Cloned<Thing>&&);
      LANGULUS_API(THINGS) Thing(Abandoned<Thing>&&);
      LANGULUS_API(THINGS)~Thing();

      template<bool CREATE_FLOW = true>
      static Thing Root(CT::String auto&&...);

      // Shallow copy is disabled, you should be able only to clone,    
      // move, or abandon                                               
      Thing(const Thing&) = delete;
      auto operator = (auto) = delete;

      LANGULUS_API(THINGS)
      bool RequiresRefresh() const noexcept;

      LANGULUS_API(THINGS)
      auto GetRuntime() const noexcept -> const Pin<Ref<Runtime>>&;

      LANGULUS_API(THINGS)
      auto GetFlow() const noexcept -> const Pin<Ref<Temporal>>&;

      LANGULUS_API(THINGS) void Do(Verb&);
      LANGULUS_API(THINGS) void Select(Verb&);
      LANGULUS_API(THINGS) void Create(Verb&);

      template<Seek = Seek::HereAndAbove, CT::VerbBased V>
      V& RunIn(V&);
      template<CT::VerbBased V>
      V& Run(V&);

      LANGULUS_API(THINGS) Many Say(const Text&);
      LANGULUS_API(THINGS) Many Run(const Code&);

      LANGULUS_API(THINGS) bool Update(Time);
      LANGULUS_API(THINGS) void Refresh(bool force = false);
      LANGULUS_API(THINGS) void Reset();

      LANGULUS_API(THINGS)
      bool operator == (const Thing&) const;

      LANGULUS_API(THINGS)
      explicit operator Text() const;

   public:
      ///                                                                     
      ///   Hierarchy management                                              
      ///                                                                     
      LANGULUS_API(THINGS)
      auto CreateRuntime() -> Runtime*;

      LANGULUS_API(THINGS)
      auto CreateFlow() -> Temporal*;

      template<class...T>
      auto CreateChild(T&&...) -> Ref<Thing>;

      template<bool TWOSIDED = true>
      Count AddChild(Thing*);
      template<bool TWOSIDED = true>
      Count RemoveChild(Thing*);

      LANGULUS_API(THINGS)
      auto LoadMod(const Token&, const Many& = {}) -> A::Module*;
      LANGULUS_API(THINGS)
      auto LoadModPath(const Path&, const Many& = {}) -> A::Module*;

      LANGULUS_API(THINGS)
      auto GetOwner() const noexcept -> const Ref<Thing>&;

      LANGULUS_API(THINGS)
      auto GetChildren() const noexcept -> const Hierarchy&;

      LANGULUS_API(THINGS)
      auto GetChild(Index = 0) -> Thing*;

      LANGULUS_API(THINGS)
      auto GetChild(Index = 0) const -> const Thing*;

      LANGULUS_API(THINGS)
      auto GetNamedChild(const Token&, Index = 0) -> Thing*;

      LANGULUS_API(THINGS)
      auto GetNamedChild(const Token&, Index = 0) const -> const Thing*;

      LANGULUS_API(THINGS)
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

      LANGULUS_API(THINGS)
      auto HasUnits(DMeta) const -> Count;
      template<CT::Unit>
      auto HasUnits() const -> Count;

      LANGULUS_API(THINGS)
      auto GetUnits() const noexcept -> const UnitList&;
      LANGULUS_API(THINGS)
      auto GetUnitsMap() const noexcept -> const UnitMap&;

      LANGULUS_API(THINGS)
      auto GetUnitMeta(DMeta, Index = 0)       -> A::Unit*;
      LANGULUS_API(THINGS)
      auto GetUnitMeta(DMeta, Index = 0) const -> A::Unit const*;

      LANGULUS_API(THINGS)
      auto GetUnitExt(DMeta, const Many&, Index = 0)       -> A::Unit*;
      LANGULUS_API(THINGS)
      auto GetUnitExt(DMeta, const Many&, Index = 0) const -> A::Unit const*;

      template<CT::Unit T = A::Unit>
      auto GetUnit(Index = 0)       -> Decay<T>*;
      template<CT::Unit T = A::Unit>
      auto GetUnit(Index = 0) const -> Decay<T> const*;

      #if LANGULUS_FEATURE(MANAGED_REFLECTION)
         LANGULUS_API(THINGS)
         auto GetUnitMeta(const Token&, Index = 0) const -> A::Unit const*;
         LANGULUS_API(THINGS)
         auto GetUnitMeta(const Token&, Index = 0)       -> A::Unit*;

         template<CT::Unit T>
         auto GetUnitAs(const Token&, Index = 0) -> Decay<T>*;
      #endif

   private:
      LANGULUS_API(THINGS) void AddUnitBases(A::Unit*, DMeta);
      LANGULUS_API(THINGS) void RemoveUnitBases(A::Unit*, DMeta);

   public:
      ///                                                                     
      ///   Trait management                                                  
      ///                                                                     
      LANGULUS_API(THINGS) auto AddTrait(Trait) -> Trait*;

      LANGULUS_API(THINGS) Count RemoveTrait(TMeta);
      LANGULUS_API(THINGS) Count RemoveTrait(Trait);

      LANGULUS_API(THINGS)
      Count HasTraits(TMeta) const;
      LANGULUS_API(THINGS)
      Count HasTraits(const Trait&) const;

      LANGULUS_API(THINGS)
      auto GetTraits() const noexcept -> const TraitMap&;
      LANGULUS_API(THINGS)
      auto GetTrait(TMeta, Index = 0) const -> Trait;
      LANGULUS_API(THINGS)
      auto GetTrait(TMeta, Index = 0)       -> Trait;
      LANGULUS_API(THINGS)
      auto GetTrait(const Trait&, Index = 0) const -> Trait;
      LANGULUS_API(THINGS)
      auto GetTrait(const Trait&, Index = 0)       -> Trait;
      template<CT::TraitBased = Trait>
      auto GetTrait(Index = 0) -> Trait;

      LANGULUS_API(THINGS)
      auto GetLocalTrait(TMeta, Index = 0) const -> Trait const*;
      LANGULUS_API(THINGS)
      auto GetLocalTrait(TMeta, Index = 0)       -> Trait*;
      template<CT::TraitBased = Trait>
      auto GetLocalTrait(Index = 0)       -> Trait*;
      template<CT::TraitBased = Trait>
      auto GetLocalTrait(Index = 0) const -> Trait const*;

      LANGULUS_API(THINGS)
      void SetName(const Text&);

      LANGULUS_API(THINGS)
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

      template<Seek = Seek::HereAndAbove>
      auto SeekUnit(DMeta, Index = 0) -> A::Unit*;
      template<Seek = Seek::HereAndAbove>
      auto SeekUnitAux(const Many&, DMeta, Index = 0) -> A::Unit*;
      template<Seek = Seek::HereAndAbove>
      auto SeekUnitExt(DMeta, const Many&, Index = 0) -> A::Unit*;
      template<Seek = Seek::HereAndAbove>
      auto SeekUnitAuxExt(DMeta, const Many&, const Many&, Index = 0) -> A::Unit*;

      template<Seek = Seek::HereAndAbove>
      auto SeekTrait(TMeta, Index = 0) -> Trait;
      template<Seek = Seek::HereAndAbove>
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

} // namespace Langulus::Things