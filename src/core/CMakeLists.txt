add_library(
    decof2-microcore
    EXCLUDE_FROM_ALL
    basic_client_context.cpp
    event.cpp
    exceptions.cpp
    handler_event.cpp
    node.cpp
    object.cpp
    object_dictionary.cpp
    object_visitor.cpp
)

target_include_directories(
    decof2-microcore
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include/
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include/decof/
)

target_compile_options(decof2-microcore PUBLIC $<IF:$<STREQUAL:${DECOF2_COMPILE_WITH_COVERAGE},ON>,-fprofile-arcs -ftest-coverage,>)

if(${Boost_FOUND})
    add_library(
        decof2-core
        EXCLUDE_FROM_ALL
        client_context.cpp
    )

    target_include_directories(
        decof2-core
        PUBLIC
            ${Boost_INCLUDE_DIR}
    )

    target_link_libraries(decof2-core decof2-microcore ${Boost_SYSTEM_LIBRARY})
endif()
