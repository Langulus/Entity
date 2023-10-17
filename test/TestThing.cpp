///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#include "Main.hpp"
#include <catch2/catch.hpp>

/// See https://github.com/catchorg/Catch2/blob/devel/docs/tostring.md			
CATCH_TRANSLATE_EXCEPTION(::Langulus::Exception const& ex) {
	const Text serialized {ex};
	return ::std::string {Token {serialized}};
}

SCENARIO("Testing Thing", "[thing]") {
   WHEN("Creating a default Thing") {
      Thing root;

      THEN("Properties should match") {
         REQUIRE(root.mOwner == nullptr);
         REQUIRE(*root.mRuntime == nullptr);
         REQUIRE(root.mRuntime.IsLocked() == false);
         REQUIRE(*root.mFlow == nullptr);
         REQUIRE(root.mFlow.IsLocked() == false);
         REQUIRE(root.mRefreshRequired == false);
         REQUIRE(!root.mChildren);
         REQUIRE(!root.mUnitsList);
         REQUIRE(!root.mUnitsAmbiguous);
         REQUIRE(!root.mTraits);
         REQUIRE(root.GetReferences() == 1);
      }
   }

   WHEN("Creating a Thing with a parent") {
      Thing root;
      Thing child {&root};

      THEN("Properties should match") {
         REQUIRE(root.mOwner == nullptr);
         REQUIRE(*root.mRuntime == nullptr);
         REQUIRE(root.mRuntime.IsLocked() == false);
         REQUIRE(*root.mFlow == nullptr);
         REQUIRE(root.mFlow.IsLocked() == false);
         REQUIRE(root.mRefreshRequired == false);
         REQUIRE(root.mChildren.GetCount() == 1);
         REQUIRE(root.mChildren[0] == &child);
         REQUIRE(!root.mUnitsList);
         REQUIRE(!root.mUnitsAmbiguous);
         REQUIRE(!root.mTraits);
         REQUIRE(root.GetReferences() == 2);

         REQUIRE(child.mOwner == &root);
         REQUIRE(*child.mRuntime == nullptr);
         REQUIRE(child.mRuntime.IsLocked() == false);
         REQUIRE(*child.mFlow == nullptr);
         REQUIRE(child.mFlow.IsLocked() == false);
         REQUIRE(child.mRefreshRequired == false);
         REQUIRE(!child.mChildren);
         REQUIRE(!child.mUnitsList);
         REQUIRE(!child.mUnitsAmbiguous);
         REQUIRE(!child.mTraits);
         REQUIRE(child.GetReferences() == 2);
      }
   }
   
   GIVEN("A root Thing") {
      Thing root;

      WHEN("Creating a runtime") {
         auto runtime = root.CreateRuntime();

         THEN("Properties should match") {
            REQUIRE(*root.mRuntime == runtime);
            REQUIRE(root.mRuntime.IsLocked() == true);
         }
      }

      WHEN("Creating a temporal flow") {
         auto flow = root.CreateFlow();

         THEN("Properties should match") {
            REQUIRE(*root.mFlow == flow);
            REQUIRE(root.mFlow.IsLocked() == true);
         }
      }

      WHEN("Creating an empty child") {
         auto child = root.CreateChild();

         THEN("Properties should match") {
            REQUIRE(root.mOwner == nullptr);
            REQUIRE(*root.mRuntime == nullptr);
            REQUIRE(root.mRuntime.IsLocked() == false);
            REQUIRE(*root.mFlow == nullptr);
            REQUIRE(root.mFlow.IsLocked() == false);
            REQUIRE(root.mRefreshRequired == false);
            REQUIRE(root.mChildren.GetCount() == 1);
            REQUIRE(!root.mUnitsList);
            REQUIRE(!root.mTraits);
            REQUIRE(root.GetReferences() == 2);

            auto child1 = root.mChildren[0];
            REQUIRE(child1 == child);
            REQUIRE(child1->mOwner == &root);
            REQUIRE(child1->mRuntime == root.mRuntime);
            REQUIRE(child1->mRuntime.IsLocked() == false);
            REQUIRE(child1->mFlow == root.mFlow);
            REQUIRE(child1->mFlow.IsLocked() == false);
            REQUIRE(child1->mRefreshRequired == false);
            REQUIRE(!child1->mChildren);
            REQUIRE(!child1->mUnitsList);
            REQUIRE(!child1->mTraits);
            REQUIRE(child1->GetReferences() == 3);
         }
      }

      /*WHEN("Adding an existing unit") {
         TestUnit1 testUnit;
         auto added = root.AddUnit(&testUnit);

         THEN("Properties should match") {
            REQUIRE(added == 1);
            REQUIRE(root.mOwner == nullptr);
            REQUIRE(root.mRuntime == nullptr);
            REQUIRE(root.mRuntime.IsPinned() == false);
            REQUIRE(root.mFlow == nullptr);
            REQUIRE(root.mFlow.IsPinned() == false);
            REQUIRE(root.mRefreshRequired == true);
            REQUIRE(root.mChildren.IsEmpty());
            REQUIRE(root.mUnits.GetCount() == 1);
            REQUIRE(root.mTraits.IsEmpty());
            REQUIRE(root.GetReferences() == 2);

            auto it = root.mUnits.begin();
            REQUIRE(it->mKey == MetaOf<TestUnit1>());
            REQUIRE(it->mValue.GetCount() == 1);
            REQUIRE(it->mValue[0] == &testUnit);
            REQUIRE(it->mValue[0]->mOwners.GetCount() == 1);
            REQUIRE(it->mValue[0]->mOwners[0] == &root);
            REQUIRE(it->mValue[0]->GetReferences() == 2);
         }
      }*/

      /*WHEN("Creating a new unit") {
         auto unit = root.CreateUnit<TestUnit1>();

         THEN("Properties should match") {
            REQUIRE(root.mOwner == nullptr);
            REQUIRE(root.mRuntime == nullptr);
            REQUIRE(root.mRuntime.IsPinned() == false);
            REQUIRE(root.mFlow == nullptr);
            REQUIRE(root.mFlow.IsPinned() == false);
            REQUIRE(root.mRefreshRequired == true);
            REQUIRE(root.mChildren.IsEmpty());
            REQUIRE(root.mUnits.GetCount() == 1);
            REQUIRE(root.mTraits.IsEmpty());
            REQUIRE(root.GetReferences() == 2);

            auto it = root.mUnits.begin();
            REQUIRE(it->mKey == MetaOf<TestUnit1>());
            REQUIRE(it->mValue.GetCount() == 1);
            REQUIRE(it->mValue[0] == unit.As<TestUnit1*>());
            REQUIRE(it->mValue[0]->mOwners.GetCount() == 1);
            REQUIRE(it->mValue[0]->mOwners[0] == &root);
            REQUIRE(it->mValue[0]->GetReferences() == 2);
         }
      }*/

      WHEN("Adding a trait") {
         auto trait = root.AddTrait(Traits::Count {666});

         THEN("Properties should match") {
            REQUIRE(trait != nullptr);

            REQUIRE(root.mOwner == nullptr);
            REQUIRE(*root.mRuntime == nullptr);
            REQUIRE(root.mRuntime.IsLocked() == false);
            REQUIRE(*root.mFlow == nullptr);
            REQUIRE(root.mFlow.IsLocked() == false);
            REQUIRE(root.mRefreshRequired == true);
            REQUIRE(!root.mChildren);
            REQUIRE(!root.mUnitsList);
            REQUIRE(!root.mUnitsAmbiguous);
            REQUIRE(root.mTraits.GetCount() == 1);
            REQUIRE(root.GetReferences() == 1);

            auto it = root.mTraits.begin();
            REQUIRE(it->mKey == MetaOf<Traits::Count>());
            REQUIRE(it->mValue.GetCount() == 1);
            REQUIRE(it->mValue[0] == Traits::Count {666});
            REQUIRE(&it->mValue[0] == trait);
         }
      }

      WHEN("Set a name trait") {
         auto trait = root.AddTrait(Traits::Name {"Dimo"});

         THEN("Properties should match") {
            REQUIRE(trait != nullptr);

            REQUIRE(root.mOwner == nullptr);
            REQUIRE(*root.mRuntime == nullptr);
            REQUIRE(root.mRuntime.IsLocked() == false);
            REQUIRE(*root.mFlow == nullptr);
            REQUIRE(root.mFlow.IsLocked() == false);
            REQUIRE(root.mRefreshRequired == true);
            REQUIRE(!root.mChildren);
            REQUIRE(!root.mUnitsList);
            REQUIRE(!root.mUnitsAmbiguous);
            REQUIRE(root.mTraits.GetCount() == 1);
            REQUIRE(root.GetName() == "Dimo");
            REQUIRE(root.GetReferences() == 1);

            auto it = root.mTraits.begin();
            REQUIRE(it->mKey == MetaOf<Traits::Name>());
            REQUIRE(it->mValue.GetCount() == 1);
            REQUIRE(it->mValue[0] == Traits::Name {"Dimo"});
            REQUIRE(&it->mValue[0] == trait);
         }
      }
   }

   WHEN("Creating a Thing by descriptor") {
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
            Construct::From<Thing>(
               Traits::Name {"GrandChild1"}
            ),
            Construct::From<Thing>(
               Traits::Name {"GrandChild2"}
            )
         ),
         Construct::From<Thing>(
            Traits::Name {"Child2"}
         )
      };

      Thing root {Describe(descriptor)};

      THEN("Properties should match") {
         REQUIRE(root.mOwner == nullptr);
         REQUIRE(*root.mRuntime != nullptr);
         REQUIRE(root.mRuntime.IsLocked() == true);
         REQUIRE(*root.mFlow != nullptr);
         REQUIRE(root.mFlow.IsLocked() == true);
         REQUIRE(root.mRefreshRequired == true);
         REQUIRE(root.mChildren.GetCount() == 2);
         REQUIRE(root.mUnitsList.GetCount() == 2);
         REQUIRE(root.mTraits.GetCount() == 1);
         REQUIRE(root.GetName() == "Root");
         REQUIRE(root.GetReferences() == 5);

         auto child1 = root.mChildren[0];
         REQUIRE(child1->mOwner == &root);
         REQUIRE(child1->mRuntime == root.mRuntime);
         REQUIRE(child1->mRuntime.IsLocked() == false);
         REQUIRE(child1->mFlow == root.mFlow);
         REQUIRE(child1->mFlow.IsLocked() == false);
         REQUIRE(child1->mRefreshRequired == true);
         REQUIRE(child1->mChildren.GetCount() == 2);
         REQUIRE(child1->mUnitsList.GetCount() == 2);
         REQUIRE(child1->mTraits.GetCount() == 1);
         REQUIRE(child1->GetName() == "Child1");
         REQUIRE(child1->GetReferences() == 6);

         auto child2 = root.mChildren[1];
         REQUIRE(child2->mOwner == &root);
         REQUIRE(child2->mRuntime == root.mRuntime);
         REQUIRE(child2->mRuntime.IsLocked() == false);
         REQUIRE(child2->mFlow == root.mFlow);
         REQUIRE(child2->mFlow.IsLocked() == false);
         REQUIRE(child2->mRefreshRequired == true);
         REQUIRE(!child2->mChildren);
         REQUIRE(!child2->mUnitsList);
         REQUIRE(child2->mTraits.GetCount() == 1);
         REQUIRE(child2->GetName() == "Child2");
         REQUIRE(child2->GetReferences() == 2);

         auto grandchild1 = child1->mChildren[0];
         REQUIRE(grandchild1->mOwner == child1);
         REQUIRE(grandchild1->mRuntime == root.mRuntime);
         REQUIRE(grandchild1->mRuntime.IsLocked() == false);
         REQUIRE(grandchild1->mFlow == root.mFlow);
         REQUIRE(grandchild1->mFlow.IsLocked() == false);
         REQUIRE(grandchild1->mRefreshRequired == true);
         REQUIRE(!grandchild1->mChildren);
         REQUIRE(!grandchild1->mUnitsList);
         REQUIRE(grandchild1->mTraits.GetCount() == 1);
         REQUIRE(grandchild1->GetName() == "GrandChild1");
         REQUIRE(grandchild1->GetReferences() == 2);

         auto grandchild2 = child1->mChildren[1];
         REQUIRE(grandchild2->mOwner == child1);
         REQUIRE(grandchild2->mRuntime == root.mRuntime);
         REQUIRE(grandchild2->mRuntime.IsLocked() == false);
         REQUIRE(grandchild2->mFlow == root.mFlow);
         REQUIRE(grandchild2->mFlow.IsLocked() == false);
         REQUIRE(grandchild2->mRefreshRequired == true);
         REQUIRE(!grandchild2->mChildren);
         REQUIRE(!grandchild2->mUnitsList);
         REQUIRE(grandchild2->mTraits.GetCount() == 1);
         REQUIRE(grandchild2->GetName() == "GrandChild2");
         REQUIRE(grandchild2->GetReferences() == 2);
      }
      Logger::Special("End: Creating a Thing by descriptor");
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

         THEN("Properties should match") {
            auto child1 = root.mChildren[0];
            REQUIRE(child1 == child);
         }
      }

      WHEN("Getting a local child by name trait") {
         auto child = root.GetNamedChild("Child2");

         THEN("Properties should match") {
            auto child2 = root.mChildren[1];
            REQUIRE(child2 == child);
         }
      }

      WHEN("Getting a local child by index and name trait") {
         auto child = root.GetNamedChild("Child2", 1);

         THEN("Properties should match") {
            auto child3 = root.mChildren[2];
            REQUIRE(child3 == child);
         }
      }

      /*WHEN("IsFamilyOf is invoked") {
         Thing separateRoot;
         auto child1 = root.GetChild(0);
         auto child2 = child1->GetChild(0);

         THEN("Properties should match") {
            REQUIRE_FALSE(root.IsFamilyOf(separateRoot));
            REQUIRE(root.IsFamilyOf(*child1));
            REQUIRE(root.IsFamilyOf(*child2));
            REQUIRE(child1->IsFamilyOf(root));
            REQUIRE(child2->IsFamilyOf(root));
         }
      }*/

      WHEN("Get a local unit by index") {
         // Hash algorithm changes might swap the order of these two    
         auto unit0 = root.GetUnit(0);
         auto unit1 = root.GetUnit(1);

         THEN("Properties should match") { 
            REQUIRE(unit0);
            REQUIRE(unit0->Is<TestUnit1>());
            REQUIRE(unit0 == root.mUnitsAmbiguous[MetaOf<TestUnit1>()][0]);
            REQUIRE(unit1);
            REQUIRE(unit1->Is<TestUnit2>());
            REQUIRE(unit1 == root.mUnitsAmbiguous[MetaOf<TestUnit2>()][0]);
         }
      }

      WHEN("Get a local unit by type and index") {
         auto unit = root.GetUnitMeta(MetaOf<TestUnit1>());

         THEN("Properties should match") {
            REQUIRE(unit);
            REQUIRE(unit->Is<TestUnit1>());
            REQUIRE(unit == root.mUnitsAmbiguous[MetaOf<TestUnit1>()][0]);
         }
      }

      WHEN("Removing a specific local unit") {
         auto unitmemory = root.GetUnit(0);
         auto removed = root.RemoveUnit(root.GetUnit(0));

         THEN("Properties should match") {
            REQUIRE(removed == 1);
            REQUIRE(root.mUnitsList.GetCount() == 1);
            REQUIRE(nullptr == Allocator::Find(MetaOf<TestUnit1>(), unitmemory));
         }
      }

      WHEN("Removing all local units") {
         auto unitmemory1 = root.GetUnit(0);
         auto unitmemory2 = root.GetUnit(1);
         auto removed = root.RemoveUnits();

         THEN("Properties should match") {
            REQUIRE(removed == 2);
            REQUIRE(!root.mUnitsList);
            REQUIRE(nullptr == Allocator::Find(MetaOf<TestUnit1>(), unitmemory1));
            REQUIRE(nullptr == Allocator::Find(MetaOf<TestUnit2>(), unitmemory2));
         }
      }

      WHEN("Replace local units") {
         TestUnit2 replacement;
         auto replaced = root.ReplaceUnit(root.GetUnit<TestUnit1>(), &replacement);

         THEN("Properties should match") {
            REQUIRE(replaced == 1);
            REQUIRE(root.mUnitsList.GetCount() == 2);
            REQUIRE(root.mUnitsAmbiguous[MetaOf<TestUnit2>()].GetCount() == 2);
            REQUIRE(root.mUnitsAmbiguous[MetaOf<TestUnit2>()].Contains(&replacement));
         }
      }

      WHEN("Count available units") {
         auto found = root.HasUnits<TestUnit1>();

         THEN("Properties should match") {
            REQUIRE(found == 1);
         }
      }

      WHEN("Gather units of a specific type") {
         auto found1 = root.GatherUnits<TestUnit1>();
         auto found2 = root.GatherUnits();

         THEN("Properties should match") {
            REQUIRE(found1.GetCount() == 1);
            //REQUIRE(found2.GetCount() == 2);
         }
      }

      WHEN("Seek a unit by index") {
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
      }
   }
}