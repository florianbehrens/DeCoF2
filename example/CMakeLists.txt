include_directories(${CMAKE_SOURCE_DIR}/src)
add_executable(example main.cpp)
target_link_libraries(example decof2-textproto decof2-webservice decof2-core ssl crypto pthread cppnetlib-server-parsers ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY})
