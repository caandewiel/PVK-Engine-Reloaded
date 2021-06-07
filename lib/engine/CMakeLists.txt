file(GLOB HEADER_LIST CONFIGURE_DEPENDS "*.hpp")
file(GLOB SOURCE_LIST CONFIGURE_DEPENDS "*.cpp")

add_library(pvk_reloaded_engine ${SOURCE_LIST} ${HEADER_LIST} exceptions/PvkException.hpp exceptions/PvkExceptionGraphics.hpp exceptions/PvkExceptionAsset.hpp exceptions/PvkExceptionUI.hpp)
target_compile_options(pvk_reloaded_engine PRIVATE -Wall -Wextra -Wshadow -Wno-unused-parameter -Werror)
target_link_libraries(pvk_reloaded_engine pvk_reloaded_vulkan pvk_reloaded_ui pvk_reloaded_geometry)
set_target_properties(pvk_reloaded_engine PROPERTIES LINKER_LANGUAGE CXX)
