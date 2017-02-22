add_subdirectory(asio_tick)
add_subdirectory(cli)
add_subdirectory(core)
add_subdirectory(scgi)

if (DECOF2_WEBSOCKET)
    add_subdirectory(websocket)
endif()
