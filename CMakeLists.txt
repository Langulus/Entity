cmake_minimum_required(VERSION 3.22)
project(Langulus.Entity)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Check if this project is built as standalone, or a part of something else 
if (PROJECT_IS_TOP_LEVEL)
    if(LANGULUS_BUILD_SHARED_LIBRARIES)
        add_compile_definitions(LANGULUS_EXPORT_ALL)
        set(LANGULUS_LIBRARY_TYPE SHARED)
    else()
        set(LANGULUS_LIBRARY_TYPE STATIC)
    endif()

    include(LangulusUtilities.cmake)
    langulus_init_git_submodule(cmake)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
    include(FetchLangulusModule)
    include(HandleLangulusFeatures)

    handle_langulus_features()

	fetch_langulus_module(Core)
	fetch_langulus_module(Logger)
	fetch_langulus_module(RTTI)
	fetch_langulus_module(SIMD)
	fetch_langulus_module(Anyness)
	fetch_langulus_module(Flow)
	fetch_langulus_module(Math)

    add_compile_definitions(LANGULUS_ENABLE_TESTING)
    enable_testing()
	add_subdirectory(test)
endif()

# Build and install Entity library                                          
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/source LANGULUS_ENTITY_SOURCES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/source/modules LANGULUS_ENTITY_SOURCES)
add_library(Langulus.Entity ${LANGULUS_LIBRARY_TYPE} ${LANGULUS_ENTITY_SOURCES})

if (MSVC)
    target_compile_options(Langulus.Entity PRIVATE /bigobj)
endif()

target_include_directories(Langulus.Entity
    PUBLIC      include
)

target_link_libraries(Langulus.Entity
    PUBLIC      Langulus.Core
                Langulus.Logger
                Langulus.RTTI
                Langulus.SIMD
                Langulus.Anyness
                Langulus.Flow
                Langulus.Math
)