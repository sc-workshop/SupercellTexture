﻿cmake_minimum_required(VERSION 3.22)

# workspace
project(supercell-sdk)

# options
option(SC_TEXTURE_BUILD_CLI "Build CLI version of supercell::texture library" ON)

# includes
include(FetchContent)

if(SC_TEXTURE_BUILD_CLI)
    set(SC_CLI_DEPENDENCIES ON)
endif()

include(cmake/dependencies.cmake)

# projects
add_subdirectory(supercell-texture)

if(SC_COMPRESSION_BUILD_CLI)
    add_subdirectory(cli-tool)
endif()
