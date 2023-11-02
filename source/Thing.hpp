///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Unit.hpp"
#include "Pin.hpp"
#include <Anyness/Any.hpp>
#include <Anyness/Ref.hpp>
#include <Anyness/TUnorderedSet.hpp>
#include <Anyness/TUnorderedMap.hpp>
#include <Flow/Temporal.hpp>
#include <Flow/Verbs/Create.hpp>
#include <Flow/Verbs/Select.hpp>

LANGULUS_DEFINE_TRAIT(Runtime,
   "Accesses the runtime of a hierarchy of Things");
LANGULUS_DEFINE_TRAIT(Unit,
   "Accesses units (components) of Things");


namespace Langulus::Entity
{

   using UnitList = TAny<Ref<Unit>>;
   using UnitMap = TUnorderedMap<DMeta, TUnorderedSet<Ref<Unit>>>;
   using TraitMap = TUnorderedMap<TMeta, TraitList>;

   /// A text specialization, dedicated for natural language text             
   /// It is a placeholder type, that can be extended by external modules     
   struct Lingua : Text {
      LANGULUS(ABSTRACT) true;
   };


   ///                                                                        
   ///   Thing                                                                
   ///                                                                        
   /// The primary composable type. Its functionality comes from its units    
   /// and children/owner's units. The Thing is an aggregate of traits,       
   /// units, and subthings.                                                  
   ///                                                                        
   class Thing final : public Resolvable, public SeekInterface<Thing> {
      LANGULUS(ABSTRACT) false;
      LANGULUS(PRODUCER) Thing;
      LANGULUS(UNINSERTABLE) false;
      LANGULUS_BASES(Resolvable);
      LANGULUS_VERBS(Verbs::Create, Verbs::Select);

   protected: IF_LANGULUS_TESTING(public:)
      LANGULUS_API(ENTITY) void ResetRuntime(Runtime*);
      LANGULUS_API(ENTITY) void ResetFlow(Temporal*);

      // The order of members is critical!                              
      // Runtime should be destroyed last, hence it is the first member 

      // Runtime                                                        
      Pin<Ptr<Runtime>> mRuntime;
      // Temporal flow                                                  
      Pin<Ptr<Temporal>> mFlow;
      // Hierarchy                                                      
      Hierarchy mChildren;
      // Units indexed by all their relevant reflected bases            
      UnitMap mUnitsAmbiguous;
      // Units indexed by concrete type, in order of addition           
      UnitList mUnitsList;
      // Traits                                                         
      TraitMap mTraits;
      // Hierarchy requires an update                                   
      bool mRefreshRequired {};
      // The entity's parent                                            
      Ref<Thing> mOwner;

      template<Seek = Seek::HereAndAbove>
      NOD() Any CreateData(const Construct&);

      template<class T>
      void CreateInner(Verb&, const T&);

   public:
      LANGULUS_API(ENTITY) Thing();
      LANGULUS_API(ENTITY) Thing(Describe&&);
      LANGULUS_API(ENTITY) Thing(Thing*, const Neat& = {});
      LANGULUS_API(ENTITY) Thing(Thing&&) noexcept;
      LANGULUS_API(ENTITY) ~Thing() IF_UNSAFE(noexcept);

      // Shallow copy is disabled, you should be able only to clone,    
      // move, or abandon                                               
      Thing(const Thing&) = delete;

      NOD() LANGULUS_API(ENTITY)
      Runtime*  GetRuntime() const noexcept;

      NOD() LANGULUS_API(ENTITY)
      Temporal* GetFlow() const noexcept;

      LANGULUS_API(ENTITY) void Do(Verb&);
      LANGULUS_API(ENTITY) void Select(Verb&);
      LANGULUS_API(ENTITY) void Create(Verb&);

      template<Seek = Seek::HereAndAbove>
      Any RunIn(Verb&);

      LANGULUS_API(ENTITY) Any Run(const Lingua&);

