add_executable(
    decof2-example
    EXCLUDE_FROM_ALL
    main.cpp
    composite.cpp
    composite.h
)

target_link_libraries(
    decof2-example
    decof2-asio-tick
    decof2-cli
    decof2-scgi
)

if(CMAKE_COMPILER_IS_GNUCXX)
	target_link_libraries(
        decof2-example
		pthread
	)
endif()
