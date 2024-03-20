cmake_minimum_required(VERSION 3.1)
include(FetchContent)
set(CMAKE_C_COMPILER:FILEPATH "C:/mingw64/bin/gcc.exe")
set(CMAKE_CXX_COMPILER:FILEPATH "C:/mingw64/bin/g++.exe")
set(CMAKE_CXX_STANDARD 11)

project(_cpp)

option(TEST_ENABLED "Tests disabled by default" OFF)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
)

add_library(
    _cpp
    STATIC
    ${PROJECT_SOURCE_DIR}/src/sample/sample.cpp
    #Add include source file here
)

add_executable(main ${PROJECT_SOURCE_DIR}/app/main.cpp)

target_link_libraries(
    main PUBLIC
    _cpp
    #Add libraries here
)

if(TEST_ENABLED)

    FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        main
    )

    FetchContent_MakeAvailable(
    googletest)

    enable_testing()
    add_subdirectory(${PROJECT_SOURCE_DIR}/test)
    
endif(TEST_ENABLED)