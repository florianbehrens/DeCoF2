cmake_minimum_required(VERSION 3.0.2)
project(decof2)

option(DECOF2_COMPILE_WITH_COVERAGE "Compile with coverage instrumentation" OFF)

if(NOT (Boost_FOUND AND Boost_VERSION VERSION_GREATER 1.58))
    find_package(Boost 1.59 COMPONENTS system thread unit_test_framework)
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

# Set C++17 as language standard
set(CMAKE_CXX_STANDARD 17)

# Remove Visual Studio 'checked iterators' and 'runtime security' warnings
if(MSVC)
    add_definitions(-D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS)

    # warning C4996: 'std::allocator<void>': warning STL4009: std::allocator<void> is deprecated in C++17
    add_definitions(-D_SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING)
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

if(${Boost_UNIT_TEST_FRAMEWORK_FOUND})
    add_subdirectory(test)
endif()

if(${Boost_THREAD_FOUND})
    add_subdirectory(example)
endif()
