#include "TestMain.hpp"

PC_MONOPOLIZE_MEMORY(0)

#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>

#define DUMP_STUFF \
	pcLogSpecial << gasm; \
	pcLogSpecial << parsed

/// Start																							
int main(int argc, char* argv[]) {
	pcFrameworkInit(0);
	Catch::Session session;
	int result = session.run(argc, argv);
	return result;
}

///																									
SCENARIO("Static PCFW checks", "[staticchecks]") {
	GIVEN("A freshly initialized framework") {
		auto meta1 = DataID::Reflect<int>();
		auto meta2 = DataID::Reflect<int*>();
		auto meta3 = DataID::Reflect<const int*>();
		auto meta4 = DataID::Reflect<int&>();
		auto meta5 = DataID::Reflect<const int&>();
		auto meta6 = DataID::Reflect<int* const>();

		auto meta7 = DataID::Reflect<Text>();
		auto meta8 = DataID::Reflect<Text*>();
		auto meta9 = DataID::Reflect<const Text*>();
		auto meta10 = DataID::Reflect<Text&>();
		auto meta11 = DataID::Reflect<const Text&>();
		auto meta12 = DataID::Reflect<Text* const>();

		REQUIRE(meta1 != nullptr);
		REQUIRE(meta2 != nullptr);
		REQUIRE(meta3 != nullptr);
		REQUIRE(meta4 != nullptr);
		REQUIRE(meta5 != nullptr);
		REQUIRE(meta6 != nullptr);
		REQUIRE(meta1 == meta4);
		REQUIRE(meta1 == meta5);
		REQUIRE(meta2 == meta6);

		REQUIRE(meta7 != nullptr);
		REQUIRE(meta8 != nullptr);
		REQUIRE(meta9 != nullptr);
		REQUIRE(meta10 != nullptr);
		REQUIRE(meta11 != nullptr);
		REQUIRE(meta12 != nullptr);

		REQUIRE(meta7 == meta10);
		REQUIRE(meta7 == meta11);
		REQUIRE(meta8 == meta12);

		REQUIRE(DataID::Of<Verb> == PCMEMORY.GetMetaData("Verb")->GetID());
	}
}

SCENARIO("Parsing GASM", "[gasm]") {
	GIVEN("1) The GASM script: .Entity(Verb).(>? as Context)") {
		const GASM gasm = ".Entity(Verb).(>? as Context)";
		Any required = Verb::From<Verbs::Select>(
			Verb::From<Verbs::Select>({}, Construct::From<Entity>(DataID::Of<Verb>)),
			Any().MakeMissing().MakeRight(),
			Traits::Context::ID
		);

		WHEN("Parsed") {
			const auto parsed = gasm.Parse();
			DUMP_STUFF;
			THEN("The parsed contents must match the requirements") {
				REQUIRE(parsed == required);
			}
		}
	}
}