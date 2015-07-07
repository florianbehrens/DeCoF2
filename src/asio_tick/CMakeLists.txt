include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
set(
    DECOF2_ASIO_TICK_SRC
    asio_tick.cpp
    asio_tick.h
)

# Standard DeCoF2 ASIO Tick library
add_library(
    decof2-asio-tick
    ${DECOF2_ASIO_TICK_SRC}
)
target_link_libraries(
    decof2-asio-tick
    decof2-core
)

# DeCoF2 ASIO Tick library with coverage information
add_library(
    decof2-asio-tick-gcov
    ${DECOF2_ASIO_TICK_SRC}
)
target_link_libraries(
    decof2-asio-tick-gcov
    decof2-core-gcov
)
