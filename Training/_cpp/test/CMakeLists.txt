cmake_minimum_required(VERSION 3.1)

set(TEST_NAME TEST)

set(TEST_SOURCE test_sample.cpp)

add_executable(${TEST_NAME} ${TEST_SOURCE})
target_link_libraries(${TEST_NAME} 
     PUBLIC 
        gtest_main
        _cpp
)

add_test(
    NAME ${TEST_NAME}
    COMMAND ${TEST_NAME}
)