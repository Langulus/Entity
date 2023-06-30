#include "Main.hpp"

#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>

LANGULUS_RTTI_BOUNDARY("MAIN");

int main(int argc, char* argv[]) {
	static_assert(CT::Complete<Hierarchy>, "Oops1");
	static_assert(CT::Complete<Unit>, "Oops2");
	static_assert(CT::Complete<Temporal>, "Oops3");
	static_assert(CT::Complete<Thing>, "Oops4");
	static_assert(CT::Complete<Runtime>, "Oops5");

	// For reflection of all verbs													
	(void)MetaOf<TAny<Thing*>>();
	(void)MetaOf<Hierarchy>();
	(void)MetaOf<Index>();
	(void)MetaOf<Thing>();

	Catch::Session session;
	return session.run(argc, argv);
}
