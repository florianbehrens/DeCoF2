# Download and install the Beast library
include(ExternalProject)

ExternalProject_Add(
    decof2-libbeast
    URL "https://github.com/vinniefalco/Beast/archive/9f10b11eff58aeb793b673c8a8cb6e2bee3db621.tar.gz"
    URL_MD5 "d8ddbf540e60b346eb3a02881ddbf6cb"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(decof2-libbeast SOURCE_DIR)

set(Beast_INCLUDE_DIR ${SOURCE_DIR}/include)

set(
    DECOF2_WEBSOCKET_SRC
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
        ${Boost_INCLUDE_DIR}
        ${Beast_INCLUDE_DIR}
        ${PROJECT_SOURCE_DIR}/include/
)
target_link_libraries(
    decof2-websocket
    decof2-core
)
add_dependencies(
    decof2-websocket
    decof2-libbeast
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
            ${Boost_INCLUDE_DIR}
            ${Beast_INCLUDE_DIR}
            ${PROJECT_SOURCE_DIR}/include/
    )
    target_link_libraries(
        decof2-websocket-gcov
        decof2-core-gcov
    )
endif()
