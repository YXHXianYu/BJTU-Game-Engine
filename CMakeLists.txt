cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(BJTUGE VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

# Variables
set(BUILD_RUNTIME_TO_EXECUTABLE TRUE) # Wait for test

# Generate compile_commands.json for clangd
# then make clangd to use it through `--compile-commands-dir=${workspaceFolder}/build`
# and it should solve the problem of standard library not found
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "") 
if(CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES 
      ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

# Directories
set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

set(ENGINE_ROOT_DIR "${ROOT_DIR}/engine")
set(BINARY_ROOT_DIR "${ROOT_DIR}/bin")
set(CMAKE_INSTALL_PREFIX "${ROOT_DIR}/bin")

set(THIRD_PARTY_DIR "${ROOT_DIR}/3rdparty")
set(ENGINE_ASSET_DIR "/asset")

# Add Subdirectories
add_subdirectory(engine)