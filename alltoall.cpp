#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

// buffer
const int N = 1024;
const int M = 262144;

// alltoall test
const int loop_times = 1024;
const int skip_times = 256;

void alltoall_test(int num_procs, int my_id)
{
    char *send_buffer = (char*)malloc(sizeof(char) * M * num_procs);
    char *recv_buffer = (char*)malloc(sizeof(char) * M * num_procs);

    double elapsed = 0.0;
    MPI_Barrier(MPI_COMM_WORLD);
    double time_start = 0.0;

    for (int i = 0; i < loop_times + skip_times; i++)
    {
        if (i == skip_times)
        {
            time_start = MPI_Wtime();
        }

        MPI_Alltoall(send_buffer, M, MPI_CHAR, recv_buffer, M, MPI_CHAR, MPI_COMM_WORLD);
    }

    double time_end = MPI_Wtime();
    MPI_Barrier(MPI_COMM_WORLD);
    elapsed = time_end - time_start;

    if (my_id == 0)
    {
        double gb = (double)M * num_procs * (num_procs - 1) * loop_times / elapsed / 1000 / 1000 / 1000 * 8;
        printf("Alltoall bandwidth: %.2fGbps\n", gb);
        fflush(stdout);
    }

    free(send_buffer);
    free(recv_buffer);

    return;
}