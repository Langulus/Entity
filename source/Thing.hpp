///                                                                           
/// Langulus::Entity                                                          
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Unit.hpp"

LANGULUS_DEFINE_TRAIT(Runtime,
   "Accesses the runtime of a hierarchy of Things");
LANGULUS_DEFINE_TRAIT(Unit,
   "Accesses units (components) of Things");

namespace Langulus::Entity
{

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
      Ref<Thing> mOwner;
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

      template<Seek = Seek::HereAndAbove>
      NOD() Any CreateData(const Construct&);

   public:
      Thing(const Any& = {});
      Thing(Thing*, const Any& = {});
      Thing(Thing&&) noexcept;
      ~Thing() SAFETY_NOEXCEPT();

      // Shallow copy is disabled, you should be able only to clone,    
      // move, or abandon                                               
      Thing(const Thing&) = delete;

      NOD() Runtime*  GetRuntime() const noexcept;
      NOD() Temporal* GetFlow() const noexcept;

      void Do(Verb&);
      void Select(Verb&);
      void Create(Verb&);

      template<Seek = Seek::HereAndAbove>
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
      Ref<Thing>  CreateChild(const Any& = {});
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

      template<CT::Unit T = Unit, bool TWOSIDED = true>
      Count RemoveUnits();

      Count ReplaceUnit(Unit*, Unit*);
      NOD() Count HasUnits(DMeta) const;
      template<CT::Unit T>
      NOD() Count HasUnits() const;

      NOD() const UnitMap& GetUnits() const noexcept;
      NOD() const Unit* GetUnitMeta(DMeta, Index = IndexFirst) const;
      NOD()       Unit* GetUnitMeta(DMeta, Index = IndexFirst);
      NOD() const Unit* GetUnitExt(const Construct&, Index = IndexFirst) const;
      NOD()       Unit* GetUnitExt(const Construct&, Index = IndexFirst);

      template<CT::Unit T = Unit>
      NOD()       Decay<T>* GetUnit(Index = IndexFirst);
      template<CT::Unit T = Unit>
      NOD() const Decay<T>* GetUnit(Index = IndexFirst) const;

      #if LANGULUS_FEATURE(MANAGED_REFLECTION)
         NOD() const Unit* GetUnitMeta(const Token&, Index = IndexFirst) const;
         NOD()       Unit* GetUnitMeta(const Token&, Index = IndexFirst);
         template<CT::Unit T>
         NOD() Decay<T>*   GetUnitAs(const Token&, Index = IndexFirst);
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
      NOD()       Trait* GetLocalTrait(TMeta, const Index& = IndexFirst);
      template<CT::Trait = Trait>
      NOD()       Trait* GetLocalTrait(const Index& = IndexFirst);
      template<CT::Trait = Trait>
      NOD() const Trait* GetLocalTrait(const Index& = IndexFirst) const;

      void SetName(const Text&);
      NOD() Text GetName() const;

      ///                                                                     
      ///   Seek                                                              
      ///                                                                     
      LANGULUS_SEEK_INTERFACE();
      LANGULUS_SEEK_TOKEN_INTERFACE();

      ///                                                                     
      ///   Gather                                                            
      ///                                                                     
      LANGULUS_GATHER_INTERFACE();
      LANGULUS_GATHER_TOKEN_INTERFACE();
   };

} // namespace Langulus::Entity