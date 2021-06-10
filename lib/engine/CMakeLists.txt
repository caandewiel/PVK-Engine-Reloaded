file(GLOB HEADER_LIST CONFIGURE_DEPENDS "*.hpp")
file(GLOB SOURCE_LIST CONFIGURE_DEPENDS "*.cpp")

set(EXCEPTIONS_HEADER_LIST exceptions/PvkException.hpp exceptions/PvkExceptionAsset.hpp exceptions/PvkExceptionGraphics.hpp exceptions/PvkExceptionUI.hpp)

set(PIPELINE_HEADER_LIST pipeline/RenderStageBuilder.hpp pipeline/ShaderReflection.hpp)
set(PIPELINE_SOURCE_LIST pipeline/RenderStageBuilder.cpp pipeline/ShaderReflection.cpp)

add_library(pvk_reloaded_engine ${SOURCE_LIST} ${HEADER_LIST} ${EXCEPTIONS_HEADER_LIST} ${PIPELINE_SOURCE_LIST} ${PIPELINE_HEADER_LIST})
target_compile_options(pvk_reloaded_engine PRIVATE -Wall -Wextra -Wshadow -Wno-unused-parameter -Werror)
target_link_libraries(pvk_reloaded_engine PUBLIC pvk_reloaded_vulkan pvk_reloaded_ui pvk_reloaded_geometry pvk_reloaded_asset)
