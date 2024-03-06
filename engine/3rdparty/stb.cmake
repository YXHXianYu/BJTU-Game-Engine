# set directory
set(STB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/stb)

# add an interface library
add_library(stb INTERFACE)

# add include directory
target_include_directories(stb INTERFACE "${STB_DIR}")