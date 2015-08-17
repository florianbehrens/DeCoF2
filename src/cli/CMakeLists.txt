include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
set(
    DECOF2_CLI_SRC
    lex.cpp
    parse.cpp
    parser.h
    parser.ih
    parser_includes.h
    clisrv_context.cpp
    clisrv_context.h
    encoder.cpp
    encoder.h
    pubsub_context.cpp
    pubsub_context.h
    visitor.cpp
    visitor.h
    scanner.h
    scanner.ih
)

# Standard DeCoF2 CLI library
add_library(
    decof2-cli ${DECOF2_LIBRARY_TYPE}
    EXCLUDE_FROM_ALL
    ${DECOF2_CLI_SRC}
)
target_link_libraries(
    decof2-cli
    decof2-core
)

# DeCoF2 CLI library with coverage information
add_library(
    decof2-cli-gcov
    EXCLUDE_FROM_ALL
    ${DECOF2_CLI_SRC}
)
target_link_libraries(
    decof2-cli-gcov
    decof2-core-gcov
)
