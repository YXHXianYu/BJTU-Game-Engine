# set glad directory
set(GLAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glad)

# add all cpp files in glad src directory
file(GLOB GLAD_SOURCE_FILE CONFIGURE_DEPENDS "${GLAD_DIR}/src/*.c")
add_library(glad STATIC ${GLAD_SOURCE_FILE})

# add include directory
target_include_directories(glad PUBLIC "${GLAD_DIR}/include")