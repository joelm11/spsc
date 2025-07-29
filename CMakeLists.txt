cmake_minimum_required(VERSION 3.29.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Only set project if this is the main project
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(STANDALONE_BUILD TRUE)
    project(spsc)

    # Only set build type if this is the main project
    if(STANDALONE_BUILD AND CMAKE_BUILD_TYPE STREQUAL "Debug")        
        message(STATUS "Debug standalone build")
        set(CMAKE_BUILD_TYPE RelWithDebInfo)
    else()
        set(CMAKE_BUILD_TYPE Release)
    endif()
endif()


# Only enable testing and FetchContent if this is the main project
if(STANDALONE_BUILD)
    # Pull in Google Test
    message(STATUS "Unit tests enabled")
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2)
    FetchContent_MakeAvailable(GoogleTest)
    enable_testing()
    include(GoogleTest)
    include("cmake/create_ut.cmake")
    add_subdirectory(src/test)
    add_subdirectory(extern)
    include("cmake/create_bench.cmake")
    add_subdirectory(src/bench)
endif()