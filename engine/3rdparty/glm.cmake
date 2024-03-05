# set glm directory
set(GLM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glm-1.0.1-light)

# add an interface library for glm
add_library(glm INTERFACE)

# add include directory
target_include_directories(glm INTERFACE "${GLM_DIR}")