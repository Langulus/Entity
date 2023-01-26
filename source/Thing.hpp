///                                                                           
/// Langulus::Entity                                                          
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Unit.hpp"
#include "Runtime.hpp"

LANGULUS_DEFINE_TRAIT(Runtime, "Accesses the runtime of a hierarchy of Things");
LANGULUS_DEFINE_TRAIT(Unit, "Accesses units (components) of Things");

namespace Langulus::Entity
{

   using Flow::Code;
   using UnitMap = TUnorderedMap<DMeta, TAny<Unit*>>;
   using TraitMap = TUnorderedMap<TMeta, TAny<Trait>>;

   /// A text specialization, dedicated for natural language text             
   /// It is a placeholder type, that can be extended by external modules     
   class Lingua : public Text {
      LANGULUS(ABSTRACT) true;
   };


   ///                                                                        
   ///   Thing                                                                
   ///                                                                        
   /// The primary composable type. Its functionality comes from its units    
   /// and children/owner's units. The Thing is an aggregate of traits,       
   /// units, and subthings.                                                  
   ///                                                                        
   class Thing final : public Resolvable {
      LANGULUS(ABSTRACT) false;
      LANGULUS(PRODUCER) Thing;
      LANGULUS(UNINSERTABLE) false;
      LANGULUS_BASES(Resolvable);

   protected: TESTING(public:)
      void ResetRuntime(Runtime*);
      void ResetFlow(Temporal*);

      // The entity's parent                                            
      Ptr<Thing> mOwner;
      // Hierarchy                                                      
      Hierarchy mChildren;
      // Units                                                          
      UnitMap mUnits;
      // Traits                                                         
      TraitMap mTraits;
      // Runtime                                                        
      Pinnable<Ptr<Runtime>> mRuntime;
      // Temporal flow                                                  
      Pinnable<Ptr<Temporal>> mFlow;
      // Hierarchy requires an update                                   
      bool mRefreshRequired {};

      template<SeekStyle = SeekStyle::HereAndAbove>
      NOD() Any CreateData(const Construct&);

   public:
      Thing(const Any& = {});
      Thing(Thing*, const Any& = {});
      Thing(Thing&&) noexcept;

      // Shallow copy is disabled, you should be able only to clone,    
      // move, or abandon                                               
      Thing(const Thing&) = delete;

      NOD() Runtime* GetRuntime() const noexcept;
      NOD() Temporal* GetFlow() const noexcept;

      void Do(Verb&);
      void Select(Verb&);
      void Create(Verb&);

      template<SeekStyle = SeekStyle::HereAndAbove>
      bool DoInHierarchy(Verb&);

      Any Run(const Lingua&);
      Any Run(const Code&);
      Any Run(const Scope&);

      void Update(Time);
      void Reset();

      NOD() bool operator == (const Thing&) const;

      NOD() explicit operator Debug() const;

   public:
      ///                                                                     
      ///   Hierarchy management                                              
      ///                                                                     
      Runtime*    CreateRuntime();
      Temporal*   CreateFlow();
      Thing*      CreateChild(const Any& = {});
      template<bool TWOSIDED = true>
      Count       AddChild(Thing*);
      template<bool TWOSIDED = true>
      Count       RemoveChild(Thing*);
      Module*     LoadMod(const Token&, const Any& = {});

      NOD() const Hierarchy& GetChildren() const noexcept;
      NOD()       Thing* GetChild(const Index& = IndexFirst);
      NOD() const Thing* GetChild(const Index& = IndexFirst) const;
      NOD()       Thing* GetNamedChild(const Token&, const Index& = IndexFirst);
      NOD() const Thing* GetNamedChild(const Token&, const Index& = IndexFirst) const;

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

      template<CT::Unit T, bool TWOSIDED = true>
      Count RemoveUnits();

      void ReplaceUnit(Unit*, Unit*);
      NOD() Count HasUnits(DMeta) const;
      template<CT::Unit T>
      NOD() Count HasUnits() const;

      NOD() const UnitMap& GetUnits() const noexcept;
      NOD() const Unit* GetUnit(DMeta, const Index& = IndexFirst) const;
      NOD() Unit* GetUnit(DMeta, const Index& = IndexFirst);
      template<CT::Unit T>
      NOD() Decay<T>* GetUnit(const Index& = IndexFirst);
      template<CT::Unit T>
      NOD() const Decay<T>* GetUnit(const Index& = IndexFirst) const;

