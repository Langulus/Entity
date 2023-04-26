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

int main(int argc, char* argv[]) {
	static_assert(CT::Complete<Hierarchy>, "Oops1");
	static_assert(CT::Complete<Unit>, "Oops2");
	static_assert(CT::Complete<Temporal>, "Oops3");
	static_assert(CT::Complete<Thing>, "Oops4");
	static_assert(CT::Complete<Runtime>, "Oops5");

	// For reflection of all verbs													
	(void)MetaData::Of<TAny<Thing*>>();
	(void)MetaData::Of<Hierarchy>();
	(void)MetaData::Of<Index>();
	(void)MetaData::Of<Thing>();

	// For reflection of all verbs													
	(void)MetaVerb::Of<Verbs::Add>();
	(void)MetaVerb::Of<Verbs::Associate>();
	(void)MetaVerb::Of<Verbs::Catenate>();
	(void)MetaVerb::Of<Verbs::Conjunct>();
	(void)MetaVerb::Of<Verbs::Create>();
	(void)MetaVerb::Of<Verbs::Do>();
	(void)MetaVerb::Of<Verbs::Exponent>();
	(void)MetaVerb::Of<Verbs::Interpret>();
	(void)MetaVerb::Of<Verbs::Multiply>();
	(void)MetaVerb::Of<Verbs::Select>();

	Catch::Session session;
	return session.run(argc, argv);
}
