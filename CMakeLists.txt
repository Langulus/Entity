cmake_minimum_required(VERSION 3.28)
project(LangulusEntity
    VERSION         1.0.0
    DESCRIPTION     "Langulus entity component system library"
    HOMEPAGE_URL    https://langulus.com
)

# Check if this project is built as standalone, or a part of something else		
if (PROJECT_IS_TOP_LEVEL OR NOT LANGULUS)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
    include(LangulusUtilities.cmake)
	fetch_langulus_module(Math GIT_TAG e3460ab98514d9d3e10fbf4d827dbb4af0e7426d)
endif()

file(GLOB_RECURSE
	LANGULUS_ENTITY_SOURCES 
	LIST_DIRECTORIES FALSE CONFIGURE_DEPENDS
	source/*.cpp
)

# Build and install Entity library												
add_langulus_library(LangulusEntity
    $<TARGET_OBJECTS:LangulusLogger>
    $<TARGET_OBJECTS:LangulusRTTI>
    $<$<BOOL:${LANGULUS_FEATURE_MANAGED_MEMORY}>:$<TARGET_OBJECTS:LangulusFractalloc>>
    $<TARGET_OBJECTS:LangulusAnyness>
    $<TARGET_OBJECTS:LangulusFlow>
    $<TARGET_OBJECTS:LangulusMath>
	${LANGULUS_ENTITY_SOURCES}
)

target_include_directories(LangulusEntity 
	PUBLIC		$<TARGET_PROPERTY:LangulusLogger,INTERFACE_INCLUDE_DIRECTORIES>
				$<TARGET_PROPERTY:LangulusRTTI,INTERFACE_INCLUDE_DIRECTORIES>
				$<$<BOOL:${LANGULUS_FEATURE_MANAGED_MEMORY}>:$<TARGET_PROPERTY:LangulusFractalloc,INTERFACE_INCLUDE_DIRECTORIES>>
				$<TARGET_PROPERTY:LangulusAnyness,INTERFACE_INCLUDE_DIRECTORIES>
				$<TARGET_PROPERTY:LangulusFlow,INTERFACE_INCLUDE_DIRECTORIES>
				$<TARGET_PROPERTY:LangulusMath,INTERFACE_INCLUDE_DIRECTORIES>
				include
)

target_link_libraries(LangulusEntity
    PUBLIC      LangulusCore
                fmt
	PRIVATE     $<$<NOT:$<BOOL:${MSVC}>>:dl>
)

target_compile_definitions(LangulusEntity
    PRIVATE     LANGULUS_EXPORT_ALL
)

if(LANGULUS_TESTING)
	enable_testing()
	add_subdirectory(test)
endif()