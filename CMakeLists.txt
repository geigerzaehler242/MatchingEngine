cmake_minimum_required(VERSION 3.5)
project(equityExchange
    VERSION 1.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20) # greater than 20 has build errors
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # export compile commands for IDEs / clangd

# ---------------------------------------------------------------------------
# librdkafka  (the underlying C library that modern-cpp-kafka wraps)
# ---------------------------------------------------------------------------
# Preferred: pkg-config (works on most Linux / macOS installs)
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(RDKAFKA REQUIRED IMPORTED_TARGET rdkafka)
    set(RDKAFKA_TARGET PkgConfig::RDKAFKA)
else()
    # Fallback: try the CMake config that some installs provide
    find_package(RdKafka QUIET)
    if(RdKafka_FOUND)
        set(RDKAFKA_TARGET RdKafka::rdkafka)
    else()
        message(FATAL_ERROR
            "librdkafka not found.\n"
            "Install it first, e.g.:\n"
            "  Ubuntu/Debian : sudo apt install librdkafka-dev\n"
            "  macOS (brew)  : brew install librdkafka\n"
            "  or build from https://github.com/confluentinc/librdkafka")
            
        # Fallback for Homebrew if pkg-config is not perfect
        if(APPLE)
            # Apple Silicon
            if(EXISTS "/opt/homebrew/include")
                list(APPEND RDKAFKA_INCLUDE_DIRS "/opt/homebrew/include")
                list(APPEND RDKAFKA_LIBRARY_DIRS "/opt/homebrew/lib")
            endif()
        endif()
    endif()
endif()

# -------------------------------------------------
# modern-cpp-kafka (header-only)
# -------------------------------------------------
include(FetchContent)
FetchContent_Declare(
    modern-cpp-kafka
    GIT_REPOSITORY https://github.com/morganstanley/modern-cpp-kafka.git
    GIT_TAG        v2024.07.03          # latest stable release
)
FetchContent_GetProperties(modern-cpp-kafka)
if(NOT modern-cpp-kafka_POPULATED)
    FetchContent_Populate(modern-cpp-kafka)
endif()

# Create an INTERFACE library so the rest of the project can simply
# target_link_libraries(... modern-cpp-kafka)
add_library(modern-cpp-kafka INTERFACE)
target_include_directories(modern-cpp-kafka
    INTERFACE
        ${modern-cpp-kafka_SOURCE_DIR}/include
)
target_link_libraries(modern-cpp-kafka
    INTERFACE
        ${RDKAFKA_TARGET}
)

# -------------------------------------------------
# Boost
# -------------------------------------------------
find_package(Boost REQUIRED COMPONENTS system filesystem)

# -------------------------------------------------
# Google Test
# -------------------------------------------------
# Prefer an installed GTest; fall back to FetchContent if not found.
find_package(GTest QUIET)
if(NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.18.0
    )
    FetchContent_MakeAvailable(googletest)
endif()

enable_testing()
include(GoogleTest)

# -------------------------------------------------
add_subdirectory(MatchingEngine)
add_subdirectory(MatchingEngineTest)


# cd MatchingEngine/build/MatchingEngine
#cmake -B build
#cmake —-build build
#cd build
#./equityExchange

#cmake -B build
#cd build
#cmake ..
#cmake --build .
