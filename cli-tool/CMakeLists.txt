# Project: supercell-texture cli-tool


# C++ headers and source files
file(GLOB_RECURSE HEADERS source/*.h)
file(GLOB_RECURSE SOURCES source/*.cpp source/*.hpp)

set(CLI_TARGET supercell-texture-cli)

add_executable(${CLI_TARGET} ${HEADERS} ${SOURCES})

set_target_properties(${CLI_TARGET} PROPERTIES
    FOLDER SupercellSDK/tools
)

target_include_directories(${CLI_TARGET} PUBLIC source)

target_link_libraries(${CLI_TARGET} PRIVATE supercell::texture)
