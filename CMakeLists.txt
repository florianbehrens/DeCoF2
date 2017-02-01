cmake_minimum_required(VERSION 3.0.2)
project(decof2)

if(NOT (Boost_FOUND AND Boost_VERSION VERSION_GREATER 1.55))
    find_package(Boost 1.56 REQUIRED COMPONENTS system unit_test_framework)
    link_directories(${Boost_LIBRARY_DIR})
endif()

# Doxygen API documentation generation
if(NOT DOXYGEN_FOUND)
    find_package(Doxygen)
endif()

if(DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/doxyfile @ONLY)
    add_custom_target(
        decof2-doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif(DOXYGEN_FOUND)

# Set C++11 as language standard
set(CMAKE_CXX_STANDARD 11)

# Remove Visual Studio 'checked iterators' and 'runtime security' warnings
if(MSVC)
    add_definitions(-D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS)
endif()

add_custom_target(
    decof2-other-files
    SOURCES
        README.md
        doxyfile.in
        LICENSE
        appveyor.yml
        .travis.yml
        .gitignore
)

add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(example)
