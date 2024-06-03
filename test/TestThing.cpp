///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Common.hpp"


SCENARIO("Testing Thing", "[thing]") {
   static Allocator::State memoryState;

   WHEN("Creating a default Thing") {
      Thing root;

      REQUIRE(root.GetOwner() == nullptr);
      REQUIRE(root.GetRuntime() == nullptr);
      REQUIRE(root.GetRuntime().IsLocked() == false);
      REQUIRE(root.GetFlow() == nullptr);
      REQUIRE(root.GetFlow().IsLocked() == false);
      REQUIRE(root.RequiresRefresh() == false);
      REQUIRE(not root.GetChildren());
      REQUIRE(not root.GetUnits());
      REQUIRE(not root.GetUnitsMap());
      REQUIRE(not root.GetTraits());
      REQUIRE(root.Reference(0) == 1);
   }

   WHEN("Creating a Thing with a parent") {
      Thing root;
      Thing child {&root};

      REQUIRE(root.GetOwner() == nullptr);
      REQUIRE(root.GetRuntime() == nullptr);
      REQUIRE(root.GetRuntime().IsLocked() == false);
      REQUIRE(root.GetFlow() == nullptr);
      REQUIRE(root.GetFlow().IsLocked() == false);
      REQUIRE(root.RequiresRefresh() == false);
      REQUIRE(root.GetChildren().GetCount() == 1);
      REQUIRE(root.GetChildren()[0] == &child);
      REQUIRE(not root.GetUnits());
      REQUIRE(not root.GetUnitsMap());
      REQUIRE(not root.GetTraits());
      REQUIRE(root.Reference(0) == 1);

      REQUIRE(child.GetOwner() == &root);
      REQUIRE(child.GetRuntime() == nullptr);
      REQUIRE(child.GetRuntime().IsLocked() == false);
      REQUIRE(child.GetFlow() == nullptr);
      REQUIRE(child.GetFlow().IsLocked() == false);
      REQUIRE(child.RequiresRefresh() == false);
      REQUIRE(not child.GetChildren());
      REQUIRE(not child.GetUnits());
      REQUIRE(not child.GetUnitsMap());
      REQUIRE(not child.GetTraits());
      REQUIRE(child.Reference(0) == 1);
   }
   
   GIVEN("A root Thing") {
      Thing root;

      WHEN("Creating a runtime") {
         auto runtime = root.CreateRuntime();

         REQUIRE(root.GetRuntime() == runtime);
         REQUIRE(root.GetRuntime().IsLocked() == true);
      }

      WHEN("Creating a temporal flow") {
         auto flow = root.CreateFlow();

         REQUIRE(root.GetFlow() == flow);
         REQUIRE(root.GetFlow().IsLocked() == true);
      }

      WHEN("Creating an empty child") {
         auto child = root.CreateChild();

         REQUIRE(root.GetOwner() == nullptr);
         REQUIRE(root.GetRuntime() == nullptr);
         REQUIRE(root.GetRuntime().IsLocked() == false);
         REQUIRE(root.GetFlow() == nullptr);
         REQUIRE(root.GetFlow().IsLocked() == false);
         REQUIRE(root.RequiresRefresh() == false);
         REQUIRE(root.GetChildren().GetCount() == 1);
         REQUIRE(not root.GetUnits());
         REQUIRE(not root.GetTraits());
         REQUIRE(root.Reference(0) == 1);

         auto child1 = root.GetChildren()[0];
         REQUIRE(child1 == child);
         REQUIRE(child1->GetOwner() == &root);
         REQUIRE(child1->GetRuntime() == root.GetRuntime());
         REQUIRE(child1->GetRuntime().IsLocked() == false);
         REQUIRE(child1->GetFlow() == root.GetFlow());
         REQUIRE(child1->GetFlow().IsLocked() == false);
         REQUIRE(child1->RequiresRefresh() == false);
         REQUIRE(not child1->GetChildren());
         REQUIRE(not child1->GetUnits());
         REQUIRE(not child1->GetTraits());
         REQUIRE(child1->Reference(0) == 3);
      }

      WHEN("Adding an existing unit") {
         TestUnit1 testUnit;
         auto added = root.AddUnit(&testUnit);

         THEN("Properties should match") {
            REQUIRE(added == 1);
            REQUIRE(root.GetOwner() == nullptr);
            REQUIRE(root.GetRuntime() == nullptr);
            REQUIRE(root.GetRuntime().IsLocked() == false);
            REQUIRE(root.GetFlow() == nullptr);
            REQUIRE(root.GetFlow().IsLocked() == false);
            REQUIRE(root.RequiresRefresh() == true);
            REQUIRE(root.GetChildren().IsEmpty());
            REQUIRE(root.GetUnits().GetCount() == 1);
            REQUIRE(root.GetTraits().IsEmpty());
            REQUIRE(root.Reference(0) == 1);

            auto it = root.GetUnits().begin();
            REQUIRE(it->GetType() == MetaOf<TestUnit1>());
            REQUIRE(*it == &testUnit);
            REQUIRE(it->GetOwners().GetCount() == 1);
            REQUIRE(it->GetOwners()[0] == &root);
            REQUIRE(it->Reference(0) == 1);
         }
      }

      WHEN("Creating a new unit") {
         auto unit = root.CreateUnit<TestUnit1>();

         THEN("Properties should match") {
            REQUIRE(root.GetOwner() == nullptr);
            REQUIRE(root.GetRuntime() == nullptr);
            REQUIRE(root.GetRuntime().IsLocked() == false);
            REQUIRE(root.GetFlow() == nullptr);
            REQUIRE(root.GetFlow().IsLocked() == false);
            REQUIRE(root.RequiresRefresh() == true);
            REQUIRE(root.GetChildren().IsEmpty());
            REQUIRE(root.GetUnits().GetCount() == 1);
            REQUIRE(root.GetTraits().IsEmpty());
            REQUIRE(root.Reference(0) == 1);

            auto it = root.GetUnits().begin();
            REQUIRE(it->GetType() == MetaOf<TestUnit1>());
            REQUIRE(*it == unit.As<TestUnit1*>());
            REQUIRE(unit.GetUses() == 3);
            REQUIRE(Fractalloc::Instance.Find({}, unit.As<TestUnit1*>())->GetUses() == 3);
            REQUIRE(unit.As<TestUnit1*>()->Reference(0) == 3);
            REQUIRE(Fractalloc::Instance.Find({}, unit.As<TestUnit1*>())->GetUses() == 3);
            REQUIRE(it->GetOwners().GetCount() == 1);
            REQUIRE(it->GetOwners()[0] == &root);
            REQUIRE(it->Reference(0) == 3);
         }
      }

      WHEN("Adding a trait") {
         auto trait = root.AddTrait(Traits::Count {666});

         REQUIRE(trait != nullptr);

         REQUIRE(root.GetOwner() == nullptr);
         REQUIRE(root.GetRuntime() == nullptr);
         REQUIRE(root.GetRuntime().IsLocked() == false);
         REQUIRE(root.GetFlow() == nullptr);
         REQUIRE(root.GetFlow().IsLocked() == false);
         REQUIRE(root.RequiresRefresh() == true);
         REQUIRE(not root.GetChildren());
         REQUIRE(not root.GetUnits());
         REQUIRE(not root.GetUnitsMap());
         REQUIRE(root.GetTraits().GetCount() == 1);
         REQUIRE(root.Reference(0) == 1);

         auto it = root.GetTraits().begin();
         REQUIRE(*it.mKey == MetaOf<Traits::Count>());
         REQUIRE(it.mValue->GetCount() == 1);
         REQUIRE((*it.mValue)[0] == Traits::Count {666});
         REQUIRE(&(*it.mValue)[0] == trait);
      }

      WHEN("Set a name trait") {
         auto trait = root.AddTrait(Traits::Name {"Dimo"});

         REQUIRE(trait != nullptr);

         REQUIRE(root.GetOwner() == nullptr);
         REQUIRE(root.GetRuntime() == nullptr);
         REQUIRE(root.GetRuntime().IsLocked() == false);
         REQUIRE(root.GetFlow() == nullptr);
         REQUIRE(root.GetFlow().IsLocked() == false);
         REQUIRE(root.RequiresRefresh() == true);
         REQUIRE(not root.GetChildren());
         REQUIRE(not root.GetUnits());
         REQUIRE(not root.GetUnitsMap());
         REQUIRE(root.GetTraits().GetCount() == 1);
         REQUIRE(root.GetName() == "Dimo");
         REQUIRE(root.Reference(0) == 1);

         auto it = root.GetTraits().begin();
         REQUIRE(*it.mKey == MetaOf<Traits::Name>());
         REQUIRE(it.mValue->GetCount() == 1);
         REQUIRE((*it.mValue)[0] == Traits::Name {"Dimo"});
         REQUIRE(&(*it.mValue)[0] == trait);
      }
   }

   for (int repeat = 0; repeat != 10; ++repeat) {
      WHEN(std::string("Creating a Thing by descriptor #") + std::to_string(repeat)) {
         Logger::Special("Start: Creating a Thing by descriptor");
         Neat descriptor {
            Traits::Name {"Root"},
            Construct::From<Runtime>(),
            Construct::From<Temporal>(),
            Construct::From<TestUnit1>(),
            Construct::From<TestUnit2>(),
            Construct::From<Thing>(
               Traits::Name {"Child1"},
               Construct::From<TestUnit1>(),
               Construct::From<TestUnit2>(),
               Construct::From<Thing>(Traits::Name {"GrandChild1"}),
               Construct::From<Thing>(Traits::Name {"GrandChild2"})
            ),
            Construct::From<Thing>(Traits::Name {"Child2"})
         };

         Thing root {Describe(descriptor)};

         REQUIRE(root.GetOwner() == nullptr);
         REQUIRE(root.GetRuntime() != nullptr);
         REQUIRE(root.GetRuntime().IsLocked() == true);
         REQUIRE(root.GetFlow() != nullptr);
         REQUIRE(root.GetFlow().IsLocked() == true);
         REQUIRE(root.RequiresRefresh() == true);
         REQUIRE(root.GetChildren().GetCount() == 2);
         REQUIRE(root.GetUnits().GetCount() == 2);
         REQUIRE(root.GetTraits().GetCount() == 1);
         REQUIRE(root.GetName() == "Root");
         REQUIRE(root.Reference(0) == 1);

         auto child1 = root.GetChildren()[0];
         REQUIRE(child1->GetOwner() == &root);
         REQUIRE(child1->GetRuntime() == root.GetRuntime());
         REQUIRE(child1->GetRuntime().IsLocked() == false);
         REQUIRE(child1->GetFlow() == root.GetFlow());
         REQUIRE(child1->GetFlow().IsLocked() == false);
         REQUIRE(child1->RequiresRefresh() == true);
         REQUIRE(child1->GetChildren().GetCount() == 2);
         REQUIRE(child1->GetUnits().GetCount() == 2);
         REQUIRE(child1->GetTraits().GetCount() == 1);
         REQUIRE(child1->GetName() == "Child1");

         // the child1 container has 1 reference
         // the root.mChildren[0] contains 1 reference
         // child1's TestUnit1 owner contains 1 references from owner
         // child1's TestUnit2 owner contains 1 reference from owner
         // child1's GrandChild1 owner contains 1 reference
         // child1's GrandChild2 owner contains 1 reference
         //---------------------------------------------------
         // total: 8 references confirmed
         REQUIRE(child1->Reference(0) == 6);

         auto child2 = root.GetChildren()[1];
         REQUIRE(child2->GetOwner() == &root);
         REQUIRE(child2->GetRuntime() == root.GetRuntime());
         REQUIRE(child2->GetRuntime().IsLocked() == false);
         REQUIRE(child2->GetFlow() == root.GetFlow());
         REQUIRE(child2->GetFlow().IsLocked() == false);
         REQUIRE(child2->RequiresRefresh() == true);
         REQUIRE(not child2->GetChildren());
         REQUIRE(not child2->GetUnits());
         REQUIRE(child2->GetTraits().GetCount() == 1);
         REQUIRE(child2->GetName() == "Child2");
         REQUIRE(child2->Reference(0) == 2);

         auto grandchild1 = child1->GetChildren()[0];
         REQUIRE(grandchild1->GetOwner() == child1);
         REQUIRE(grandchild1->GetRuntime() == root.GetRuntime());
         REQUIRE(grandchild1->GetRuntime().IsLocked() == false);
         REQUIRE(grandchild1->GetFlow() == root.GetFlow());
         REQUIRE(grandchild1->GetFlow().IsLocked() == false);
         REQUIRE(grandchild1->RequiresRefresh() == true);
         REQUIRE(not grandchild1->GetChildren());
         REQUIRE(not grandchild1->GetUnits());
         REQUIRE(grandchild1->GetTraits().GetCount() == 1);
         REQUIRE(grandchild1->GetName() == "GrandChild1");
         REQUIRE(grandchild1->Reference(0) == 2);

         auto grandchild2 = child1->GetChildren()[1];
         REQUIRE(grandchild2->GetOwner() == child1);
         REQUIRE(grandchild2->GetRuntime() == root.GetRuntime());
         REQUIRE(grandchild2->GetRuntime().IsLocked() == false);
         REQUIRE(grandchild2->GetFlow() == root.GetFlow());
         REQUIRE(grandchild2->GetFlow().IsLocked() == false);
         REQUIRE(grandchild2->RequiresRefresh() == true);
         REQUIRE(not grandchild2->GetChildren());
         REQUIRE(not grandchild2->GetUnits());
         REQUIRE(grandchild2->GetTraits().GetCount() == 1);
         REQUIRE(grandchild2->GetName() == "GrandChild2");
         REQUIRE(grandchild2->Reference(0) == 2);

         Logger::Special("End: Creating a Thing by descriptor");
      }
   }

   GIVEN("A complex hierarchy with runtime, flow, units, and traits") {
      Neat descriptor {
         Traits::Name {"Root"},
         Construct::From<Runtime>(),
         Construct::From<Temporal>(),
         Construct::From<TestUnit1>(),
         Construct::From<TestUnit2>(),
         Construct::From<Thing>(
            Traits::Name {"Child1"},
            Construct::From<TestUnit1>(),
            Construct::From<TestUnit2>(),
            Construct::From<Thing>(
               Traits::Name {"GrandChild1"}
            ),
            Construct::From<Thing>(
               Traits::Name {"GrandChild2"}
            )
         ),
         Construct::From<Thing>(
            Traits::Name {"Child2"}
         ),
         Construct::From<Thing>(
            Traits::Name {"Child2"}
         )
      };

      Thing root {Describe(descriptor)};

      WHEN("Getting a local child by index") {
         auto child = root.GetChild(0);
         auto child1 = root.GetChildren()[0];
         REQUIRE(child1 == child);
      }

      WHEN("Getting a local child by name trait") {
         auto child = root.GetNamedChild("Child2");
         auto child2 = root.GetChildren()[1];
         REQUIRE(child2 == child);
      }

      WHEN("Getting a local child by index and name trait") {
         auto child = root.GetNamedChild("Child2", 1);
         auto child3 = root.GetChildren()[2];
         REQUIRE(child3 == child);
      }

      WHEN("Get a local unit by index") {
         // Hash algorithm changes might swap the order of these two    
         auto unit0 = root.GetUnit(0);
         auto unit1 = root.GetUnit(1);

         REQUIRE(unit0);
         REQUIRE(unit0->Is<TestUnit1>());
         REQUIRE(unit0 == root.GetUnitsMap()[MetaOf<TestUnit1>()][0]);
         REQUIRE(unit1);
         REQUIRE(unit1->Is<TestUnit2>());
         REQUIRE(unit1 == root.GetUnitsMap()[MetaOf<TestUnit2>()][0]);
      }

      WHEN("Get a local unit by type and index") {
         auto unit = root.GetUnitMeta(MetaOf<TestUnit1>());

         REQUIRE(unit);
         REQUIRE(unit->Is<TestUnit1>());
         REQUIRE(unit == root.GetUnitsMap()[MetaOf<TestUnit1>()][0]);
      }

      WHEN("Removing a specific local unit") {
         auto unitmemory = root.GetUnit(0);
         auto removed = root.RemoveUnit(root.GetUnit(0));

         REQUIRE(removed == 1);
         REQUIRE(root.GetUnits().GetCount() == 1);
         REQUIRE(nullptr == Allocator::Find(MetaOf<TestUnit1>(), unitmemory));
      }

      WHEN("Removing all local units") {
         auto unitmemory1 = root.GetUnit(0);
         auto unitmemory2 = root.GetUnit(1);
         auto removed = root.RemoveUnits();

         REQUIRE(removed == 2);
         REQUIRE(not root.GetUnits());
         REQUIRE(nullptr == Allocator::Find(MetaOf<TestUnit1>(), unitmemory1));
         REQUIRE(nullptr == Allocator::Find(MetaOf<TestUnit2>(), unitmemory2));
      }

      /*WHEN("Replace local units") {
         TestUnit2 replacement;
         auto replaced = root.ReplaceUnit(root.GetUnit<TestUnit1>(), &replacement);

         REQUIRE(replaced == 1);
         REQUIRE(root.mUnitsList.GetCount() == 2);
         REQUIRE(root.mUnitsAmbiguous[MetaOf<TestUnit2>()].GetCount() == 2);
         REQUIRE(root.mUnitsAmbiguous[MetaOf<TestUnit2>()].Contains(&replacement));
      }*/

      WHEN("Count available units") {
         auto found = root.HasUnits<TestUnit1>();

         REQUIRE(found == 1);
      }

      WHEN("Gather units of a specific type") {
         auto found1 = root.GatherUnits<TestUnit1>();

         REQUIRE(found1.GetCount() == 1);
      }

      /*WHEN("Seek a unit by index") {
         auto unit = root.SeekUnit(0);
      }

      WHEN("Seek a unit by type and index") {
         auto unit = root.SeekUnit<Unit>(0);
      }

      WHEN("Get a static/unit/dynamic trait by index") {
         auto child = root.GetTrait(0);
      }

      WHEN("Get a static/unit/dynamic trait by type and index") {
         auto child = root.GetTrait<Traits::Name>();
      }

      WHEN("Seek a trait in the hierarchy by index") {
         auto trait = root.SeekTrait(0);
      }

      WHEN("Seek a trait in the hierarchy by type index") {
         auto child = root.SeekTrait<Traits::Name>();
      }*/
   }

   REQUIRE(memoryState.Assert());
}