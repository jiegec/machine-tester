#include <cstdio>
#include <mpi.h>
#include <omp.h>

#ifdef ENABLE_BLAS
extern "C" void dgemm_(const char *, const char *, int *, int *, int *,
                       double *, double *, int *, double *, int *, double *,
                       double *, int *);
void gemm_test(int num_procs, int my_id) {
  int n = 4096;
  double gflops = 2 * (double)n * n * n / 1000000000;
  double *a = new double[n * n];
  double *b = new double[n * n];
  double *c = new double[n * n];

  double alpha = 1.0;
  double beta = 1.0;

  int loop = 4;
  if (my_id == 0) {
    printf("Num Threads Per Process: %d\n", omp_get_max_threads());
  }

  // warmup
  dgemm_("N", "N", &n, &n, &n, &alpha, a, &n, b, &n, &beta, c, &n);

  MPI_Barrier(MPI_COMM_WORLD);
  double start = MPI_Wtime();
  for (int i = 0; i < loop; i++) {
    dgemm_("N", "N", &n, &n, &n, &alpha, a, &n, b, &n, &beta, c, &n);
  }
  MPI_Barrier(MPI_COMM_WORLD);
  double elapsed = MPI_Wtime() - start;

  if (my_id == 0) {
    printf("DGEMM Perf: %lf GF/s\n", gflops * loop * num_procs / elapsed);
  }
  return;
}

#else

void gemm_test(int num_procs, int my_id) {
  printf("BLAS is not linked. Run make with BLAS=library\n");
}

#endif