file(GLOB HEADER_LIST CONFIGURE_DEPENDS "*.hpp")
file(GLOB SOURCE_LIST CONFIGURE_DEPENDS "*.cpp")

add_library(pvk_reloaded_geometry ${SOURCE_LIST} ${HEADER_LIST})
target_compile_options(pvk_reloaded_geometry PRIVATE -Wall -Wextra -Wshadow -Wno-unused-parameter -Werror)
target_link_libraries(pvk_reloaded_geometry PUBLIC pvk_reloaded_vulkan PRIVATE pvk_reloaded_engine pvk_reloaded_io fmt)
set_target_properties(pvk_reloaded_geometry PROPERTIES LINKER_LANGUAGE CXX)
