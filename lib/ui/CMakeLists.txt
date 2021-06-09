file(GLOB HEADER_LIST CONFIGURE_DEPENDS "*.hpp")
file(GLOB SOURCE_LIST CONFIGURE_DEPENDS "*.cpp")

add_library(pvk_reloaded_ui ${SOURCE_LIST} ${HEADER_LIST})
target_compile_options(pvk_reloaded_ui PRIVATE -Wall -Wextra -Wshadow -Wno-unused-parameter -Werror)
target_link_libraries(pvk_reloaded_ui pvk_reloaded_vulkan imgui fmt fa)
set_target_properties(pvk_reloaded_ui PROPERTIES LINKER_LANGUAGE CXX)
