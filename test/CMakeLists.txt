include_directories(${CMAKE_SOURCE_DIR}/src/webservice)
add_executable(
    test
    test_http_body_parser.cpp
)
target_link_libraries(test decof2-webservice ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
