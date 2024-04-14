# CMAKE generated file: DO NOT EDIT!
# Generated by CMake Version 3.26
cmake_policy(SET CMP0009 NEW)

# GLAD_SOURCE_FILE at engine/3rdparty/glad.cmake:5 (file)
file(GLOB NEW_GLOB LIST_DIRECTORIES true "D:/Documents/BJTU-Game-Engine/engine/3rdparty/glad/src/*.c")
set(OLD_GLOB
  "D:/Documents/BJTU-Game-Engine/engine/3rdparty/glad/src/glad.c"
  )
if(NOT "${NEW_GLOB}" STREQUAL "${OLD_GLOB}")
  message("-- GLOB mismatch!")
  file(TOUCH_NOCREATE "D:/Documents/BJTU-Game-Engine/cmake-build-debug/CMakeFiles/cmake.verify_globs")
endif()

# SHADER_FILES at engine/shader/CMakeLists.txt:12 (file)
file(GLOB_RECURSE NEW_GLOB LIST_DIRECTORIES false "D:/Documents/BJTU-Game-Engine/engine/shader/build/glsl/*.frag")
set(OLD_GLOB
  "D:/Documents/BJTU-Game-Engine/engine/shader/build/glsl/basic.frag"
  "D:/Documents/BJTU-Game-Engine/engine/shader/build/glsl/block.frag"
  "D:/Documents/BJTU-Game-Engine/engine/shader/build/glsl/depth_rendering.frag"
  "D:/Documents/BJTU-Game-Engine/engine/shader/build/glsl/light.frag"
  "D:/Documents/BJTU-Game-Engine/engine/shader/build/glsl/model.frag"
  "D:/Documents/BJTU-Game-Engine/engine/shader/build/glsl/model_with_lighting.frag"
  "D:/Documents/BJTU-Game-Engine/engine/shader/build/glsl/shadertoy_clouds.frag"
  "D:/Documents/BJTU-Game-Engine/engine/shader/build/glsl/shadertoy_slisesix.frag"
  )
if(NOT "${NEW_GLOB}" STREQUAL "${OLD_GLOB}")
  message("-- GLOB mismatch!")
  file(TOUCH_NOCREATE "D:/Documents/BJTU-Game-Engine/cmake-build-debug/CMakeFiles/cmake.verify_globs")
endif()

# SHADER_FILES at engine/shader/CMakeLists.txt:12 (file)
file(GLOB_RECURSE NEW_GLOB LIST_DIRECTORIES false "D:/Documents/BJTU-Game-Engine/engine/shader/build/glsl/*.vert")
set(OLD_GLOB
  "D:/Documents/BJTU-Game-Engine/engine/shader/build/glsl/basic.vert"
  "D:/Documents/BJTU-Game-Engine/engine/shader/build/glsl/block.vert"
  "D:/Documents/BJTU-Game-Engine/engine/shader/build/glsl/depth_rendering.vert"
  "D:/Documents/BJTU-Game-Engine/engine/shader/build/glsl/model.vert"
  "D:/Documents/BJTU-Game-Engine/engine/shader/build/glsl/model_with_lighting.vert"
  "D:/Documents/BJTU-Game-Engine/engine/shader/build/glsl/post_process.vert"
  )
if(NOT "${NEW_GLOB}" STREQUAL "${OLD_GLOB}")
  message("-- GLOB mismatch!")
  file(TOUCH_NOCREATE "D:/Documents/BJTU-Game-Engine/cmake-build-debug/CMakeFiles/cmake.verify_globs")
endif()

# SOURCE_FILE at engine/source/runtime/CMakeLists.txt:6 (file)
file(GLOB_RECURSE NEW_GLOB LIST_DIRECTORIES false "D:/Documents/BJTU-Game-Engine/engine/source/runtime/*.cpp")
set(OLD_GLOB
  "D:/Documents/BJTU-Game-Engine/engine/source/runtime/engine.cpp"
  "D:/Documents/BJTU-Game-Engine/engine/source/runtime/function/framework/component/block_manager/block_manager_component.cpp"
  "D:/Documents/BJTU-Game-Engine/engine/source/runtime/function/framework/component/mesh/mesh_component.cpp"
  "D:/Documents/BJTU-Game-Engine/engine/source/runtime/function/framework/component/transform/transform_component.cpp"
  "D:/Documents/BJTU-Game-Engine/engine/source/runtime/function/framework/level/level.cpp"
  "D:/Documents/BJTU-Game-Engine/engine/source/runtime/function/framework/object/object.cpp"
  "D:/Documents/BJTU-Game-Engine/engine/source/runtime/function/framework/world/world_manager.cpp"
  "D:/Documents/BJTU-Game-Engine/engine/source/runtime/function/global/global_context.cpp"
  "D:/Documents/BJTU-Game-Engine/engine/source/runtime/function/input/input_system.cpp"
  "D:/Documents/BJTU-Game-Engine/engine/source/runtime/function/render/lighting/render_direction_light.cpp"
  "D:/Documents/BJTU-Game-Engine/engine/source/runtime/function/render/lighting/render_spot_light.cpp"
  "D:/Documents/BJTU-Game-Engine/engine/source/runtime/function/render/minecraft_blocks/gmemory_buffer.cpp"
  "D:/Documents/BJTU-Game-Engine/engine/source/runtime/function/render/minecraft_blocks/render_minecraft_blocks_manager.cpp"
  "D:/Documents/BJTU-Game-Engine/engine/source/runtime/function/render/render_camera.cpp"
  "D:/Documents/BJTU-Game-Engine/engine/source/runtime/function/render/render_entity.cpp"
  "D:/Documents/BJTU-Game-Engine/engine/source/runtime/function/render/render_mesh.cpp"
  "D:/Documents/BJTU-Game-Engine/engine/source/runtime/function/render/render_mesh_blocks.cpp"
  "D:/Documents/BJTU-Game-Engine/engine/source/runtime/function/render/render_pipeline.cpp"
  "D:/Documents/BJTU-Game-Engine/engine/source/runtime/function/render/render_resource.cpp"
  "D:/Documents/BJTU-Game-Engine/engine/source/runtime/function/render/render_shader.cpp"
  "D:/Documents/BJTU-Game-Engine/engine/source/runtime/function/render/render_system.cpp"
  "D:/Documents/BJTU-Game-Engine/engine/source/runtime/function/render/render_texture.cpp"
  "D:/Documents/BJTU-Game-Engine/engine/source/runtime/function/render/render_texture_3d.cpp"
  "D:/Documents/BJTU-Game-Engine/engine/source/runtime/function/swap/minecraft_blocks/add_block_swap_event.cpp"
  "D:/Documents/BJTU-Game-Engine/engine/source/runtime/function/swap/minecraft_blocks/remove_block_swap_event.cpp"
  "D:/Documents/BJTU-Game-Engine/engine/source/runtime/function/swap/swap_context.cpp"
  "D:/Documents/BJTU-Game-Engine/engine/source/runtime/function/window/window_system.cpp"
  "D:/Documents/BJTU-Game-Engine/engine/source/runtime/main.cpp"
  )
if(NOT "${NEW_GLOB}" STREQUAL "${OLD_GLOB}")
  message("-- GLOB mismatch!")
  file(TOUCH_NOCREATE "D:/Documents/BJTU-Game-Engine/cmake-build-debug/CMakeFiles/cmake.verify_globs")
endif()
