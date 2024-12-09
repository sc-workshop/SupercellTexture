cmake_minimum_required(VERSION 3.22)

# workspace
project(supercell-sdk)

# options
option(SC_TEXTURE_BUILD_CLI "Build CLI version of supercell::texture library" OFF)

# includes
include(FetchContent)

if(SC_TEXTURE_BUILD_CLI)
    set(WK_CLI_DEPENDENCIES ON)
endif()

set(WK_SERIALIZATION_FLATBUFFER ON)

include(cmake/dependencies.cmake)

# projects
add_subdirectory(supercell-texture)

if(SC_COMPRESSION_BUILD_CLI)
    add_subdirectory(cli-tool)
endif()
