include(ExternalProject)

# Download and install the RapidJSON library
ExternalProject_Add(
    decof2-rapidjson
    URL "https://github.com/miloyip/rapidjson/archive/v1.1.0.tar.gz"
    URL_MD5 "badd12c511e081fec6c89c43a7027bce"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(decof2-rapidjson SOURCE_DIR)

set(RapidJSON_INCLUDE_DIR ${SOURCE_DIR}/include)

set(
    DECOF2_WEBSOCKET_SRC
    error.cpp
    error.h
    json_value_encoder.cpp
    json_value_encoder.h
    request.cpp
    request.h
    response.cpp
    response.h
    websocket_context.cpp
)

# Standard DeCoF2 WebSocket library
add_library(
    decof2-websocket
    EXCLUDE_FROM_ALL
    ${DECOF2_WEBSOCKET_SRC}
)
target_include_directories(
    decof2-websocket
    PUBLIC
        ${RapidJSON_INCLUDE_DIR}
        ${PROJECT_SOURCE_DIR}/include/
)
target_link_libraries(
    decof2-websocket
    decof2-core
)
add_dependencies(
    decof2-websocket
    decof2-rapidjson
)

# DeCoF2 WebSocket library with coverage information
if(CMAKE_COMPILER_IS_GNUCXX)
    add_library(
        decof2-websocket-gcov
        EXCLUDE_FROM_ALL
        ${DECOF2_WEBSOCKET_SRC}
    )
    target_include_directories(
        decof2-websocket-gcov
        PUBLIC
            ${RapidJSON_INCLUDE_DIR}
            ${PROJECT_SOURCE_DIR}/include/
    )
    target_link_libraries(
        decof2-websocket-gcov
        decof2-core-gcov
    )
    add_dependencies(
        decof2-websocket-gcov
        decof2-rapidjson
    )
    target_compile_options(
        decof2-websocket-gcov
        PUBLIC
            -fprofile-arcs
            -ftest-coverage
    )
endif()
