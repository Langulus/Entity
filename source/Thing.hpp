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
LANGULUS_DEFINE_TRAIT(Child, "Accesses children in any kind of hierarchies");
LANGULUS_DEFINE_TRAIT(Parent, "Accesses parents in any kind of hierarchies");

namespace Langulus::Entity
{

   using Flow::Code;

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

   protected:
   TESTING(public:)
      void ResetRuntime(Runtime*);
      void ResetFlow(Temporal*);

      //Any CreateDependencies(DMeta);

      // The entity's parent                                            
      Ptr<Thing> mOwner;
      // Hierarchy                                                      
      Hierarchy mChildren;
      // Units                                                          
      TUnorderedMap<DMeta, TAny<Unit*>> mUnits;
      // Traits                                                         
      TUnorderedMap<TMeta, TAny<Trait>> mTraits;

      // Runtime                                                        
      Pinnable<Ptr<Runtime>> mRuntime;

      // Temporal flow                                                  
      Pinnable<Ptr<Temporal>> mFlow;

      // Hierarchy requires an update                                   
      bool mRefreshRequired {};

      template<SeekStyle = SeekStyle::UpToHere>
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

      template<SeekStyle = SeekStyle::UpToHere>
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

      NOD()       Thing* GetChild(const Index& = IndexFirst);
      NOD() const Thing* GetChild(const Index& = IndexFirst) const;
      NOD()       Thing* GetNamedChild(const Token&, const Index& = IndexFirst);
      NOD() const Thing* GetNamedChild(const Token&, const Index& = IndexFirst) const;

      NOD() bool IsFamilyOf(const Thing&) const;

      void DumpHierarchy() const;

      ///                                                                     
      ///   Unit management                                                   
      ///                                                                     
      template<bool TWOSIDED = true>
      Count AddUnit(Unit*);
      template<bool TWOSIDED = true>
      Count RemoveUnit(Unit*);

      template<CT::Unit T, bool TWOSIDED = true>
      Count RemoveUnits();

      void ReplaceUnit(Unit*, Unit*);
      NOD() Count HasUnits(DMeta) const;
      template<CT::Unit T>
      NOD() Count HasUnits() const;

      template<SeekStyle = SeekStyle::UpToHere>
      NOD() TAny<const Unit*> GatherUnits(DMeta) const;
      template<CT::Unit T = Unit, SeekStyle = SeekStyle::UpToHere>
      NOD() TAny<const Decay<T>*> GatherUnits() const;

      template<CT::Unit T, class ... A>
      Any CreateUnit(A&&...);

      template<SeekStyle = SeekStyle::UpToHere>
      NOD() Unit* SeekUnit(DMeta, const Index& = IndexFirst);
      template<SeekStyle = SeekStyle::UpToHere>
      NOD() const Unit* SeekUnit(DMeta, const Index& = IndexFirst) const;
      template<CT::Unit T, SeekStyle = SeekStyle::UpToHere>
      NOD() const Decay<T>* SeekUnit(const Index& = IndexFirst) const;
      template<CT::Unit T, SeekStyle = SeekStyle::UpToHere>
      NOD() Decay<T>* SeekUnit(const Index& = IndexFirst);

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

      template<SeekStyle = SeekStyle::UpToHere>
      Trait SeekTrait(TMeta, const Index& = IndexFirst) const;
      template<SeekStyle = SeekStyle::UpToHere>
      Trait SeekTrait(TMeta, const Index& = IndexFirst);
      template<CT::Trait T = Trait, SeekStyle = SeekStyle::UpToHere>
      Trait SeekTrait(const Index& = IndexFirst) const;
      template<CT::Trait T = Trait, SeekStyle = SeekStyle::UpToHere>
      Trait SeekTrait(const Index& = IndexFirst);

      Trait GetTrait(TMeta, const Index& = IndexFirst) const;
      Trait GetTrait(TMeta, const Index& = IndexFirst);
      template<CT::Trait T = Trait>
      Trait GetTrait(const Index& = IndexFirst);

      Trait GetTrait(const Trait&, const Index& = IndexFirst) const;
      Trait GetTrait(const Trait&, const Index& = IndexFirst);

      const Trait* GetLocalTrait(TMeta, const Index& = IndexFirst) const;
      Trait* GetLocalTrait(TMeta, const Index& = IndexFirst);

      template<CT::Trait>
      Trait* GetLocalTrait(const Index& = IndexFirst);
      template<CT::Trait>
      const Trait* GetLocalTrait(const Index& = IndexFirst) const;

      template<SeekStyle = SeekStyle::UpToHere, CT::Data T>
      bool SeekValue(TMeta, T&) const;
      template<CT::Trait, SeekStyle = SeekStyle::UpToHere, CT::Data D>
      bool SeekValue(D&) const;

      void SetName(const Text&);
      NOD() Text GetName() const;
   };

} // namespace Langulus::Entity

#include "Thing.inl"