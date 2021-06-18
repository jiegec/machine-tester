#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <vector>
#include <tuple>

#include "stats.h"

// buffer
const int N = 1024;
const int M = 262144;

// bandwidth test
const int loop_times = 1024;
const int skip_times = 256;
const int batch_size = 64;

void bandwidth_test(int num_procs, int my_id, const std::vector<std::tuple<int, int>> &comms)
{
    double *bandwidth = nullptr;
    if (my_id == 0)
    {
        bandwidth = (double *)malloc(sizeof(double) * num_procs * num_procs);
        assert(bandwidth != nullptr);
    }

    static char send_buffer[M] = {0};
    static char recv_buffer[M] = {0};

    double *bandwidth_buffer = nullptr;
    MPI_Request request[batch_size];
    MPI_Status status[batch_size];
    if (my_id == 0)
    {
        bandwidth_buffer = (double *)malloc(sizeof(double) * num_procs);
        assert(bandwidth_buffer != nullptr);
    }

    for (int sum = 1; sum <= num_procs + num_procs - 3; sum++)
    {
        double elapsed = 0.0;
        MPI_Barrier(MPI_COMM_WORLD);
        double time_start = 0.0;

        int from = my_id;
        int to = sum - from;
        if (0 <= to && to < num_procs && from != to)
        {
            if (from < to)
            {
                for (int i = 0; i < loop_times + skip_times; i++)
                {
                    if (i == skip_times)
                    {
                        time_start = MPI_Wtime();
                    }

                    for (int j = 0; j < batch_size; j++)
                    {
                        MPI_Isend(send_buffer, M, MPI_CHAR, to, 0, MPI_COMM_WORLD, &request[j]);
                    }
                    MPI_Waitall(batch_size, request, status);
                    MPI_Recv(recv_buffer, 1, MPI_CHAR, to, 1, MPI_COMM_WORLD, &status[0]);
                }
            }
            else
            {
                for (int i = 0; i < loop_times + skip_times; i++)
                {
                    for (int j = 0; j < batch_size; j++)
                    {
                        MPI_Irecv(recv_buffer, M, MPI_CHAR, to, 0, MPI_COMM_WORLD, &request[j]);
                    }
                    MPI_Waitall(batch_size, request, status);
                    MPI_Send(send_buffer, 1, MPI_CHAR, to, 1, MPI_COMM_WORLD);
                }
            }
        }

        double time_end = MPI_Wtime();
        MPI_Barrier(MPI_COMM_WORLD);
        elapsed = time_end - time_start;

        MPI_Gather(&elapsed, 1, MPI_DOUBLE, bandwidth_buffer, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        if (my_id == 0)
        {
            for (int i = 0; i < num_procs; i++)
            {
                double actual_elapsed = bandwidth_buffer[i];
                int from = i;
                int to = sum - i;
                if (from < to && 0 <= to && to < num_procs)
                {
                    double gb = (double)M * batch_size * loop_times / actual_elapsed / 1000 / 1000 / 1000 * 8;
                    printf("%d <-> %d: %.2fGbps\n", from, to, gb);
                    bandwidth[from * num_procs + to] = gb;
                    fflush(stdout);
                }
            }
        }
    }

    // stats
    if (my_id == 0)
    {
        // calculate mean
        double mean, variance;
        stats(num_procs, bandwidth, &mean, &variance);
        printf("Bandwidth mean %.2fGbps, var %.2fGbps\n", mean, variance);

        // find anomaly
        for (auto [from, to] : comms)
        {
            double data = bandwidth[from * num_procs + to];
            if (abs(data - mean) > 3 * variance)
            {
                printf("Found anomaly: %d <-> %d %.2fGbps\n", from, to, data);
            }
        }
        fflush(stdout);
    }

    if (my_id == 0)
    {
        free(bandwidth_buffer);
        free(bandwidth);
    }
    return;
}