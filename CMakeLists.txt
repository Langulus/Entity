cmake_minimum_required(VERSION 3.22)
include(cmake/FetchLangulusModule.cmake)
project(Langulus.Entity)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configure the features you require, all these are disabled by default
option(LANGULUS_ENABLE_SAFE_MODE 
    "Overrides additional error checking and sanity checks, \
    incurs a serious runtime overhead")

option(LANGULUS_ENABLE_PARANOIA 
    "Make sure that any unused memory is nullified, \
    incurs a serious runtime overhead")

option(LANGULUS_ENABLE_DEBUGGING 
    "Explicitly define debug mode, in the case that default flags are \
    not detected by some odd reason")

set(LANGULUS_FEATURE_MANAGED_MEMORY ON CACHE BOOL "Managed memory feature")
set(LANGULUS_FEATURE_MANAGED_REFLECTION ON CACHE BOOL "Managed reflection feature")

if(LANGULUS_ENABLE_SAFE_MODE)
    add_compile_definitions(LANGULUS_ENABLE_SAFE_MODE)
endif()
if(LANGULUS_ENABLE_PARANOIA)
    add_compile_definitions(LANGULUS_ENABLE_PARANOIA)
endif()
if(LANGULUS_ENABLE_DEBUGGING)
    add_compile_definitions(LANGULUS_ENABLE_DEBUGGING)
endif()
if(LANGULUS_FEATURE_MANAGED_MEMORY)
    add_compile_definitions(LANGULUS_ENABLE_FEATURE_MANAGED_MEMORY)
endif()
if(LANGULUS_FEATURE_MANAGED_REFLECTION)
    add_compile_definitions(LANGULUS_ENABLE_FEATURE_MANAGED_REFLECTION)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	message(STATUS "Debug mode, all assertions are enabled")
	add_compile_definitions(LANGULUS_ENABLE_ASSERTION_LEVEL=1000)
    add_compile_definitions(LANGULUS_ENABLE_FEATURE_MEMORY_STATISTICS)
    add_compile_definitions(LANGULUS_ENABLE_DEBUGGING)
endif()

# Check if this project is built as standalone, or a part of something else
if (PROJECT_IS_TOP_LEVEL)
	fetch_langulus_module(Core)
	fetch_langulus_module(Logger)
	fetch_langulus_module(RTTI)
	fetch_langulus_module(Anyness)
	fetch_langulus_module(Flow)

    add_compile_definitions(LANGULUS_ENABLE_TESTING)
    enable_testing()
	add_subdirectory(test)
endif()

# Build and install Entity library
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/source SOURCE_FILES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/source/modules SOURCE_FILES)
add_library(Langulus.Entity ${SOURCE_FILES})

if (MSVC)
    target_compile_options(Langulus.Entity PRIVATE /utf-8 /bigobj)
endif()

target_include_directories(Langulus.Entity PUBLIC include)
target_link_libraries(Langulus.Entity PUBLIC Langulus.Flow)