      #if LANGULUS_FEATURE(MANAGED_REFLECTION)
         NOD() const Unit* GetUnit(const Token&, const Index& = IndexFirst) const;
         NOD() Unit* GetUnit(const Token&, const Index& = IndexFirst);
         template<CT::Unit T>
         NOD() Decay<T>* GetUnitAs(const Token&, const Index& = IndexFirst);
      #endif

      ///                                                                     
      ///   Trait management                                                  
      ///                                                                     
      Trait* AddTrait(const Trait&);

      Count RemoveTrait(TMeta);
      Count RemoveTrait(const Trait&);

      NOD() Count HasTraits(TMeta) const;
      NOD() Count HasTraits(const Trait&) const;

      NOD() const TraitMap& GetTraits() const noexcept;
      NOD() Trait GetTrait(TMeta, const Index& = IndexFirst) const;
      NOD() Trait GetTrait(TMeta, const Index& = IndexFirst);
      template<CT::Trait T = Trait>
      NOD() Trait GetTrait(const Index& = IndexFirst);
      NOD() Trait GetTrait(const Trait&, const Index& = IndexFirst) const;
      NOD() Trait GetTrait(const Trait&, const Index& = IndexFirst);

      NOD() const Trait* GetLocalTrait(TMeta, const Index& = IndexFirst) const;
      NOD() Trait* GetLocalTrait(TMeta, const Index& = IndexFirst);
      template<CT::Trait = Trait>
      NOD() Trait* GetLocalTrait(const Index& = IndexFirst);
      template<CT::Trait = Trait>
      NOD() const Trait* GetLocalTrait(const Index& = IndexFirst) const;

      void SetName(const Text&);
      NOD() Text GetName() const;

      ///                                                                     
      ///   Seek                                                              
      ///                                                                     
      #if LANGULUS_FEATURE(MANAGED_MEMORY)
         template<SeekStyle = SeekStyle::HereAndAbove>
         NOD() Unit* SeekUnit(const Token&, const Index& = IndexFirst);
         template<SeekStyle = SeekStyle::HereAndAbove>
         NOD() const Unit* SeekUnit(const Token&, const Index& = IndexFirst) const;
         template<SeekStyle = SeekStyle::HereAndAbove>
         NOD() const Unit* SeekUnit(const Any&, const Token&, Index = IndexFirst) const;
      #endif

      template<SeekStyle = SeekStyle::HereAndAbove>
      NOD() Unit* SeekUnit(DMeta, const Index& = IndexFirst);
      template<SeekStyle = SeekStyle::HereAndAbove>
      NOD() const Unit* SeekUnit(DMeta, const Index& = IndexFirst) const;
      template<SeekStyle = SeekStyle::HereAndAbove>
      NOD() const Unit* SeekUnit(const Any&, DMeta, const Index& = IndexFirst) const;

      template<CT::Data T, SeekStyle = SeekStyle::HereAndAbove>
      NOD() Decay<T>* SeekUnit(const Index& = IndexFirst);
      template<CT::Data T, SeekStyle = SeekStyle::HereAndAbove>
      NOD() const Decay<T>* SeekUnit(const Index& = IndexFirst) const;
      template<CT::Data T, SeekStyle = SeekStyle::HereAndAbove>
      NOD() const Decay<T>* SeekUnit(const Any&, const Index& = IndexFirst) const;

      #if LANGULUS_FEATURE(MANAGED_MEMORY)
         template<SeekStyle = SeekStyle::HereAndAbove>
         NOD() Trait SeekTrait(const Token&, const Index& = IndexFirst);
         template<SeekStyle = SeekStyle::HereAndAbove>
         NOD() Trait SeekTrait(const Token&, const Index& = IndexFirst) const;
         template<SeekStyle = SeekStyle::HereAndAbove, CT::Data D>
         NOD() bool SeekTrait(const Token&, D&, const Index& = IndexFirst) const;

         template<SeekStyle = SeekStyle::HereAndAbove>
         NOD() Trait SeekTrait(const Any&, const Token&, const Index& = IndexFirst) const;
         template<SeekStyle = SeekStyle::HereAndAbove, CT::Data D>
         NOD() bool SeekTrait(const Any&, const Token&, D&, const Index& = IndexFirst) const;
      #endif

