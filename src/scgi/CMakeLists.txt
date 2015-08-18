set(
    DECOF2_SCGI_SRC
    array_view.h
    bencode_string_parser.cpp
    bencode_string_parser.h
    context.cpp
    context.h
    endian.h
    js_value_encoder.cpp
    js_value_encoder.h
    response.cpp
    response.h
    request_parser.cpp
    request_parser.h
    xml_visitor.cpp
    xml_visitor.h
)

# Standard DeCoF2 SCGI library
add_library(
    decof2-scgi ${DECOF2_LIBRARY_TYPE}
    EXCLUDE_FROM_ALL
    ${DECOF2_SCGI_SRC}
)
target_include_directories(
    decof2-scgi
    PUBLIC
        ${Boost_INCLUDE_DIR}
        ../../include
        ../../include/decof
    PRIVATE
        ../core
)
target_link_libraries(
    decof2-scgi
    decof2-core
)

# DeCoF2 SCGI library with covergae information
add_library(
    decof2-scgi-gcov
    EXCLUDE_FROM_ALL
    ${DECOF2_SCGI_SRC}
)
target_include_directories(
    decof2-scgi-gcov
    PUBLIC
        ${Boost_INCLUDE_DIR}
        ../../include
        ../../include/decof
    PRIVATE
        ../core
)
target_link_libraries(
    decof2-scgi-gcov
    decof2-core-gcov
)
