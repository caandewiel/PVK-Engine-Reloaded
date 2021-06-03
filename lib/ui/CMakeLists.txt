file(GLOB HEADER_LIST CONFIGURE_DEPENDS "*.hpp")
file(GLOB SOURCE_LIST CONFIGURE_DEPENDS "*.cpp")

add_library(imgui STATIC)
target_include_directories(imgui PUBLIC ../../external/imgui/)

target_sources(imgui PRIVATE 
"../../external/imgui/imgui.h"
"../../external/imgui/imgui.cpp"
"../../external/imgui/imgui_demo.cpp"
"../../external/imgui/imgui_draw.cpp"
"../../external/imgui/imgui_tables.cpp"
"../../external/imgui/imgui_widgets.cpp"
"../../external/imgui/backends/imgui_impl_vulkan.cpp"
"../../external/imgui/backends/imgui_impl_glfw.cpp"
)

target_link_libraries(imgui Vulkan::Vulkan glfw)

add_library(pvk_reloaded_ui ${SOURCE_LIST} ${HEADER_LIST})
target_link_libraries(pvk_reloaded_ui imgui absl::flat_hash_map fmt)
target_compile_options(pvk_reloaded_ui PRIVATE -Wall -Wextra -Wshadow -Wno-unused-parameter -Werror)
set_target_properties(pvk_reloaded_ui PROPERTIES LINKER_LANGUAGE CXX)
