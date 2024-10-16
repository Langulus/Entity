///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Thing.hpp"
#include "Thing.inl"
#include <Flow/Verbs/Conjunct.hpp>


namespace Langulus::Entity
{

   /// Interpret and execute a natural message wherever possible in the       
   /// current context                                                        
   ///   @param text - text to execute                                        
   ///   @return the results of the execution                                 
   Many Thing::Say(const Text& text) {
      if (not text)
         return {};

      // Push and execute in the active flow                            
      auto flow = GetFlow();
      if (not flow) {
         Logger::Error(this, ": No flow - can't execute message: ", text);
         return {};
      }

      return flow->Push(Verbs::Do::In(this, text));
   }
   
   /// Executes a piece of code in the current context                        
   ///   @param code - code to execute                                        
   ///   @return the results of the execution, if any                         
   Many Thing::Run(const Code& code) {
      if (not code)
         return {};

      // Parse the code                                                 
      auto parsed = code.Parse();
      if (not parsed)
         return {};

      // Push and execute in the active flow                            
      auto flow = GetFlow();
      if (not flow) {
         Logger::Error(this, ": No flow - can't Run code: ", code);
         return {};
      }

      return flow->Push(Verbs::Do::In(this, Abandon(parsed)));
   }

   /// Custom dispatcher, reflected via CT::Dispatcher                        
   /// Entity is a composite type, and its behavior depends on the coupled    
   /// units, so the custom dispatcher forwards verbs to these units          
   ///   @param verb - any verb                                               
   void Thing::Do(Verb& verb) {
      Run(verb);
   }

   /// Create/Destroy stuff inside entity's context                           
   ///   @param verb - creation verb                                          
   void Thing::Create(Verb& verb) {
      if (not verb)
         return;
      CreateInner(verb, verb.GetArgument());
   }

   /// Pick something from the entity - children, traits, units, modules      
   /// If this entity doesn't satisfy the query, a search will be performed   
   /// in all parents, climbing the hierarchy                                 
   ///   @param verb - the selection verb                                     
   void Thing::Select(Verb& verb) {
      // Probe every part of the argument and check if it matches       
      TMany<Trait>    selectedTraits;
      TMany<A::Unit*> selectedUnits;
      TMany<Thing*>   selectedEntities;
      bool mismatch = false;

      const auto selectTrait = [&](const TMeta& trait) {
         auto found = GetTrait(trait);
         if (not found) {
            mismatch = true;
            return Loop::Break;
         }

         selectedTraits << Abandon(found);
         return Loop::Continue;
      };

      const auto selectUnit = [&](const DMeta& type) {
         if (type->Is<Thing>())
            return Loop::Continue;

         auto found = GetUnitMeta(type);
         if (not found) {
            mismatch = true;
            return Loop::Break;
         }

         selectedUnits << found;
         return Loop::Continue;
      };

      const auto selectConstruct = [&](const Construct& construct) {
         if (construct.Is<Thing>()) {
            // Find an entity containing construct arguments            
            // Start with this one                                      
            Verbs::Select selector {construct.GetDescriptor()};
            Select(selector);
            if (selector.GetOutput()) {
               selectedEntities << this;
               return Loop::Continue;
            }
         }
         else if (construct.CastsTo<A::Unit>()) {
            // Find a unit containing construct arguments               
            if (not selectUnit(construct.GetType()))
               return Loop::Break;

            // selectedComponents has been populated with results       
            // Filter them additionally by construct arguments          
            TMany<A::Unit*> filteredSelectedComponents;
            for (auto& unit : selectedUnits) {
               bool localMismatch = false;
               auto unitBlock = unit->GetBlock();
               construct->ForEach(
                  [&](const Many& part) {
                     for (Offset i = 0; i < part.GetCount(); ++i) {
                        auto element = part.GetElementResolved(i);
                        Verbs::Select selector {element};
                        if (not Flow::DispatchFlat(unitBlock, selector)) {
                           // Abort on first failure                    
                           localMismatch = true;
                           return Loop::Break;
                        }
                     }

                     return Loop::Continue;
                  }
               );

               if (not localMismatch) {
                  // The unit passes the test                           
                  filteredSelectedComponents << unit;
               }
            }

            // Substitute the selected units with the filtered ones     
            selectedUnits = Move(filteredSelectedComponents);
         }

         return Loop::Break;
      };

      verb.ForEachDeep(
         [&](const Construct& construct) {
            return selectConstruct(construct);
         },
         [&](const Trait& trait) {
            return selectTrait(trait.GetTrait());
         },
         [&](const TMeta& trait) {
            return selectTrait(trait);
         },
         [&](const DMeta& type) {
            return selectUnit(type);
         }
      );

      if (not mismatch) {
         // We're not seeking an entity, but components/traits          
         if (selectedTraits) {
            ENTITY_SELECTION_VERBOSE_SELF(Logger::Green,
               "Trait(s) selected: ", selectedTraits);
            verb << selectedTraits;
         }

         if (selectedUnits) {
            ENTITY_SELECTION_VERBOSE_SELF(Logger::Green,
               "Unit(s) selected: ", selectedUnits);
            verb << selectedUnits;
         }

         if (selectedEntities) {
            ENTITY_SELECTION_VERBOSE_SELF(Logger::Green,
               "Entity(s) selected: ", selectedEntities);
            verb << selectedEntities;
         }
      }

      // Climb the hierarchy                                            
      if (not verb.IsDone() and mOwner)
         mOwner->Select(verb);
   }
      
} // namespace Langulus::Entry
