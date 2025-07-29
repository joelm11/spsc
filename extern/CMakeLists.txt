if (STANDALONE_BUILD)
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable building benchmark tests" FORCE)
    add_subdirectory(benchmark)
endif ()