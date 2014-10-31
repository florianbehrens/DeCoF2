# Flexc++ scanner generation
add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/scannerbase.h" "${CMAKE_CURRENT_BINARY_DIR}/lex.cpp"
    COMMAND cp "${CMAKE_CURRENT_SOURCE_DIR}/scanner.h" "${CMAKE_CURRENT_BINARY_DIR}"
    COMMAND cp "${CMAKE_CURRENT_SOURCE_DIR}/scanner.ih" "${CMAKE_CURRENT_BINARY_DIR}"
    COMMAND flexc++ ARGS --no-lines "${CMAKE_CURRENT_SOURCE_DIR}/scanner.flexc++"
    DEPENDS scanner.flexc++
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)

# Bisonc++ parser generation
add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/parserbase.h" "${CMAKE_CURRENT_BINARY_DIR}/parse.cpp"
    COMMAND cp "${CMAKE_CURRENT_SOURCE_DIR}/parser.h" "${CMAKE_CURRENT_BINARY_DIR}"
    COMMAND cp "${CMAKE_CURRENT_SOURCE_DIR}/parser.ih" "${CMAKE_CURRENT_BINARY_DIR}"
    COMMAND bisonc++ ARGS "${CMAKE_CURRENT_SOURCE_DIR}/grammar.bisonc++"
    DEPENDS grammar.bisonc++
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_library(
    decof2-textproto
    "${CMAKE_CURRENT_BINARY_DIR}/lex.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/parse.cpp"
    parser.h
    parser.ih
    parser_includes.h
    string_encoder.cpp
    string_encoder.h
    textproto_clisrv.cpp
    textproto_clisrv.h
    textproto_pubsub.cpp
    textproto_pubsub.h
    textproto_visitor.cpp
    textproto_visitor.h
    scanner.flexc++
    scanner.h
    scanner.ih
    grammar.bisonc++
)
