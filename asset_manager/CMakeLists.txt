cmake_minimum_required(VERSION 3.17)
project(pvk_reloaded_asset_manager)

add_library(json INTERFACE)
target_include_directories(json INTERFACE ../external/json/)
set_target_properties(json PROPERTIES LINKER_LANGUAGE CXX)

add_library(lz4 STATIC)
target_include_directories(lz4 PUBLIC ../external/lz4/lib/)

set(LZ4_DIR "../external/lz4/lib/")
set(PRG_DIR "../external/lz4/programs/")
set(LZ4_SRCS_LIB ${LZ4_DIR}lz4.c ${LZ4_DIR}lz4hc.c ${LZ4_DIR}lz4.h ${LZ4_DIR}lz4hc.h ${LZ4_DIR}lz4frame.c ${LZ4_DIR}lz4frame.h ${LZ4_DIR}xxhash.c)
set(LZ4_SRCS ${LZ4_DIR}lz4frame.c ${LZ4_DIR}xxhash.c ${PRG_DIR}bench.c ${PRG_DIR}lz4cli.c ${PRG_DIR}lz4io.c)

target_sources(lz4 PRIVATE ${LZ4_SRCS_LIB} ${LZ4_SRCS})

add_executable(${PROJECT_NAME} main.cpp)
target_compile_options(pvk_reloaded_asset_manager PRIVATE -Wall -Wextra -Wshadow -Wno-unused-parameter -Werror)
target_link_libraries(${PROJECT_NAME} assimp fmt lz4 json glm pvk_reloaded_asset)