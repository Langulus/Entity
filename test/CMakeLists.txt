file(GLOB_RECURSE
	LANGULUS_ENTITY_TEST_SOURCES 
	LIST_DIRECTORIES FALSE CONFIGURE_DEPENDS
	*.cpp
)

add_langulus_test(LangulusEntityTest
	SOURCES		${LANGULUS_ENTITY_TEST_SOURCES}
	LIBRARIES	LangulusEntity
)