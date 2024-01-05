file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h)
add_executable(Server  ${srcs})
target_include_directories(Server PUBLIC include)
target_link_libraries(Server PUBLIC Thread)