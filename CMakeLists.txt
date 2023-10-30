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
	fetch_langulus_module(Core    GIT_TAG d52feac3e995e3eeac44fadd7812361919b73a11)
	fetch_langulus_module(Logger  GIT_TAG 3314fbe3bf5611a5bd9ee649f2a6ba5c43f6b4c4)
	fetch_langulus_module(RTTI    GIT_TAG 8c735514612820f76da494421baccaaea0d4d5e1)
    if(LANGULUS_FEATURE_MANAGED_MEMORY)
        fetch_langulus_module(Fractalloc GIT_TAG 0126c1750d9101ab29227a735d2a7a06da672e02)
    endif()
	fetch_langulus_module(SIMD    GIT_TAG 4aadc7d243c10a09e8095bf502909931b6ec6122)
	fetch_langulus_module(Anyness GIT_TAG 2922d043c686826d39e710ab7be5358b47c1f66f)
    fetch_langulus_module(Flow	  GIT_TAG 9c3f62a53f3e0c2b9d0c684e3f08aad86b925f68)
	fetch_langulus_module(Math	  GIT_TAG d0ded7b53cd07e85b032d667a79db5d215f2e619)
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