# Install script for directory: D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "D:/Documents/BJTU-Game-Engine/bin")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "D:/Program Files/JetBrains/CLion 2023.2.2/bin/mingw/bin/objdump.exe")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "libassimp5.3.0-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/Documents/BJTU-Game-Engine/cmake-build-debug/engine/3rdparty/assimp-5.3.1/lib/libassimpd.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp" TYPE FILE FILES
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/anim.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/aabb.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/ai_assert.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/camera.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/color4.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/color4.inl"
    "D:/Documents/BJTU-Game-Engine/cmake-build-debug/engine/3rdparty/assimp-5.3.1/code/../include/assimp/config.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/ColladaMetaData.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/commonMetaData.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/defs.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/cfileio.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/light.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/material.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/material.inl"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/matrix3x3.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/matrix3x3.inl"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/matrix4x4.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/matrix4x4.inl"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/mesh.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/ObjMaterial.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/pbrmaterial.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/GltfMaterial.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/postprocess.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/quaternion.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/quaternion.inl"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/scene.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/metadata.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/texture.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/types.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/vector2.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/vector2.inl"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/vector3.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/vector3.inl"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/version.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/cimport.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/AssertHandler.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/importerdesc.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/Importer.hpp"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/DefaultLogger.hpp"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/ProgressHandler.hpp"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/IOStream.hpp"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/IOSystem.hpp"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/Logger.hpp"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/LogStream.hpp"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/NullLogger.hpp"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/cexport.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/Exporter.hpp"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/DefaultIOStream.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/DefaultIOSystem.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/ZipArchiveIOSystem.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/SceneCombiner.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/fast_atof.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/qnan.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/BaseImporter.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/Hash.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/MemoryIOWrapper.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/ParsingUtils.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/StreamReader.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/StreamWriter.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/StringComparison.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/StringUtils.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/SGSpatialSort.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/GenericProperty.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/SpatialSort.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/SkeletonMeshBuilder.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/SmallVector.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/SmoothingGroups.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/SmoothingGroups.inl"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/StandardShapes.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/RemoveComments.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/Subdivision.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/Vertex.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/LineSplitter.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/TinyFormatter.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/Profiler.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/LogAux.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/Bitmap.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/XMLTools.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/IOStreamBuffer.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/CreateAnimMesh.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/XmlParser.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/BlobIOSystem.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/MathFunctions.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/Exceptional.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/ByteSwapper.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/Base64.hpp"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp/Compiler" TYPE FILE FILES
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/Compiler/pushpack1.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/Compiler/poppack1.h"
    "D:/Documents/BJTU-Game-Engine/engine/3rdparty/assimp-5.3.1/code/../include/assimp/Compiler/pstdint.h"
    )
endif()

