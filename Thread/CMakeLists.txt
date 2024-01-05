file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h)
add_library(Thread STATIC ${srcs})
target_include_directories(Thread PUBLIC include)