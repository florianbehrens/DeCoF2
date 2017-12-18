set(
    DECOF2_ASIO_EXECUTOR_SRC
    asio_executor.cpp
)

# Default Asio executor library
add_library(
    decof2-asio-executor
    EXCLUDE_FROM_ALL
    ${DECOF2_ASIO_EXECUTOR_SRC}
)
target_include_directories(
    decof2-asio-executor
    PUBLIC
        ${Boost_INCLUDE_DIR}
        ${PROJECT_SOURCE_DIR}/include/
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include/decof/asio_executor
)
target_link_libraries(
    decof2-asio-executor
    decof2-core
)

# Default Asio executor library with coverage information
if(CMAKE_COMPILER_IS_GNUCXX)
    add_library(
        decof2-asio-executor-gcov
        EXCLUDE_FROM_ALL
        ${DECOF2_ASIO_EXECUTOR_SRC}
    )
    target_include_directories(
    decof2-asio-executor-gcov
        PUBLIC
            ${Boost_INCLUDE_DIR}
            ${PROJECT_SOURCE_DIR}/include/
        PRIVATE
            ${PROJECT_SOURCE_DIR}/include/decof/asio_executor
    )
    target_link_libraries(
        decof2-asio-executor-gcov
        decof2-core-gcov
    )
endif()
