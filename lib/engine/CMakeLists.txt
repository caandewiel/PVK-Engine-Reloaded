file(GLOB HEADER_LIST CONFIGURE_DEPENDS "*.hpp")
file(GLOB SOURCE_LIST CONFIGURE_DEPENDS "*.cpp")

set(EXCEPTIONS_HEADER_LIST exceptions/PvkException.hpp exceptions/PvkExceptionAsset.hpp exceptions/PvkExceptionGraphics.hpp exceptions/PvkExceptionUI.hpp)

set(RENDER_STAGE_HEADER_LIST render_stage/Shader.hpp render_stage/ShaderBuilder.hpp render_stage/ShaderReflection.hpp)
set(RENDER_STAGE_SOURCE_LIST render_stage/Shader.cpp render_stage/ShaderBuilder.cpp render_stage/ShaderReflection.cpp)

set(SHADER_HEADER_LIST shader/Descriptor.hpp shader/UniformBuffer.hpp)
set(SHADER_SOURCE_LIST shader/UniformBuffer.cpp)

set(CAMERA_HEADER_LIST camera/Camera.hpp camera/CameraArc.hpp)
set(CAMERA_SOURCE_LIST camera/Camera.cpp camera/CameraArc.cpp)

set(ENTITY_HEADER_LIST entity/Entity.hpp)
set(ENTITY_SOURCE_LIST entity/Entity.cpp)

add_library(pvk_reloaded_engine ${SOURCE_LIST} ${HEADER_LIST}
    ${EXCEPTIONS_HEADER_LIST}
    ${RENDER_STAGE_SOURCE_LIST} ${RENDER_STAGE_HEADER_LIST}
    ${SHADER_SOURCE_LIST} ${SHADER_HEADER_LIST}
    ${CAMERA_SOURCE_LIST} ${CAMERA_HEADER_LIST}
    ${ENTITY_SOURCE_LIST} ${ENTITY_HEADER_LIST})
target_compile_options(pvk_reloaded_engine PRIVATE -Wall -Wextra -Wshadow -Wno-unused-parameter -Werror)
target_link_libraries(pvk_reloaded_engine PUBLIC pvk_reloaded_vulkan pvk_reloaded_ui pvk_reloaded_geometry pvk_reloaded_asset)
