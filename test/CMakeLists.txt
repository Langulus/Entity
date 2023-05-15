project(Langulus::Entity::Test)

add_executable(LangulusEntityTest 
	Main.cpp
	TestComposition.cpp
	TestRuntime.cpp
	TestThing.cpp
)

target_link_libraries(LangulusEntityTest
	PRIVATE		LangulusEntity
				Catch2
)

add_test(
	NAME		Langulus::Entity::Test
	COMMAND		LangulusEntityTest
)

langulus_copy_dlls(LangulusEntityTest ON LangulusEntityTest)
