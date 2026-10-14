cmake_minimum_required(VERSION 3.5)
project(equityExchange
    VERSION 1.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20) # greater than 20 has build errors
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # export compile commands for IDEs / clangd

find_package(Boost REQUIRED COMPONENTS system filesystem)

# Google Test
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

add_subdirectory(MatchingEngine)
add_subdirectory(MatchingEngineTest)



#cmake -B build
#cmake —-build build
#cd build
#./equityExchange
