cmake_minimum_required(VERSION 3.12)
project(machine-tester)

find_package(MPI REQUIRED)

add_executable(machine_tester main.cpp latency.cpp stats.cpp bandwidth.cpp alltoall.cpp alltoallv.cpp cpu.cpp gemm.cpp)
target_link_libraries(machine_tester MPI::MPI_C MPI::MPI_CXX)

add_executable(memory_latency memory_latency.cpp)