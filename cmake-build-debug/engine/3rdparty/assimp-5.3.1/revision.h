#ifndef ASSIMP_REVISION_H_INC
#define ASSIMP_REVISION_H_INC

#define GitVersion 0x858607ae
#define GitBranch "main"

#define VER_MAJOR 5
#define VER_MINOR 3
#define VER_PATCH 0
#define VER_BUILD 0

#define STR_HELP(x) #x
#define STR(x) STR_HELP(x)

#define VER_FILEVERSION             VER_MAJOR,VER_MINOR,VER_PATCH,VER_BUILD
#if (GitVersion == 0)
#define VER_FILEVERSION_STR         STR(VER_MAJOR) "." STR(VER_MINOR) "." STR(VER_PATCH) "." STR(VER_BUILD)
#else
#define VER_FILEVERSION_STR         STR(VER_MAJOR) "." STR(VER_MINOR) "." STR(VER_PATCH) "." STR(VER_BUILD) " (Commit 858607ae)"
#endif
#define VER_COPYRIGHT_STR           "\xA9 2006-2023"

#ifdef  NDEBUG
#define VER_ORIGINAL_FILENAME_STR   "libassimp-5.dll"
#else
#define VER_ORIGINAL_FILENAME_STR   "libassimp-5d.dll"
#endif //  NDEBUG

#endif // ASSIMP_REVISION_H_INC
