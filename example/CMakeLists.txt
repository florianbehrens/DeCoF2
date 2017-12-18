add_executable(
    decof2-example
    EXCLUDE_FROM_ALL
    application.cpp
    application.h
    background_worker.cpp
    background_worker.h
    composite.cpp
    composite.h
    main.cpp
)

target_link_libraries(
    decof2-example
    decof2-asio-executor
    decof2-asio-tick
    decof2-cli
    decof2-scgi
    ${Boost_THREAD_LIBRARY}
)

if(CMAKE_COMPILER_IS_GNUCXX)
    target_link_libraries(
        decof2-example
        pthread
    )
endif()
