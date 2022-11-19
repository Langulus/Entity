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
      void ResetRuntime(Runtime*);
      void ResetFlow(Temporal*);

      Any CreateDependencies(DMeta);

      // The entity's parent                                            
      Ptr<Thing> mOwner;
      // Hierarchy                                                      
      Hierarchy mChildren;
      // Units                                                          
      TUnorderedMap<DMeta, TAny<Unit*>> mUnits;
      // Traits                                                         
      TUnorderedMap<TMeta, TAny<Trait>> mTraits;

      // Runtime                                                        
      Ptr<Runtime> mRuntime;
      bool mOwnRuntime {};

      // Temporal flow                                                  
      Ptr<Temporal> mFlow;
      bool mOwnFlow {};

      // Hierarchy requires an update                                   
      bool mRefreshRequired {};

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

      template<CT::Data, SeekStyle = SeekStyle::UpToHere>
      Any CreateData(const Any& = {});

      #if LANGULUS_FEATURE(MANAGED_REFLECTION)
         template<SeekStyle = SeekStyle::UpToHere>
         Any CreateData(const Token&, const Any& = {});
      #endif

      template<SeekStyle = SeekStyle::UpToHere>
      Any CreateData(DMeta, const Any& = {});

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
      Runtime* CreateRuntime();
      Temporal* CreateFlow();
      Thing* CreateChild(const Any&);
      Count DestroyChild(Thing*);
      Module* LoadMod(const Token&, const Any& = {});

      NOD() Thing* GetChild(const Index&);
      NOD() const Thing* GetChild(const Index&) const;
      NOD() Thing* GetChild(const Token&, const Index&);
      NOD() const Thing* GetChild(const Token&, const Index&) const;

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

      template<SeekStyle>
      NOD() TAny<const Unit*> GatherUnits(DMeta) const;
      template<SeekStyle, CT::Unit T = Unit>
      NOD() TAny<const Decay<T>*> GatherUnits() const;

      NOD() Any CreateDataInner(const Construct&);

      template<CT::Unit T, class ... A>
      Decay<T>* CreateUnit(A&&...);

      template<SeekStyle>
      NOD() Unit* SeekUnit(DMeta, const Index& = IndexFirst);
      template<SeekStyle>
      NOD() const Unit* SeekUnit(DMeta, const Index& = IndexFirst) const;
      template<CT::Unit T, SeekStyle>
      NOD() const Decay<T>* SeekUnit(const Index& = IndexFirst) const;
      template<CT::Unit T, SeekStyle>
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

      template<SeekStyle>
      bool SeekTrait(TMeta, Trait&, const Index& = IndexFirst) const;
      template<SeekStyle>
      bool SeekTrait(TMeta, Trait&, const Index& = IndexFirst);

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

      template<SeekStyle, CT::Data T>
      bool SeekValue(TMeta, T&) const;
      template<SeekStyle, CT::Trait, CT::Data D>
      bool SeekValue(D&) const;

      void SetName(const Text&);
      NOD() Text GetName() const;
   };

} // namespace Langulus::Entity

#include "Thing.inl"