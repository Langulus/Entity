file(GLOB_RECURSE
	LANGULUS_ENTITY_TEST_SOURCES 
	LIST_DIRECTORIES FALSE CONFIGURE_DEPENDS
	*.cpp
)

add_executable(LangulusEntityTest ${LANGULUS_ENTITY_TEST_SOURCES})

target_link_libraries(LangulusEntityTest
	PRIVATE		LangulusEntity
				Catch2
)

add_test(
	NAME		LangulusEntityTest
	COMMAND		LangulusEntityTest
)