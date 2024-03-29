///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#include "Thing.inl"
#include "Runtime.hpp"
#include <Flow/Verbs/Interpret.hpp>

#if 0
   #define ENTITY_VERBOSE_ENABLED() 1
   #define ENTITY_VERBOSE_SELF(...)            Logger::Verbose(Self(), __VA_ARGS__)
   #define ENTITY_VERBOSE(...)                 Logger::Append(__VA_ARGS__)
   #define ENTITY_CREATION_VERBOSE_SELF(...)   Logger::Verbose(Self(), __VA_ARGS__)
   #define ENTITY_SELECTION_VERBOSE_SELF(...)  Logger::Verbose(Self(), __VA_ARGS__)
#else
   #define ENTITY_VERBOSE_ENABLED() 0
   #define ENTITY_VERBOSE_SELF(...)
   #define ENTITY_VERBOSE(...)
   #define ENTITY_CREATION_VERBOSE_SELF(...)
   #define ENTITY_SELECTION_VERBOSE_SELF(...)
#endif


namespace Langulus::Entity
{

   /// Find an AIAD component that can process speech and interpret using it  
   /// This function completely relies on external modules                    
   ///   @param text - text to execute                                        
   ///   @return the results of the execution                                 
   Any Thing::Run(const Lingua& text) {
      if (not text)
         return {};

      // Message is still contextless, we don't know where exactly to   
      // interpret it, so create message objects from the hierarchy,    
      // and try interpreting those to executable temporals             
      // Each trained AI in the hierarchy will produce its own          
      // interpretation                                                 
      auto messages = CreateData(
         Construct::From<Lingua>(static_cast<const Text&>(text))
      );

      Verbs::InterpretAs<Flow::Temporal> interpreter;
      if (not Flow::DispatchFlat(messages, interpreter)) {  
         Logger::Error("Messages failed to interpret to temporal: ", messages);
         return {};
      }

      // Execute the resulting scopes                                   
      Any results;
      interpreter->ForEach([&](const Flow::Temporal& scope) {
         results << Resolvable::Run(scope);
      });

      return Abandon(results);
   }

   /// Custom dispatcher, reflected via CT::Dispatcher                        
   /// Entity is a composite type, and its behavior depends on the coupled    
   /// units, so the custom dispatcher forwards verbs to these units          
   ///   @param verb - any verb                                               
   void Thing::Do(Verb& verb) {
      // Dispatch to entity first, using reflected and default verbs,   
      // but disallowing custom dispatch, because we're currently in it 
      // and there's a potential for infinite regress                   
      Resolvable::Run<false>(verb);
      if (verb.IsDone())
         return;

      // If verb is still not satisfied, dispatch to ALL units          
      for (auto& unit : mUnitsList) {
         Verb local {verb};
         local.ShortCircuit(false);
         verb << Abandon(unit->Run(local));
      }
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
      TAny<Trait> selectedTraits;
      TAny<Unit*> selectedUnits;
      TAny<Thing*> selectedEntities;
      bool mismatch = false;

      const auto selectTrait = [&](const MetaTrait* trait) {
         auto found = GetTrait(trait);
         if (not found) {
            mismatch = true;
            return Flow::Break;
         }

         selectedTraits << Abandon(found);
         return Flow::Continue;
      };

      const auto selectUnit = [&](const MetaData* type) {
         if (type->Is<Thing>())
            return Flow::Continue;

         auto found = GetUnitMeta(type);
         if (not found) {
            mismatch = true;
            return Flow::Break;
         }

         selectedUnits << found;
         return Flow::Continue;
      };

      const auto selectConstruct = [&](const Construct& construct) {
         if (construct.Is<Thing>()) {
            // Find an entity containing construct arguments            
            // Start with this one                                      
            Verbs::Select selector {construct.GetDescriptor()};
            Select(selector);
            if (selector.GetOutput()) {
               selectedEntities << this;
               return Flow::Continue;
            }
         }
         else if (construct.CastsTo<Unit>()) {
            // Find a unit containing construct arguments               
            if (not selectUnit(construct.GetType()))
               return Flow::Break;

            // selectedComponents has been populated with results       
            // Filter them additionally by construct arguments          
            TAny<Unit*> filteredSelectedComponents;
            for (auto& unit : selectedUnits) {
               bool localMismatch = false;
               auto unitBlock = unit->GetBlock();
               construct.GetDescriptor().ForEach([&](const Block& part) {
                  for (Offset i = 0; i < part.GetCount(); ++i) {
                     auto element = part.GetElementResolved(i);
                     Verbs::Select selector {element};
                     if (not Flow::DispatchFlat(unitBlock, selector)) {
                        // Abort on first failure                       
                        localMismatch = true;
                        return Flow::Break;
                     }
                  }

                  return Flow::Continue;
               });

               if (not localMismatch) {
                  // The unit passes the test                           
                  filteredSelectedComponents << unit;
               }
            }

            // Substitute the selected units with the filtered ones     
            selectedUnits = Move(filteredSelectedComponents);
         }

         return Flow::Break;
      };

      verb.ForEachDeep(
         [&](const Construct& construct) {
            return selectConstruct(construct);
         },
         [&](const Trait& trait) {
            return selectTrait(trait.GetTrait());
         },
         [&](const MetaTrait* trait) {
            return selectTrait(trait);
         },
         [&](const MetaData* type) {
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
