add_library(pvk_reloaded_io io.hpp io.cpp AssetLoader.hpp AssetLoader.cpp)
target_compile_options(pvk_reloaded_io PRIVATE -Wall -Wextra -Wshadow -Wno-unused-parameter -Werror)
target_link_libraries(pvk_reloaded_io lz4 json Vulkan::Vulkan pvk_reloaded_asset pvk_reloaded_geometry)
set_target_properties(pvk_reloaded_io PROPERTIES LINKER_LANGUAGE CXX)