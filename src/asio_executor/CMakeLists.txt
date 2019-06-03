add_library(
    decof2-asio-executor
    EXCLUDE_FROM_ALL
    asio_executor.cpp
)
target_include_directories(
    decof2-asio-executor
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include/decof/asio_executor
)
target_link_libraries(decof2-asio-executor decof2-core)
