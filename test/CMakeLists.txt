project(PCTestFramework)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SOURCE_FILES)

add_executable(PCTestFramework ${SOURCE_FILES}) 
target_link_libraries(PCTestFramework PRIVATE PCMemory PCMath PCFramework Catch2)

install(TARGETS PCTestFramework DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})