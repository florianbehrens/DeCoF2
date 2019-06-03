if(MSVC)
    # Enable alternative boolean operators with Visual Studio compiler
    set_source_files_properties(
        parse.cpp
        PROPERTIES
            COMPILE_FLAGS "/Za"
    )
    # Enable alternative boolean operators with Visual Studio compiler
    # Disable C4267: 'argument': conversion from 'size_t' to 'const unsigned char', possible loss of data
    set_source_files_properties(
        lex.cpp
        PROPERTIES
            COMPILE_FLAGS "/Za /wd4267"
    )
endif()

# Standard DeCoF2 CLI library
add_library(
    decof2-cli
    EXCLUDE_FROM_ALL
    browse_visitor.cpp
    browse_visitor.h
    cli_context_base.cpp
    clisrv_context.cpp
    decoder.cpp
    decoder.h
    encoder.cpp
    encoder.h
    grammar.bisonc++
    lex.cpp
    parse.cpp
    parser.h
    parser.ih
    parser_includes.h
    parserbase.h
    pubsub_context.cpp
    scanner.flexc++
    scanner.h
    scanner.ih
    tree_visitor.cpp
    tree_visitor.h
    update_container.cpp
)

target_link_libraries(decof2-cli decof2-core)
