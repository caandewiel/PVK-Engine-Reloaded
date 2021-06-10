file(GLOB HEADER_LIST CONFIGURE_DEPENDS "*.hpp")
file(GLOB SOURCE_LIST CONFIGURE_DEPENDS "*.cpp")

add_library(pvk_reloaded_asset ${SOURCE_LIST} ${HEADER_LIST})
target_compile_options(pvk_reloaded_asset PRIVATE -Wall -Wextra -Wshadow -Wno-unused-parameter -Werror)
target_link_libraries(pvk_reloaded_asset PUBLIC glm lz4 absl::flat_hash_map json)
