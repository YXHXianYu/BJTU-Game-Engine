# Force to produce static libraries
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

if (NOT TARGET glfw)
    option(GLFW_BUILD_EXAMPLES "" OFF)
    option(GLFW_BUILD_TESTS "" OFF)
    option(GLFW_BUILD_DOCS "" OFF)
    option(GLFW_INSTALL "" OFF)
    add_subdirectory(glfw-3.4)
endif()

if (NOT TARGET glad)
    include(glad.cmake)
endif()

if (NOT TARGET glm)
    include(glm.cmake)
endif()

if (NOT TARGET stb)
    include(stb.cmake)
endif()

if (NOT TARGET assimp)
    add_subdirectory(assimp-5.3.1)
endif()