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

set(
    DECOF2_CLI_SRC
    cli_context_base.cpp
    lex.cpp
    parse.cpp
    parser.h
    parser.ih
    parser_includes.h
    clisrv_context.cpp
    encoder.cpp
    encoder.h
    pubsub_context.cpp
    update_container.cpp
    visitor.cpp
    visitor.h
    scanner.h
    scanner.ih
)

# Standard DeCoF2 CLI library
add_library(
    decof2-cli
    EXCLUDE_FROM_ALL
    ${DECOF2_CLI_SRC}
)
target_include_directories(
    decof2-cli
    PUBLIC
        ${Boost_INCLUDE_DIR}
        ${PROJECT_SOURCE_DIR}/include/
)
target_link_libraries(
    decof2-cli
    decof2-core
)

# DeCoF2 CLI library with coverage information
if(CMAKE_COMPILER_IS_GNUCXX)
	add_library(
		decof2-cli-gcov
		EXCLUDE_FROM_ALL
		${DECOF2_CLI_SRC}
	)
	target_include_directories(
		decof2-cli
		PUBLIC
			${Boost_INCLUDE_DIR}
            ${PROJECT_SOURCE_DIR}/include/
	)
	target_link_libraries(
		decof2-cli-gcov
		decof2-core-gcov
	)
endif()
