# Project: supercell-texture

# C++ headers and source files
file(GLOB_RECURSE HEADERS source/*.h)
file(GLOB_RECURSE SOURCES source/*.cpp)
file(GLOB SCTX_SCHEMAS sctx_schemas/*.fbs)

# choosing build type
if(WK_BUILD_SHARED)
    # shared library
    set(TARGET supercell-texture-shared)
    add_library(${TARGET} SHARED)

else()
    # static library
    set(TARGET supercell-texture-static)
    add_library(${TARGET} STATIC)

endif()

add_library(supercell::texture ALIAS ${TARGET})

# add *.h and *.cpp files
target_sources(${TARGET} PRIVATE ${HEADERS} ${SOURCES})

# include directories
target_include_directories(${TARGET} PUBLIC source)
target_link_libraries(${TARGET} PUBLIC wk::core supercell::compression)

set_target_properties(${TARGET} PROPERTIES
    FOLDER SupercellSDK/core
)

# setup project
wk_project_setup(${TARGET})

# SCTX
wk_flatbuffers_generate_headers(
    TARGET SCTX_API
    INCLUDE_PREFIX texture/SCTX
    SCHEMAS ${SCTX_SCHEMAS}
    FLAGS --cpp-std=c++17
)

set_target_properties(${TARGET} PROPERTIES
    FOLDER SupercellSDK/core
)

target_link_libraries(${TARGET}
    PUBLIC SCTX_API
)
