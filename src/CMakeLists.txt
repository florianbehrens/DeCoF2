add_subdirectory(core)

if(${Boost_FOUND})
    add_subdirectory(asio_executor)
    add_subdirectory(asio_tick)
    add_subdirectory(cli)
    add_subdirectory(scgi)
endif()
