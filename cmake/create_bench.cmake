# Function to create a benchmark executable.
# benchmark_name:    Benchmark executable name.
# benchmark_source:  Benchmark source file name.
function(benchmark benchmark_name benchmark_source)
    add_executable(${benchmark_name} ${benchmark_source})
    target_link_libraries(${benchmark_name} PUBLIC benchmark::benchmark)
endfunction()