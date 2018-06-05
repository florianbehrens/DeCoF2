include(ExternalProject)

# Download and install the Beast library
ExternalProject_Add(
    decof2-libbeast
    URL "https://github.com/boostorg/beast/archive/3777cb8b9c78748d0e64401316f00382d37b83be.tar.gz"
    URL_MD5 "435f2a956c4e6f33e054972faaaa2168"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(decof2-libbeast SOURCE_DIR)

set(Beast_INCLUDE_DIR ${SOURCE_DIR}/include)

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
        ${Beast_INCLUDE_DIR}
        ${RapidJSON_INCLUDE_DIR}
        ${PROJECT_SOURCE_DIR}/include/
)
target_link_libraries(
    decof2-websocket
    decof2-core
)
add_dependencies(
    decof2-websocket
    decof2-libbeast
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
            ${Beast_INCLUDE_DIR}
            ${RapidJSON_INCLUDE_DIR}
            ${PROJECT_SOURCE_DIR}/include/
    )
    target_link_libraries(
        decof2-websocket-gcov
        decof2-core-gcov
    )
    add_dependencies(
        decof2-websocket-gcov
        decof2-libbeast
        decof2-rapidjson
    )
    target_compile_options(
        decof2-websocket-gcov
        PUBLIC
            -fprofile-arcs
            -ftest-coverage
    )
endif()
