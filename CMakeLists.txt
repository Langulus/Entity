add_subdirectory(Test)

project(PCFramework)

add_definitions(-DPC_EXPORT_PCFW)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SOURCE_FILES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Containers SOURCE_FILES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Hierarchy SOURCE_FILES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Flow SOURCE_FILES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Utilities SOURCE_FILES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Modules SOURCE_FILES)

add_library(PCFramework SHARED ${SOURCE_FILES})

target_include_directories(PCFramework
	PRIVATE		${CMAKE_CURRENT_SOURCE_DIR}
	INTERFACE	${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(NOT PC_OS STREQUAL "Win")
	set(PC_ADDITIONAL_PCFW_RUNTIMES dl 
		CACHE STRING "Libraries to link against if not building PCFW on Windows")
endif()

target_link_libraries(PCFramework 
	PUBLIC PCMath PCMemory Flow
	PRIVATE ${PC_ADDITIONAL_PCFW_RUNTIMES}
)

install(TARGETS PCFramework DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})