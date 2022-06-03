cmake_minimum_required(VERSION 3.18)
project(machine-tester)

find_package(MPI REQUIRED)
find_package(OpenMP REQUIRED)
find_package(BLAS)

add_executable(machine_tester main.cpp latency.cpp stats.cpp bandwidth.cpp alltoall.cpp alltoallv.cpp cpu.cpp gemm.cpp)
target_link_libraries(machine_tester MPI::MPI_C MPI::MPI_CXX OpenMP::OpenMP_CXX)

if (${BLAS_FOUND})
target_compile_definitions(machine_tester PRIVATE "-DENABLE_BLAS")
target_link_libraries(machine_tester BLAS::BLAS)
endif ()

add_executable(memory_latency memory_latency.cpp)