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

#if 0
   #define ENTITY_VERBOSE_ENABLED()             1
   #define ENTITY_VERBOSE_SELF(...)             Logger::Verbose(this, ": ", __VA_ARGS__)
   #define ENTITY_VERBOSE_SELF_TAB(...)         const auto scoped = Logger::VerboseTab(this, ": ", __VA_ARGS__)
   #define ENTITY_VERBOSE(...)                  Logger::Append(__VA_ARGS__)
   #define ENTITY_CREATION_VERBOSE_SELF(...)    Logger::Verbose(Self(), __VA_ARGS__)
   #define ENTITY_SELECTION_VERBOSE_SELF(...)   Logger::Verbose(Self(), __VA_ARGS__)
#else
   #define ENTITY_VERBOSE_ENABLED()             0
   #define ENTITY_VERBOSE_SELF(...)             LANGULUS(NOOP)
   #define ENTITY_VERBOSE_SELF_TAB(...)         LANGULUS(NOOP)
   #define ENTITY_VERBOSE(...)                  LANGULUS(NOOP)
   #define ENTITY_CREATION_VERBOSE_SELF(...)    LANGULUS(NOOP)
   #define ENTITY_SELECTION_VERBOSE_SELF(...)   LANGULUS(NOOP)
#endif


namespace Langulus::Entity
{

   /// Find an AIAD component that can process speech and interpret using it  
   /// This function completely relies on external modules                    
   ///   @param text - text to execute                                        
   ///   @return the results of the execution                                 
   Many Thing::Run(const Lingua& text) {
      if (not text)
         return {};

      // Message is still contextless, we don't know where exactly to   
      // interpret it, so create message objects from the hierarchy,    
      // and try interpreting those to executable temporals             
      // Each trained AI in the hierarchy will produce its own          
      // interpretation                                                 
      auto messages = CreateData(
         Construct::From<Lingua>(static_cast<const Text&>(text)));

      Verbs::InterpretAs<Flow::Temporal> interpreter;
      if (not Flow::DispatchFlat(messages, interpreter)) {  
         Logger::Error("Messages failed to interpret to temporal: ", messages);
         return {};
      }

      // Execute the resulting scopes                                   
      Many results;
      interpreter->ForEach([&](const Flow::Temporal& scope) {
         results << Resolvable::Run(scope);
      });

      return Abandon(results);
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

      /*if (not flow->Push(Verbs::Do::In(this, Abandon(parsed)))) {
         // Couldn't push verbs - the flow probably has no future link  
         // points, or we're out of memory?                             
         Logger::Error(this, ": Flow ", *flow, " couldn't Run code: ", code);
         return {};
      }
      
      // Flow has changed, update it as if no time has passed, in order 
      // to execute any newly added parts, if in the proper time        
      // context. Anything produced as a side-effect will be returned.  
      // @attention, if you push a command that has time context before 
      // the current flow time, you'll be technically rewriting this    
      // thing's history - the entire timeline will be reverted back,   
      // and recomputed up to now (if that's possible).                 
      flow->Update({});
      return {};*/
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
               construct.GetDescriptor().ForEach(
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
