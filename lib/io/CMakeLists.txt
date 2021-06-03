add_library(pvk_reloaded_io io.hpp io.cpp AssetLoader.hpp AssetLoader.cpp)
target_compile_options(pvk_reloaded_io PRIVATE -Wall -Wextra -Wshadow -Wno-unused-parameter -Werror)
target_link_libraries(pvk_reloaded_io lz4 json)
set_target_properties(pvk_reloaded_io PROPERTIES LINKER_LANGUAGE CXX)