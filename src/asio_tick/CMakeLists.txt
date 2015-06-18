include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
add_library(
    decof2-asio-tick
    asio_tick.cpp
    asio_tick.h
)
target_link_libraries(
    decof2-asio-tick
    decof2-core
)