      template<SeekStyle = SeekStyle::HereAndAbove>
      NOD() Trait SeekTrait(TMeta, const Index& = IndexFirst);
      template<SeekStyle = SeekStyle::HereAndAbove>
      NOD() Trait SeekTrait(TMeta, const Index& = IndexFirst) const;
      template<SeekStyle = SeekStyle::HereAndAbove, CT::Data D>
      NOD() bool SeekTrait(TMeta, D&, const Index& = IndexFirst) const;

      template<SeekStyle = SeekStyle::HereAndAbove>
      NOD() Trait SeekTrait(const Any&, TMeta, const Index& = IndexFirst) const;
      template<SeekStyle = SeekStyle::HereAndAbove, CT::Data D>
      NOD() bool SeekTrait(const Any&, TMeta, D&, const Index& = IndexFirst) const;

      template<CT::Trait T = Trait, SeekStyle = SeekStyle::HereAndAbove>
      NOD() Trait SeekTrait(const Index& = IndexFirst);
      template<CT::Trait T = Trait, SeekStyle = SeekStyle::HereAndAbove>
      NOD() Trait SeekTrait(const Index& = IndexFirst) const;
      template<CT::Trait T = Trait, SeekStyle = SeekStyle::HereAndAbove, CT::Data D>
      NOD() bool SeekTrait(D&, const Index& = IndexFirst) const;

      template<CT::Trait T = Trait, SeekStyle = SeekStyle::HereAndAbove>
      NOD() Trait SeekTrait(const Any&, const Index& = IndexFirst) const;
      template<CT::Trait T = Trait, SeekStyle = SeekStyle::HereAndAbove, CT::Data D>
      NOD() bool SeekTrait(const Any&, D&, const Index& = IndexFirst) const;

      template<SeekStyle = SeekStyle::HereAndAbove, CT::Data D>
      bool SeekValue(D&, const Index& = IndexFirst) const;
      template<SeekStyle = SeekStyle::HereAndAbove, CT::Data D>
      bool SeekValue(const Any&, D&, const Index& = IndexFirst) const;

      ///                                                                     
      ///   Gather                                                            
      ///                                                                     
      #if LANGULUS_FEATURE(MANAGED_MEMORY)
         template<SeekStyle = SeekStyle::HereAndAbove>
         NOD() TAny<Unit*> GatherUnits(const Token&);
         template<SeekStyle = SeekStyle::HereAndAbove>
         NOD() TAny<const Unit*> GatherUnits(const Token&) const;
      #endif 

      template<SeekStyle = SeekStyle::HereAndAbove>
      NOD() TAny<Unit*> GatherUnits(DMeta);
      template<SeekStyle = SeekStyle::HereAndAbove>
      NOD() TAny<const Unit*> GatherUnits(DMeta) const;
      template<CT::Data T = Unit, SeekStyle = SeekStyle::HereAndAbove>
      NOD() TAny<Decay<T>*> GatherUnits();
      template<CT::Data T = Unit, SeekStyle = SeekStyle::HereAndAbove>
      NOD() TAny<const Decay<T>*> GatherUnits() const;

      #if LANGULUS_FEATURE(MANAGED_MEMORY)
         template<SeekStyle = SeekStyle::HereAndAbove>
         NOD() TAny<Trait> GatherTraits(const Token&);
         template<SeekStyle = SeekStyle::HereAndAbove>
         NOD() TAny<Trait> GatherTraits(const Token&) const;
      #endif 

      template<SeekStyle = SeekStyle::HereAndAbove>
      NOD() TAny<Trait> GatherTraits(TMeta);
      template<SeekStyle = SeekStyle::HereAndAbove>
      NOD() TAny<Trait> GatherTraits(TMeta) const;
      template<CT::Trait T = Trait, SeekStyle = SeekStyle::HereAndAbove>
      NOD() TAny<Trait> GatherTraits();
      template<CT::Trait T = Trait, SeekStyle = SeekStyle::HereAndAbove>
      NOD() TAny<Trait> GatherTraits() const;

      template<SeekStyle = SeekStyle::HereAndAbove, CT::Data D>
      NOD() TAny<D> GatherValues() const;
   };

} // namespace Langulus::Entity

#include "Thing.inl"
#include "Thing-Seek.inl"
#include "Thing-Gather.inl"
#include "Unit-Seek.inl"
#include "Unit-Gather.inl"