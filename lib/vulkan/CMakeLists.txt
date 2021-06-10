file(GLOB HEADER_LIST CONFIGURE_DEPENDS "*.hpp")
file(GLOB SOURCE_LIST CONFIGURE_DEPENDS "*.cpp")

add_library(pvk_reloaded_vulkan ${SOURCE_LIST} ${HEADER_LIST})
target_link_libraries(pvk_reloaded_vulkan PUBLIC Vulkan::Vulkan vma glm glfw spirv-cross-cpp PRIVATE pvk_reloaded_engine pvk_reloaded_io)
target_compile_options(pvk_reloaded_vulkan PRIVATE -Wall -Wextra -Wshadow -Wno-unused-parameter -Werror)
set_target_properties(pvk_reloaded_vulkan PROPERTIES LINKER_LANGUAGE CXX)
