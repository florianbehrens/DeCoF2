cmake_minimum_required(VERSION 2.8.12)
project(decof2)

find_package(Boost 1.49 REQUIRED COMPONENTS system unit_test_framework)

# Doxygen API documentation generation
find_package(Doxygen)
if(DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/doxyfile @ONLY)
    add_custom_target(
        doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif(DOXYGEN_FOUND)

# Use DECOF2_LIBRARY_TYPE to control library type, i.e., static or 
# dynamic.
if(NOT DEFINED DECOF2_LIBRARY_TYPE)
    set(DECOF2_LIBRARY_TYPE "SHARED")
endif()

add_definitions(-std=c++0x -DBOOST_ASIO_SEPARATE_COMPILATION)
include_directories(include include/decof src/core)
add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(example)
add_custom_target(
    decof2-readme
    SOURCES
        README.md
)
