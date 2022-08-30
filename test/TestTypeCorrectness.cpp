#include "TestMain.hpp"
#include <catch2/catch.hpp>

TEMPLATE_TEST_CASE("Test PCFW number types", "[metadata]", RefreshRate) {
	GIVEN("An unsigned integer type") {
		auto meta = MetaData::Of<TestType>();
		REQUIRE(meta != nullptr);

		WHEN("Interpreted as another type") {
			THEN("Requirements should be met") {
				REQUIRE(meta->template InterpretsAs<ANumber>());
				REQUIRE(meta->template InterpretsAs<ANumber>(1));
				REQUIRE(!meta->template InterpretsAs<ANumber>(2));
			}
		}
	}
}
