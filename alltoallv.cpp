#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

// buffer
const int N = 1024;
const int M = 262144;

// alltoallv test
const int loop_times = 1024;
const int skip_times = 256;

void alltoallv_test(int num_procs, int my_id)
{
    char *send_buffer = (char *)malloc(sizeof(char) * M * num_procs);
    char *recv_buffer = (char *)malloc(sizeof(char) * M * num_procs);
    int *send_counts = (int*)malloc(sizeof(int) * num_procs);
    int *send_displs = (int*)malloc(sizeof(int) * num_procs);
    int *recv_counts = (int*)malloc(sizeof(int) * num_procs);
    int *recv_displs = (int*)malloc(sizeof(int) * num_procs);

    for (int i = 0;i < num_procs;i ++) {
        send_counts[i] = M;
        send_displs[i] = M * i;
        recv_counts[i] = M;
        recv_displs[i] = M * i;
    }

    double elapsed = 0.0;
    MPI_Barrier(MPI_COMM_WORLD);
    double time_start = 0.0;

    for (int i = 0; i < loop_times + skip_times; i++)
    {
        if (i == skip_times)
        {
            time_start = MPI_Wtime();
        }

        MPI_Alltoallv(send_buffer, send_counts, send_displs, MPI_CHAR, recv_buffer, recv_counts, recv_displs, MPI_CHAR, MPI_COMM_WORLD);
    }

    double time_end = MPI_Wtime();
    MPI_Barrier(MPI_COMM_WORLD);
    elapsed = time_end - time_start;

    if (my_id == 0)
    {
        double gb = (double)M * num_procs * (num_procs - 1) * loop_times / elapsed / 1000 / 1000 / 1000 * 8;
        printf("Alltoallv bandwidth: %.2fGbps\n", gb);
        fflush(stdout);
    }

    free(send_buffer);
    free(recv_buffer);
    free(send_counts);
    free(send_displs);
    free(recv_counts);
    free(recv_displs);

    return;
}