cmake_minimum_required(VERSION 3.22)

# workspace
project(supercell-sdk)

# options
option(SC_TEXTURE_BUILD_CLI "Build CLI version of supercell::texture library" OFF)

# includes
include(FetchContent)

include(cmake/dependencies.cmake)

# projects
add_subdirectory(supercell-texture)

if(SC_TEXTURE_BUILD_CLI)
    include(WkArgsparse)
    add_subdirectory(cli-tool)
endif()
