IF(NOT EXISTS "D:/Documents/BJTU-Game-Engine/cmake-build-debug/engine/3rdparty/assimp-5.3.1/install_manifest.txt")
  MESSAGE(FATAL_ERROR "Cannot find install manifest: \"D:/Documents/BJTU-Game-Engine/cmake-build-debug/engine/3rdparty/assimp-5.3.1/install_manifest.txt\"")
ENDIF(NOT EXISTS "D:/Documents/BJTU-Game-Engine/cmake-build-debug/engine/3rdparty/assimp-5.3.1/install_manifest.txt")

FILE(READ "D:/Documents/BJTU-Game-Engine/cmake-build-debug/engine/3rdparty/assimp-5.3.1/install_manifest.txt" files)
STRING(REGEX REPLACE "\n" ";" files "${files}")
FOREACH(file ${files})
  MESSAGE(STATUS "Uninstalling \"$ENV{DESTDIR}${file}\"")
  EXEC_PROGRAM(
    "D:/Program Files/JetBrains/CLion 2023.2.2/bin/cmake/win/x64/bin/cmake.exe" ARGS "-E remove \"$ENV{DESTDIR}${file}\""
    OUTPUT_VARIABLE rm_out
    RETURN_VALUE rm_retval
    )
  IF(NOT "${rm_retval}" STREQUAL 0)
    MESSAGE(FATAL_ERROR "Problem when removing \"$ENV{DESTDIR}${file}\"")
  ENDIF(NOT "${rm_retval}" STREQUAL 0)
ENDFOREACH(file)
