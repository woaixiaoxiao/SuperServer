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
add_subdirectory(Pool)
add_subdirectory(Buffer)
add_subdirectory(Log)
add_subdirectory(Http)
add_subdirectory(Timer)
add_subdirectory(Test)
add_subdirectory(SkipList)
# add_subdirectory(Main)

add_executable(myServer main.cpp)
target_link_libraries(myServer PUBLIC Server)
