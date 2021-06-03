cmake_minimum_required(VERSION 3.17)
project(pvk_reloaded)

IF (APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_MACOS_MVK")
    SET(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "Build architectures for Mac OS X" FORCE)
ENDIF ()

#
# Source: https://stackoverflow.com/questions/28344564/cmake-remove-a-compile-flag-for-a-single-translation-unit
# Applies CMAKE_CXX_FLAGS to all targets in the current CMake directory.
# After this operation, CMAKE_CXX_FLAGS is cleared.
#
macro(apply_global_cxx_flags_to_all_targets)
    separate_arguments(_global_cxx_flags_list UNIX_COMMAND ${CMAKE_CXX_FLAGS})
    get_property(_targets DIRECTORY PROPERTY BUILDSYSTEM_TARGETS)
    foreach(_target ${_targets})
        target_compile_options(${_target} PUBLIC ${_global_cxx_flags_list})
    endforeach()
    unset(CMAKE_CXX_FLAGS)
    set(_flag_sync_required TRUE)
endmacro()

#
# Source: https://stackoverflow.com/questions/28344564/cmake-remove-a-compile-flag-for-a-single-translation-unit
# Removes the specified compile flag from the specified target.
#   _target     - The target to remove the compile flag from
#   _flag       - The compile flag to remove
#
# Pre: apply_global_cxx_flags_to_all_targets() must be invoked.
#
macro(remove_flag_from_target _target _flag)
    get_target_property(_target_cxx_flags ${_target} COMPILE_OPTIONS)
    if(_target_cxx_flags)
        list(REMOVE_ITEM _target_cxx_flags ${_flag})
        set_target_properties(${_target} PROPERTIES COMPILE_OPTIONS "${_target_cxx_flags}")
    endif()
endmacro()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fno-omit-frame-pointer ")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(ASSIMP_NO_EXPORT ON)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_INSTALL OFF)

# Fetch external libraries
find_package(Vulkan REQUIRED)

# Load subfolders
add_subdirectory(external/glfw)
add_subdirectory(external/glm)
add_subdirectory(external/abseil-cpp)
add_subdirectory(external/fmt)
add_subdirectory(external/assimp)
add_subdirectory(lib/io)
add_subdirectory(lib/vulkan)
add_subdirectory(lib/engine)
add_subdirectory(lib/ui)
add_subdirectory(lib/geometry)
add_subdirectory(asset_manager)

# Include external library headers
include_directories(SYSTEM external/VulkanMemoryAllocator-Hpp/)
include_directories(SYSTEM external/imgui/)
include_directories(SYSTEM external/json/)
include_directories(SYSTEM external/lz4/programs)
include_directories(SYSTEM ${Vulkan_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} pvk_reloaded_engine)
