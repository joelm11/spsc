if (STANDALONE_BUILD AND CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    add_subdirectory(benchmark)
endif ()    