      LANGULUS_API(ENTITY) bool Update(Time);
      LANGULUS_API(ENTITY) void Refresh(bool force = false);
      LANGULUS_API(ENTITY) void Reset();

      NOD() LANGULUS_API(ENTITY)
      bool operator == (const Thing&) const;

      NOD() LANGULUS_API(ENTITY)
      explicit operator Debug() const;

   public:
      ///                                                                     
      ///   Hierarchy management                                              
      ///                                                                     
      LANGULUS_API(ENTITY)
      Runtime* CreateRuntime();

      LANGULUS_API(ENTITY)
      Temporal* CreateFlow();

      LANGULUS_API(ENTITY)
      Ref<Thing> CreateChild(const Neat& = {});

      template<bool TWOSIDED = true>
      Count AddChild(Thing*);
      template<bool TWOSIDED = true>
      Count RemoveChild(Thing*);

      LANGULUS_API(ENTITY)
      Module* LoadMod(const Token&, const Neat& = {});

      NOD() LANGULUS_API(ENTITY)
      const Hierarchy& GetChildren() const noexcept;

      NOD() LANGULUS_API(ENTITY)
      Thing* GetChild(const Index& = IndexFirst);

      NOD() LANGULUS_API(ENTITY)
      const Thing* GetChild(const Index& = IndexFirst) const;

      NOD() LANGULUS_API(ENTITY)
      Thing* GetNamedChild(const Token&, const Index& = IndexFirst);

      NOD() LANGULUS_API(ENTITY)
      const Thing* GetNamedChild(const Token&, const Index& = IndexFirst) const;

      LANGULUS_API(ENTITY)
      void DumpHierarchy() const;

      ///                                                                     
      ///   Unit management                                                   
      ///                                                                     
      template<bool TWOSIDED = true>
      Count AddUnit(Unit*);
      template<bool TWOSIDED = true>
      Count RemoveUnit(Unit*);

      template<CT::Unit T, class... A>
      Any CreateUnit(A&&...);

      #if LANGULUS_FEATURE(MANAGED_REFLECTION)
         template<class... A>
         Any CreateUnitToken(const Token&, A&&...);
      #endif

      template<CT::Unit T = Unit, bool TWOSIDED = true>
      Count RemoveUnits();

      LANGULUS_API(ENTITY)
      Count ReplaceUnit(Unit*, Unit*);

      NOD() LANGULUS_API(ENTITY)
      Count HasUnits(DMeta) const;

      template<CT::Unit T>
      NOD() Count HasUnits() const;

      NOD() LANGULUS_API(ENTITY)
      const UnitList& GetUnits() const noexcept;

      NOD() LANGULUS_API(ENTITY)
            Unit* GetUnitMeta(DMeta, Index = IndexFirst);
      NOD() LANGULUS_API(ENTITY)
      const Unit* GetUnitMeta(DMeta, Index = IndexFirst) const;

      NOD() LANGULUS_API(ENTITY)
            Unit* GetUnitExt(DMeta, const Neat&, Index = IndexFirst);
      NOD() LANGULUS_API(ENTITY)
      const Unit* GetUnitExt(DMeta, const Neat&, Index = IndexFirst) const;

      template<CT::Unit T = Unit>
      NOD()       Decay<T>* GetUnit(Index = IndexFirst);
      template<CT::Unit T = Unit>
      NOD() const Decay<T>* GetUnit(Index = IndexFirst) const;

      #if LANGULUS_FEATURE(MANAGED_REFLECTION)
         NOD() LANGULUS_API(ENTITY)
         const Unit* GetUnitMeta(const Token&, Index = IndexFirst) const;

         NOD() LANGULUS_API(ENTITY)
         Unit* GetUnitMeta(const Token&, Index = IndexFirst);

         template<CT::Unit T>
         NOD() Decay<T>* GetUnitAs(const Token&, Index = IndexFirst);
      #endif

