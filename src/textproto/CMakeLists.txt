include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_library(
    decof2-textproto
    lex.cpp
    parse.cpp
    parser.h
    parser.ih
    parser_includes.h
    textproto_clisrv.cpp
    textproto_clisrv.h
    textproto_encoder.cpp
    textproto_encoder.h
    textproto_pubsub.cpp
    textproto_pubsub.h
    textproto_visitor.cpp
    textproto_visitor.h
    scanner.h
    scanner.ih
)
