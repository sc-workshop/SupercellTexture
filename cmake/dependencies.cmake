
# install workshop core
FetchContent_Declare(
    WorkshopCore
    GIT_REPOSITORY https://github.com/sc-workshop/Workshop-Core.git
    GIT_TAG main
)

FetchContent_MakeAvailable(WorkshopCore)

# install supercell-compression
FetchContent_Declare(
    supercell-compression
    GIT_REPOSITORY https://github.com/sc-workshop/SupercellCompression.git
    GIT_TAG main
)

FetchContent_MakeAvailable(supercell-compression)