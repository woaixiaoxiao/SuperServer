file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h)
add_executable(server_  ${srcs})
target_include_directories(server_ PUBLIC include)
target_link_libraries(server_ PUBLIC thread_)