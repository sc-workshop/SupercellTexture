# Project: supercell-texture

# C++ headers and source files
file(GLOB_RECURSE HEADERS source/*.h)
file(GLOB_RECURSE SOURCES source/*.cpp)

# choosing build type
if(SC_TEXTURE_BUILD_SHARED)
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

target_link_libraries(${TARGET} PUBLIC supercell::core)
target_link_libraries(${TARGET} PUBLIC supercell::compression)

set_target_properties(${TARGET} PROPERTIES
    FOLDER SupercellSDK/core
)

# setup project
sc_project_setup(${TARGET})
