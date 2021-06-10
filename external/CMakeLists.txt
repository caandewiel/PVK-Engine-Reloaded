# GLFW
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/glfw")

# Abseil
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/abseil-cpp")

# FMT
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/fmt")

# Assimp
set(ASSIMP_SHARED_LIBS OFF)
set(ASSIMP_NO_EXPORT ON)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_INSTALL OFF)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/assimp")

# SPIRV-Cross
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/SPIRV-Cross")

# Header only libraries
add_library(glm INTERFACE)
add_library(json INTERFACE)
add_library(fa INTERFACE)
add_library(vma INTERFACE)

# Static libraries
add_library(lz4 STATIC)
add_library(imgui STATIC)

# GLM
target_include_directories(glm INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/glm/")

# JSON
target_include_directories(json INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/json/")

# LZ4
target_include_directories(lz4 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lz4/lib/")
set(LZ4_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lz4/lib/")
set(PRG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lz4/programs/")
set(LZ4_SRCS_LIB ${LZ4_DIR}lz4.c ${LZ4_DIR}lz4hc.c ${LZ4_DIR}lz4.h ${LZ4_DIR}lz4hc.h ${LZ4_DIR}lz4frame.c ${LZ4_DIR}lz4frame.h ${LZ4_DIR}xxhash.c)
set(LZ4_SRCS ${LZ4_DIR}lz4frame.c ${LZ4_DIR}xxhash.c ${PRG_DIR}bench.c ${PRG_DIR}lz4cli.c ${PRG_DIR}lz4io.c)
target_sources(lz4 PRIVATE ${LZ4_SRCS_LIB} ${LZ4_SRCS})

# FontAwesome
target_include_directories(fa INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/FontAwesome/")

# VMA
target_include_directories(vma INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/proxy")

# ImGUI
target_include_directories(imgui PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/imgui/")
target_sources(imgui PRIVATE 
"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.h"
"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_demo.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_draw.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_tables.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_widgets.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_vulkan.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_glfw.cpp"
)
target_link_libraries(imgui Vulkan::Vulkan glfw)