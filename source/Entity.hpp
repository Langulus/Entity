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

LANGULUS_DEFINE_TRAIT(Trait, "Accesses traits, dynamic or static");
LANGULUS_DEFINE_TRAIT(Runtime, "Accesses Runtime component inside a hierarchy of Entities");
LANGULUS_DEFINE_TRAIT(Unit, "Accesses components, dynamic or static");
LANGULUS_DEFINE_TRAIT(Child, "Accesses children in hierarchies");
LANGULUS_DEFINE_TRAIT(Parent, "Accesses parents in hierarchies");

namespace Langulus::Entity
{

   using Flow::Code;

   /// A text specialization, dedicated for natural language text             
   /// It is reflected as abstract, so production happens in external modules 
   class Lingua : public Text {
      LANGULUS(ABSTRACT) true;
   };


   ///                                                                        
   ///   Entity                                                               
   ///                                                                        
   ///   The primary composed type. Its functionality comes from its units    
   /// and children/owner's units. An entity is an aggregate of traits,       
   /// units, and other entities.                                             
   ///                                                                        
   class Entity final : public Resolvable {
      LANGULUS(ABSTRACT) false;
   protected:
      void ResetRuntime(Runtime*);
      void ResetFlow(Temporal*);

      Any CreateDependencies(DMeta);

      // The entity's parent                                            
      Ptr<Entity> mOwner;
      //   Hierarchy                                                    
      TAny<Entity*> mChildren;
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
      Entity(Entity* = nullptr) noexcept;
      Entity(const Entity&) = delete;
      Entity(Entity&&) noexcept;

      NOD() Runtime* GetRuntime() const noexcept;
      NOD() Temporal* GetFlow() const noexcept;

      void Do(Verb&);
      void Select(Verb&);
      void Create(Verb&);

      template<CT::Data, SeekStyle = SeekStyle::UpToHere>
      Any CreateData(const Any& = {});
      template<SeekStyle = SeekStyle::UpToHere>
      Any CreateData(const Token&, const Any& = {});
      template<SeekStyle = SeekStyle::UpToHere>
      Any CreateData(DMeta, const Any& = {});

      template<SeekStyle = SeekStyle::UpToHere>
      bool DoInHierarchy(Verb&);

      Any Run(const Lingua&);
      Any Run(const Code&);
      Any Run(const Any&);

      void Update(Time);
      void Reset();

      NOD() bool operator == (const Entity&) const;

      NOD() explicit operator Debug() const;

   public:
      ///                                                                     
      ///   HIERARCHY MANAGEMENT                                              
      ///                                                                     
      NOD() Entity* CreateChild(const Any&);
      Count DestroyChild(Entity*);

      NOD() Entity* GetChild(const Index&);
      NOD() const Entity* GetChild(const Index&) const;
      NOD() Entity* GetChild(const Token&, const Index&);
      NOD() const Entity* GetChild(const Token&, const Index&) const;

      NOD() bool IsFamilyOf(const Entity&) const;

      void DumpHierarchy() const;

      ///                                                                     
      ///   UNIT MANAGEMENT                                                   
      ///                                                                     
      Count AddUnit(Unit*);
      Count RemoveUnit(Unit*);

      template<CT::Unit T>
      Count RemoveUnits();

      void ReplaceUnit(Unit*, Unit*);
      NOD() Count HasUnits(DMeta) const;

      template<SeekStyle>
      NOD() TAny<const Unit*> GatherUnits(DMeta) const;
      template<CT::Unit T, SeekStyle>
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
      ///   TRAIT MANAGEMENT                                                  
      ///                                                                     
      Trait* AddTrait(const Trait&);
      template<CT::Trait, CT::Data D>
      Trait* AddTrait(const D&);

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

#include "Entity.inl"