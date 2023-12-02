cmake_minimum_required(VERSION 3.24)
project(LangulusEntity
    VERSION         1.0.0
    DESCRIPTION     "Langulus entity component system library"
    HOMEPAGE_URL    https://langulus.com
)

# Check if this project is built as standalone, or a part of something else 
if(PROJECT_IS_TOP_LEVEL OR NOT LANGULUS)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

    include(LangulusUtilities.cmake)

    # Add all Langulus libraries                                            
	fetch_langulus_module(Core    GIT_TAG f1e134132b9dc3fbc8604d428a4d702c1405c256)
	fetch_langulus_module(Logger  GIT_TAG 758350aae22de77eac65152d2e4b3ca74fb5d5ad)
	fetch_langulus_module(RTTI    GIT_TAG 3ff4a909d62be9891534fb4943d793fe62973ffc)
    if(LANGULUS_FEATURE_MANAGED_MEMORY)
        fetch_langulus_module(Fractalloc GIT_TAG f367cb2f00f776ddda42cec256810e685f78d85d)
    endif()
	fetch_langulus_module(SIMD    GIT_TAG 5db6bf520139acee5692e053c340e0904a89e05c)
	fetch_langulus_module(Anyness GIT_TAG 385514b4b55fb95a92c592e08854b6ececc98c7c)
    fetch_langulus_module(Flow	  GIT_TAG c2cffd20cdba81f6d2d6ce7bb29b128afe8d6115)
	fetch_langulus_module(Math	  GIT_TAG d091d0e2e44eef7fe5d2bc1dc9b237d9d5bc79c9)
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