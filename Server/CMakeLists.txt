file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h)
# 可执行文件
add_executable(Server  ${srcs})
target_include_directories(Server PUBLIC include)
target_link_libraries(Server PUBLIC Thread)
