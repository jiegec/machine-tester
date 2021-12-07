#include <cstdio>
#include <mpi.h>
#include <omp.h>

#ifdef ENABLE_MKL
#include <mkl.h>
#endif

#ifdef ENABLE_BLAS
extern "C" void sgemm_(const char *, const char *, int *, int *, int *, float *,
                       float *, int *, float *, int *, float *, float *, int *);
extern "C" void dgemm_(const char *, const char *, int *, int *, int *,
                       double *, double *, int *, double *, int *, double *,
                       double *, int *);

template <class T>
double gemm_test_generic(int num_procs, int my_id,
                         void (*gemm)(const char *, const char *, int *, int *,
                                      int *, T *, T *, int *, T *, int *, T *,
                                      T *, int *)) {
  int n = 4096;
  T gflops = 2 * (T)n * n * n / 1000000000;
  T *a = new T[n * n];
  T *b = new T[n * n];
  T *c = new T[n * n];

  T alpha = 1.0;
  T beta = 1.0;

  // warmup
  MPI_Barrier(MPI_COMM_WORLD);
  double start = MPI_Wtime();
  gemm("N", "N", &n, &n, &n, &alpha, a, &n, b, &n, &beta, c, &n);
  MPI_Barrier(MPI_COMM_WORLD);
  double elapsed = MPI_Wtime() - start;

  // run for roughly 2s
  int loop = 2 / elapsed;
  if (loop == 0) {
    loop = 1;
  }
  // make loop equal to all procs
  MPI_Bcast(&loop, 1, MPI_INT, 0, MPI_COMM_WORLD);

  MPI_Barrier(MPI_COMM_WORLD);
  start = MPI_Wtime();
  for (int i = 0; i < loop; i++) {
    gemm("N", "N", &n, &n, &n, &alpha, a, &n, b, &n, &beta, c, &n);
  }
  MPI_Barrier(MPI_COMM_WORLD);
  elapsed = MPI_Wtime() - start;

  double perf = gflops * loop * num_procs / elapsed;

  delete[] a;
  delete[] b;
  delete[] c;

  return perf;
}

void gemm_test(int num_procs, int my_id) {
  double perf;
  if (my_id == 0) {
    printf("Num Threads Per Process: %d\n", omp_get_max_threads());
  }

#ifdef ENABLE_MKL
  mkl_set_num_threads(omp_get_max_threads());
#endif

  perf = gemm_test_generic<float>(num_procs, my_id, sgemm_);
  if (my_id == 0) {
    printf("SGEMM Perf: %lf GF/s\n", perf);
  }

  perf = gemm_test_generic<double>(num_procs, my_id, dgemm_);
  if (my_id == 0) {
    printf("DGEMM Perf: %lf GF/s\n", perf);
  }

  return;
}

#else

void gemm_test(int num_procs, int my_id) {
  printf("BLAS is not linked. Run make with BLAS=library\n");
}

#endif
