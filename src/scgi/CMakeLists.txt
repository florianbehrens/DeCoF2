# Standard DeCoF2 SCGI library
add_library(
    decof2-scgi
    EXCLUDE_FROM_ALL
    array_view.h
    bencode_string_parser.cpp
    bencode_string_parser.h
    scgi_context.cpp
    endian.h
    js_value_encoder.cpp
    js_value_encoder.h
    response.cpp
    request_parser.cpp
    xml_visitor.cpp
    xml_visitor.h
)

target_include_directories(
    decof2-scgi
    PUBLIC
        ${Boost_INCLUDE_DIR}
        ${PROJECT_SOURCE_DIR}/include/
)

target_link_libraries(decof2-scgi decof2-core)
