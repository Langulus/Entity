#include "Main.hpp"
#include <Flow/Verbs/Add.hpp>
#include <Flow/Verbs/Associate.hpp>
#include <Flow/Verbs/Catenate.hpp>
#include <Flow/Verbs/Conjunct.hpp>
#include <Flow/Verbs/Create.hpp>
#include <Flow/Verbs/Do.hpp>
#include <Flow/Verbs/Exponent.hpp>
#include <Flow/Verbs/Interpret.hpp>
#include <Flow/Verbs/Multiply.hpp>
#include <Flow/Verbs/Select.hpp>

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

	// For reflection of all verbs													
	(void)MetaOf<Verbs::Add>();
	(void)MetaOf<Verbs::Associate>();
	(void)MetaOf<Verbs::Catenate>();
	(void)MetaOf<Verbs::Conjunct>();
	(void)MetaOf<Verbs::Create>();
	(void)MetaOf<Verbs::Do>();
	(void)MetaOf<Verbs::Exponent>();
	(void)MetaOf<Verbs::Interpret>();
	(void)MetaOf<Verbs::Multiply>();
	(void)MetaOf<Verbs::Select>();

	Catch::Session session;
	return session.run(argc, argv);
}
