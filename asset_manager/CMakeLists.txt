cmake_minimum_required(VERSION 3.17)
project(pvk_reloaded_asset_manager)

add_executable(${PROJECT_NAME} main.cpp)
target_compile_options(pvk_reloaded_asset_manager PRIVATE -Wall -Wextra -Wshadow -Wno-unused-parameter -Werror)
target_link_libraries(${PROJECT_NAME} assimp fmt lz4 json glm pvk_reloaded_asset)