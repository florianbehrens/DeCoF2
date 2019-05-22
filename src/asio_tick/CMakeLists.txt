# Standard DeCoF2 ASIO Tick library
add_library(
    decof2-asio-tick
    EXCLUDE_FROM_ALL
    asio_tick.cpp
)
target_include_directories(
    decof2-asio-tick
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include/decof/asio_tick
)
target_link_libraries(
    decof2-asio-tick
    decof2-core
)
