file(GLOB HEADER_LIST CONFIGURE_DEPENDS "*.hpp")
file(GLOB SOURCE_LIST CONFIGURE_DEPENDS "*.cpp")

add_library(pvk_reloaded_ui ${SOURCE_LIST} ${HEADER_LIST})
target_compile_options(pvk_reloaded_ui PRIVATE -Wall -Wextra -Wshadow -Wno-unused-parameter -Werror)
target_link_libraries(pvk_reloaded_ui PUBLIC imgui fmt fa absl::flat_hash_map PRIVATE pvk_reloaded_vulkan)
