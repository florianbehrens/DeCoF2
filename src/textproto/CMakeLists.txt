# Flexc++ scanner generation
add_custom_command(
    OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/scannerbase.h" "${CMAKE_CURRENT_SOURCE_DIR}/lex.cpp"
    COMMAND flexc++ ARGS scanner.flexc++
    DEPENDS scanner.flexc++
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Bisonc++ parser generation
add_custom_command(
    OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/parserbase.h" "${CMAKE_CURRENT_SOURCE_DIR}/parse.cpp"
    COMMAND bisonc++ ARGS grammar.bisonc++
    DEPENDS grammar.bisonc++
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

add_library(
    decof2-textproto
    lex.cpp
    parse.cpp
    string_encoder.cpp
    textproto_clisrv.cpp
    textproto_pubsub.cpp
    textproto_visitor.cpp
    scanner.flexc++
    grammar.bisonc++
)
