add_library(pvk_reloaded_io io.hpp io.cpp AssetLoader.hpp AssetLoader.cpp)
target_compile_options(pvk_reloaded_io PRIVATE -Wall -Wextra -Wshadow -Wno-unused-parameter -Werror)
target_link_libraries(pvk_reloaded_io pvk_reloaded_geometry pvk_reloaded_asset)