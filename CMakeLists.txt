cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(BJTUGE VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

# Variables
set(BUILD_RUNTIME_TO_EXECUTABLE TRUE) # Wait for test

# Directories
set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

set(ENGINE_ROOT_DIR "${ROOT_DIR}/engine")
set(BINARY_ROOT_DIR "${ROOT_DIR}/bin")
set(CMAKE_INSTALL_PREFIX "${ROOT_DIR}/bin")

set(THIRD_PARTY_DIR "${ENGINE_ROOT_DIR}/3rdparty")
# set(ENGINE_ASSET_DIR "${ENGINE_ROOT_DIR}/asset")

# Add Subdirectories
add_subdirectory(engine)