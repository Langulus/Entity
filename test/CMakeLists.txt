project(Langulus_Test_Entity)

# Configure Catch2 library
download_project(
	PROJ                Catch2
	GIT_REPOSITORY      https://github.com/catchorg/Catch2.git
	GIT_TAG             v2.x
	UPDATE_DISCONNECTED 1
)
add_subdirectory(${Catch2_SOURCE_DIR} ${Catch2_BINARY_DIR})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} LANGULUS_ENTITY_TEST_SOURCES)
add_executable(Test.Entity ${LANGULUS_ENTITY_TEST_SOURCES})

target_link_libraries(Test.Entity
	PRIVATE		Langulus.Entity
				Catch2
)

if (MSVC)
    target_compile_options(Test.Entity PRIVATE /bigobj)
endif()

add_test(NAME Test.Entity COMMAND Test.Entity)
