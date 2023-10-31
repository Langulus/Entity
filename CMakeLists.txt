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
	fetch_langulus_module(Core    GIT_TAG d22b2ae2bae11deec5ec20e6baed4839fbf95aaa)
	fetch_langulus_module(Logger  GIT_TAG 3314fbe3bf5611a5bd9ee649f2a6ba5c43f6b4c4)
	fetch_langulus_module(RTTI    GIT_TAG b09fd3fbcea315c056b456ba49b582b3bdbdc1ea)
    if(LANGULUS_FEATURE_MANAGED_MEMORY)
        fetch_langulus_module(Fractalloc GIT_TAG 0126c1750d9101ab29227a735d2a7a06da672e02)
    endif()
	fetch_langulus_module(SIMD    GIT_TAG 4aadc7d243c10a09e8095bf502909931b6ec6122)
	fetch_langulus_module(Anyness GIT_TAG e18ccf59a00ab10409ad3a8f96c4c6b3471d4083)
    fetch_langulus_module(Flow	  GIT_TAG 304a4586d6650bb4454c4ae4f8964d30ff82cf66)
	fetch_langulus_module(Math	  GIT_TAG 20a9ff52989117c62715312cdcc9e4cbf79a516a)
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

target_include_directories(LangulusEntity PUBLIC include
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