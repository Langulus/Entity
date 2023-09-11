cmake_minimum_required(VERSION 3.24)
project(LangulusEntity
    VERSION         1.0.0
    DESCRIPTION     "Langulus entity component system library"
    HOMEPAGE_URL    https://langulus.com
)


# Check if this project is built as standalone, or a part of something else 
if(PROJECT_IS_TOP_LEVEL OR NOT LANGULUS)
    include(LangulusUtilities.cmake)

    # Add all Langulus libraries                                            
	fetch_langulus_module(Core)
	fetch_langulus_module(Logger)
	fetch_langulus_module(RTTI)
    if(LANGULUS_FEATURE_MANAGED_MEMORY)
        fetch_langulus_module(Fractalloc)
    endif()
	fetch_langulus_module(SIMD)
	fetch_langulus_module(Anyness)
	fetch_langulus_module(Flow)
	fetch_langulus_module(Math)
endif()

# Build and install Entity library                                          
add_library(LangulusEntity ${LANGULUS_LIBRARY_TYPE}
    $<TARGET_OBJECTS:LangulusLogger>
    $<TARGET_OBJECTS:LangulusRTTI>
    $<$<BOOL:${LANGULUS_FEATURE_MANAGED_MEMORY}>:$<TARGET_OBJECTS:LangulusFractalloc>>
    $<TARGET_OBJECTS:LangulusAnyness>
    $<TARGET_OBJECTS:LangulusFlow>
    source/Thing.cpp
    source/Thing-Traits.cpp
    source/Thing-Verbs.cpp
    source/Runtime.cpp
    source/Unit.cpp
)

target_include_directories(LangulusEntity
    PUBLIC      include
                $<TARGET_PROPERTY:LangulusLogger,INTERFACE_INCLUDE_DIRECTORIES>
                $<TARGET_PROPERTY:LangulusRTTI,INTERFACE_INCLUDE_DIRECTORIES>
                $<$<BOOL:${LANGULUS_FEATURE_MANAGED_MEMORY}>:$<TARGET_PROPERTY:LangulusFractalloc,INTERFACE_INCLUDE_DIRECTORIES>>
                $<TARGET_PROPERTY:LangulusAnyness,INTERFACE_INCLUDE_DIRECTORIES>
                $<TARGET_PROPERTY:LangulusFlow,INTERFACE_INCLUDE_DIRECTORIES>
                $<TARGET_PROPERTY:LangulusMath,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(LangulusEntity
    PUBLIC      LangulusCore
                fmt
)

if(NOT MSVC)
    target_link_libraries(LangulusEntity PRIVATE dl)
endif()

target_compile_definitions(LangulusEntity
    PRIVATE     LANGULUS_EXPORT_ALL
)

if(LANGULUS_TESTING)
    enable_testing()
	add_subdirectory(test)
endif()