cmake_minimum_required(VERSION 3.18)
project(openblas-svd-bug VERSION 1.0.0 DESCRIPTION "Investigate a segmentation fault in OpenBLAS SVD routine")

# Use bundled find_<pkg>.cmake modules (for lapacke)
list(INSERT CMAKE_MODULE_PATH 0 ${PROJECT_SOURCE_DIR}/cmake)

# Make a "flags" target with compiler warnings
include(cmake/CompilerFlags.cmake)

# Create an executable
add_executable(openblas-svd-bug
               source/main.cpp
               source/gesvd.cpp
               source/gesdd.cpp
               source/stacktrace.cpp
               )

find_package(h5pp REQUIRED)
find_package(Backward REQUIRED)
find_package(Lapacke REQUIRED)
target_link_libraries(openblas-svd-bug PRIVATE flags)
target_link_libraries(openblas-svd-bug PRIVATE h5pp::h5pp)
target_link_libraries(openblas-svd-bug PRIVATE Backward::Backward)
target_link_libraries(openblas-svd-bug PRIVATE lapacke::lapacke)
target_compile_definitions(openblas-svd-bug PRIVATE OUTDIR="${PROJECT_SOURCE_DIR}/data")

# Print summary of CMake configuration
include(cmake/PrintTargetInfo.cmake)
print_project_summary(openblas-svd-bug)



