cmake_minimum_required(VERSION 3.17)
project(pvk_reloaded)

IF (APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_MACOS_MVK")
    SET(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "Build architectures for Mac OS X" FORCE)
ENDIF ()

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
# set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

# Fetch external libraries
find_package(Vulkan REQUIRED)

# Load subfolders
add_subdirectory(external)
add_subdirectory(lib/io)
add_subdirectory(lib/vulkan)
add_subdirectory(lib/engine)
add_subdirectory(lib/ui)
add_subdirectory(lib/geometry)
add_subdirectory(lib/asset)
add_subdirectory(asset_manager)

# Include external library headers
include_directories(SYSTEM external/VulkanMemoryAllocator-Hpp/)
include_directories(SYSTEM ${Vulkan_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} pvk_reloaded_engine)
