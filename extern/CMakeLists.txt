if (STANDALONE_BUILD AND CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable building benchmark tests" FORCE)
    add_subdirectory(benchmark)
endif ()