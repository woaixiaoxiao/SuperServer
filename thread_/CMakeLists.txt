file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h)
add_library(thread_ STATIC ${srcs})
target_include_directories(thread_ PUBLIC include)