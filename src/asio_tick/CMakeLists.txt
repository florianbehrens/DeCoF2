set(
    DECOF2_ASIO_TICK_SRC
    asio_tick.cpp
    asio_tick.h
)

# Standard DeCoF2 ASIO Tick library
add_library(
    decof2-asio-tick
    EXCLUDE_FROM_ALL
    ${DECOF2_ASIO_TICK_SRC}
)
target_include_directories(
    decof2-asio-tick
    PUBLIC
        ${Boost_INCLUDE_DIR}
        ../../include
        ../../include/decof
    PRIVATE
        ../core
)
target_link_libraries(
    decof2-asio-tick
    decof2-core
)

# DeCoF2 ASIO Tick library with coverage information
if(CMAKE_COMPILER_IS_GNUCXX)
	add_library(
		decof2-asio-tick-gcov
		EXCLUDE_FROM_ALL
		${DECOF2_ASIO_TICK_SRC}
	)
	target_include_directories(
		decof2-asio-tick-gcov
		PUBLIC
			${Boost_INCLUDE_DIR}
			../../include
			../../include/decof
		PRIVATE
			../core
	)
	target_link_libraries(
		decof2-asio-tick-gcov
		decof2-core-gcov
	)
endif()
