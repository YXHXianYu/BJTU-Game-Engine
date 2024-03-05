
# Use SPIRV
set(ENABLE_SPIRV FALSE)

if(ENABLE_SPIRV)
    message(FATAL_ERROR "VulkanSDK is NOT installed!")
    # Vulkan (For build shader)
    if(WIN32)
        # set(vulkan_lib ${THIRD_PARTY_DIR}/VulkanSDK/lib/Win32/vulkan-1.lib)
        set(GLSLANG_VALIDATOR ${THIRD_PARTY_DIR}/VulkanSDK/bin/Win32/glslangValidator.exe)
    elseif(UNIX)
        if(APPLE)
            # set(vulkan_lib ${THIRD_PARTY_DIR}/VulkanSDK/lib/MacOS/libvulkan.1.dylib)
            set(GLSLANG_VALIDATOR ${THIRD_PARTY_DIR}/VulkanSDK/bin/MacOS/glslangValidator)
        else()
            # set(vulkan_lib ${THIRD_PARTY_DIR}/VulkanSDK/lib/Linux/libvulkan.so.1)
            set(GLSLANG_VALIDATOR ${THIRD_PARTY_DIR}/VulkanSDK/bin/Linux/glslangValidator)
        endif()
    else()
        message(FATAL_ERROR "Unknown Platform")
    endif()
endif()

# Shader
set(SHADER_COMPILE_TARGET PiccoloShader)
add_subdirectory(shader)

# 3rdparty
add_subdirectory(3rdparty)

# Runtime
add_subdirectory(source/runtime)