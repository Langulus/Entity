///                                                                           
/// Langulus::Entity                                                          
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#include "Thing.hpp"
#include "Runtime.hpp"

#define ENTITY_VERBOSE_SELF(a)            //Logger::Verbose() << this << ": "<< a
#define ENTITY_VERBOSE(a)                 //Logger::Append() << a
#define ENTITY_CREATION_VERBOSE_SELF(a)   //Logger::Verbose() << this << ": " <<a
#define ENTITY_SELECTION_VERBOSE_SELF(a)  //Logger::Verbose() << this << ": " <<a


namespace Langulus::Entity
{

   /// Execute a piece of langulus code in the closest possible temporal flow 
   ///   @param code - the code to parse and execute                          
   ///   @return the results of the execution                                 
   Any Thing::Run(const Code& code) {
      if (code.IsEmpty())
         return {};
      return Run(code.Parse());
   }

   /// Find an AIAD component that can process speech and interpret using it  
   /// This function completely relies on external modules                    
   ///   @param text - text to execute                                        
   ///   @return the results of the execution                                 
   Any Thing::Run(const Lingua& text) {
      if (text.IsEmpty())
         return {};

      // Message is still contextless, we don't know where exactly to   
      // interpret it, so create message objects from the hierarchy,    
      // and try interpreting those to executable scopes                
      // Each trained AI in the hierarchy will produce its own          
      // interpretation                                                 
      auto messages = CreateData(
         Construct::From<Lingua>(static_cast<const Text&>(text))
      );

      Verbs::InterpretTo<Flow::Scope> interpreter;
      if (!Flow::DispatchFlat(messages, interpreter)) {  
         Logger::Error()
            << "Messages failed to interpret to scope: " << messages;
         return {};
      }

      // Execute the resulting scopes                                   
      Any results;
      interpreter.GetOutput().ForEach([&](const Flow::Scope& scope) {
         results << Run(scope);
      });

      return Abandon(results);
   }

   /// Execute a scope in the entity's context                                
   ///   @param scope - the scope to execute                                  
   ///   @return the results of the execution                                 
   Any Thing::Run(const Scope& scope) {
      Any context {GetBlock()};
      Any output;
      if (!scope.Execute(context, output)) {
         Logger::Error() << "Can't execute scope: " << scope;
         return {};
      }

      return output;
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
      mUnits.ForEachValue([&](Unit* unit) {
         Verb local {verb};
         local.ShortCircuit(false);
         verb << Abandon(unit->Run(local));
      });
   }

   /// Create/Destroy stuff inside entity's context                           
   ///   @param verb - creation verb                                          
   void Thing::Create(Verb& verb) {
      if (verb.IsEmpty())
         return;

      const auto create = [&](const Construct& construct) {
         const auto count = static_cast<Count>(construct.GetCharge().mMass);
         for (Offset i = 0; i < count; ++i) {
            if (count != 1) {
               ENTITY_CREATION_VERBOSE_SELF(Logger::Yellow <<
                  "Charged creation - creating " << i + 1 << " of " << count);
            }

            if (construct.Is<Thing>()) {
               // Instantiate a child Thing                             
               verb << CreateChild(construct);
            }
            else if (construct.Is<Runtime>()) {
               // Instantiate a runtime                                 
               verb << CreateRuntime();
            }
            else if (construct.Is<Temporal>()) {
               // Instantiate a temporal flow                           
               verb << CreateFlow();
            }
            else if (construct.CastsTo<Module>()) {
               // Instantiate a module from the runtime                 
               auto runtime = GetRuntime();
               auto dependency = runtime->GetDependency(construct.GetType());
               verb << runtime->InstantiateModule(dependency, construct);
            }
            else {
               // Instantiate anything else                             
               verb << CreateData(construct);
            }
         }
      };

      // Scan the request                                               
      verb.ForEachDeep([&](const Block& group) {
         group.ForEach(
            [&](const Trait& trait) {
               ENTITY_CREATION_VERBOSE_SELF(
                  "Creating: " << Logger::Yellow << trait);
               verb << AddTrait(trait);
            },
            [&](const Construct& construct) {
               if (construct.GetCharge().mMass > 0) {
                  ENTITY_CREATION_VERBOSE_SELF(
                     "Creating: " << Logger::Yellow << construct);
                  create(construct);
               }
            },
            [&](const MetaData* type) {
               ENTITY_CREATION_VERBOSE_SELF(
                  "Creating: " << Logger::Yellow << type->mToken);
               create(Construct(type));
            }
         );
      });
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
         if (found.IsEmpty()) {
            mismatch = true;
            return false;
         }

         selectedTraits << Abandon(found);
         return true;
      };

      const auto selectUnit = [&](const MetaData* type) {
         if (type->Is<Thing>())
            return true;

         auto found = GetUnit(type);
         if (!found) {
            mismatch = true;
            return false;
         }

         selectedUnits << found;
         return true;
      };

      const auto selectConstruct = [&](const Construct& construct) {
         if (construct.Is<Thing>()) {
            // Find an entity containing construct arguments            
            // Start with this one                                      
            Verbs::Select selector {construct.GetArgument()};
            Select(selector);
            if (!selector.GetOutput().IsEmpty()) {
               selectedEntities << this;
               return true;
            }
         }
         else if (construct.CastsTo<Unit>()) {
            // Find a unit containing construct arguments               
            if (!selectUnit(construct.GetType()))
               return false;

            // selectedComponents has been populated with results       
            // Filter them additionally by construct arguments          
            TAny<Unit*> filteredSelectedComponents;
            for (auto& unit : selectedUnits) {
               bool localMismatch = false;
               auto unitBlock = unit->GetBlock();
               construct.ForEachDeep([&](const Block& part) {
                  for (Offset i = 0; i < part.GetCount(); ++i) {
                     auto element = part.GetElementResolved(i);
                     Verbs::Select selector {element};
                     if (!Flow::DispatchFlat(unitBlock, selector)) {
                        // Abort on first failure                       
                        localMismatch = true;
                        return false;
                     }
                  }

                  return true;
               });

               if (!localMismatch) {
                  // The unit passes the test                           
                  filteredSelectedComponents << unit;
               }
            }

            // Substitute the selected units with the filtered ones     
            selectedUnits = Move(filteredSelectedComponents);
         }

         return false;
      };

      verb.ForEachDeep([&](const Block& part) {
         part.ForEach(
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

         return !mismatch;
      });

      if (!mismatch) {
         // We're not seeking an entity, but components/traits          
         if (!selectedTraits.IsEmpty()) {
            ENTITY_SELECTION_VERBOSE_SELF(Logger::Green 
               << "Trait(s) selected: " << selectedTraits);
            verb << selectedTraits;
         }

         if (!selectedUnits.IsEmpty()) {
            ENTITY_SELECTION_VERBOSE_SELF(Logger::Green
               << "Unit(s) selected: " << selectedUnits);
            verb << selectedUnits;
         }

         if (!selectedEntities.IsEmpty()) {
            ENTITY_SELECTION_VERBOSE_SELF(Logger::Green
               << "Entity(s) selected: " << selectedEntities);
            verb << selectedEntities;
         }
      }

      // Climb the hierarchy                                            
      if (!verb.IsDone() && mOwner)
         mOwner->Select(verb);
   }
      
} // namespace Langulus::Entry
