cmake_minimum_required(VERSION 3.18)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(myServer LANGUAGES CXX)

enable_testing()

add_subdirectory(Server)
add_subdirectory(Thread)
add_subdirectory(Buffer)

add_subdirectory(Test)