   private:
      LANGULUS_API(ENTITY) void AddUnitBases(Unit*, DMeta);
      LANGULUS_API(ENTITY) void RemoveUnitBases(Unit*, DMeta);

   public:
      ///                                                                     
      ///   Trait management                                                  
      ///                                                                     
      LANGULUS_API(ENTITY) Trait* AddTrait(const Trait&);

      LANGULUS_API(ENTITY) Count RemoveTrait(TMeta);
      LANGULUS_API(ENTITY) Count RemoveTrait(const Trait&);

      NOD() LANGULUS_API(ENTITY)
      Count HasTraits(TMeta) const;

      NOD() LANGULUS_API(ENTITY)
      Count HasTraits(const Trait&) const;

      NOD() LANGULUS_API(ENTITY)
      const TraitMap& GetTraits() const noexcept;

      NOD() LANGULUS_API(ENTITY)
      Trait GetTrait(TMeta, const Index& = IndexFirst) const;

      NOD() LANGULUS_API(ENTITY)
      Trait GetTrait(TMeta, const Index& = IndexFirst);

      NOD() LANGULUS_API(ENTITY)
      Trait GetTrait(const Trait&, const Index& = IndexFirst) const;

      NOD() LANGULUS_API(ENTITY)
      Trait GetTrait(const Trait&, const Index& = IndexFirst);

      template<CT::Trait T = Trait>
      NOD() Trait GetTrait(const Index & = IndexFirst);

      NOD() LANGULUS_API(ENTITY)
      const Trait* GetLocalTrait(TMeta, const Index& = IndexFirst) const;

      NOD() LANGULUS_API(ENTITY)
      Trait* GetLocalTrait(TMeta, const Index& = IndexFirst);

      template<CT::Trait = Trait>
      NOD()       Trait* GetLocalTrait(const Index& = IndexFirst);
      template<CT::Trait = Trait>
      NOD() const Trait* GetLocalTrait(const Index& = IndexFirst) const;

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

      template<Seek = Seek::HereAndAbove>
      NOD() Unit* SeekUnit(DMeta, Index = IndexFirst);
      template<Seek = Seek::HereAndAbove>
      NOD() Unit* SeekUnitAux(const Neat&, DMeta, Index = IndexFirst);
      template<Seek = Seek::HereAndAbove>
      NOD() Unit* SeekUnitExt(DMeta, const Neat&, Index = IndexFirst);
      template<Seek = Seek::HereAndAbove>
      NOD() Unit* SeekUnitAuxExt(DMeta, const Neat&, const Neat&, Index = IndexFirst);

      template<Seek = Seek::HereAndAbove>
      NOD() Trait SeekTrait(TMeta, Index = IndexFirst);
      template<Seek = Seek::HereAndAbove>
      NOD() Trait SeekTraitAux(const Neat&, TMeta, Index = IndexFirst);

      template<Seek = Seek::HereAndAbove>
      bool SeekValue(TMeta, CT::Data auto&, Index = IndexFirst) const;
      template<Seek = Seek::HereAndAbove>
      bool SeekValueAux(TMeta, const Neat&, CT::Data auto&, Index = IndexFirst) const;

      ///                                                                     
      ///   Gather                                                            
      ///                                                                     
      using SeekInterface::GatherUnits;
      using SeekInterface::GatherUnitsExt;
      using SeekInterface::GatherTraits;

      template<Seek = Seek::HereAndAbove>
      NOD() TAny<Unit*> GatherUnits(DMeta);
      template<Seek = Seek::HereAndAbove>
      NOD() TAny<Unit*> GatherUnitsExt(DMeta, const Neat&);

      template<Seek = Seek::HereAndAbove>
      NOD() TraitList GatherTraits(TMeta);

      template<CT::Data D, Seek = Seek::HereAndAbove>
      NOD() TAny<D> GatherValues() const;
   };

} // namespace Langulus::Entity