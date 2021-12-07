#include <assert.h>
#include <limits.h>
#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tuple>
#include <unistd.h>
#include <vector>

#include "alltoall.h"
#include "alltoallv.h"
#include "bandwidth.h"
#include "cpu.h"
#include "gemm.h"
#include "latency.h"
#include "mpi-ext.h"
#include "stats.h"

int main(int argc, char *argv[]) {
  int num_procs = 0, my_id = 0;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_id);

  if (my_id == 0) {
    printf("Running with %d processes\n", num_procs);

    int version = 0, subversion = 0;
    MPI_Get_version(&version, &subversion);

    char buffer[MPI_MAX_LIBRARY_VERSION_STRING] = {0};
    int len = 0;
    MPI_Get_library_version(buffer, &len);

    printf("Using MPI %d.%d library %s\n", version, subversion, buffer);

// https://www.open-mpi.org/faq/?category=runcuda
#if defined(MPIX_CUDA_AWARE_SUPPORT) && MPIX_CUDA_AWARE_SUPPORT
    printf("MPI is CUDA-aware in compile time\n");
#elif defined(MPIX_CUDA_AWARE_SUPPORT) && !MPIX_CUDA_AWARE_SUPPORT
    printf("MPI is NOT CUDA-aware in compile time\n");
#endif

#if defined(MPIX_CUDA_AWARE_SUPPORT)
    if (1 == MPIX_Query_cuda_support()) {
      printf("MPI is CUDA-aware in run time\n");
    } else {
      printf("MPI is NOT CUDA-aware in run time\n");
    }
#endif
  }

  // get hostname
  char my_hostname[HOST_NAME_MAX] = {0};
  int res = gethostname(my_hostname, sizeof(my_hostname));
  if (res < 0) {
    perror("Failed to call gethostname()");
  }

  // gather hostnames
  char *all_hostnames = nullptr;
  if (my_id == 0) {
    all_hostnames = (char *)malloc(HOST_NAME_MAX * num_procs);
    assert(all_hostnames != nullptr);
  }
  MPI_Gather(my_hostname, HOST_NAME_MAX, MPI_CHAR, all_hostnames, HOST_NAME_MAX,
             MPI_CHAR, 0, MPI_COMM_WORLD);
  if (my_id == 0) {
    for (int i = 0; i < num_procs; i++) {
      printf("Host %d: %s\n", i, &all_hostnames[i * HOST_NAME_MAX]);
    }
  }

  // pairs of communications
  std::vector<std::tuple<int, int>> comms;
  for (int from = 0; from < num_procs; from++) {
    for (int to = 0; to < num_procs; to++) {
      if (from >= to) {
        continue;
      }
      comms.emplace_back(from, to);
    }
  }

  printf("Available tests: latency, bandwidth, alltoall, alltoallv, cpu, gemm\n");

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "latency") == 0) {
      latency_test(num_procs, my_id, comms);
    } else if (strcmp(argv[i], "bandwidth") == 0) {
      bandwidth_test(num_procs, my_id, comms);
    } else if (strcmp(argv[i], "alltoall") == 0) {
      alltoall_test(num_procs, my_id);
    } else if (strcmp(argv[i], "alltoallv") == 0) {
      alltoallv_test(num_procs, my_id);
    } else if (strcmp(argv[i], "cpu") == 0) {
      cpu_test(num_procs, my_id);
    } else if (strcmp(argv[i], "gemm") == 0) {
      gemm_test(num_procs, my_id);
    }
  }

  if (my_id == 0) {
    free(all_hostnames);
  }

  MPI_Finalize();
  return 0;
}
