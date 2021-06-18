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

// latency test
const int loop_times = 1024;
const int skip_times = 256;

void latency_test(int num_procs, int my_id, const std::vector<std::tuple<int, int>> &comms)
{
    double *latency = nullptr;
    if (my_id == 0)
    {
        latency = (double *)malloc(sizeof(double) * num_procs * num_procs);
        assert(latency != nullptr);
    }

    static char send_buffer[M] = {0};
    static char recv_buffer[M] = {0};

    double *latency_buffer = nullptr;
    if (my_id == 0)
    {
        latency_buffer = (double *)malloc(sizeof(double) * num_procs);
        assert(latency_buffer != nullptr);
    }

    for (int sum = 1; sum <= num_procs + num_procs - 3; sum++)
    {
        double elapsed = 0.0;
        MPI_Status status;
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
                    // 2 rtt
                    MPI_Send(send_buffer, N, MPI_CHAR, to, 0, MPI_COMM_WORLD);
                    MPI_Recv(recv_buffer, N, MPI_CHAR, to, 0, MPI_COMM_WORLD, &status);
                }
            }
            else
            {
                for (int i = 0; i < loop_times + skip_times; i++)
                {
                    MPI_Recv(recv_buffer, N, MPI_CHAR, to, 0, MPI_COMM_WORLD, &status);
                    MPI_Send(send_buffer, N, MPI_CHAR, to, 0, MPI_COMM_WORLD);
                }
            }
        }

        double time_end = MPI_Wtime();
        MPI_Barrier(MPI_COMM_WORLD);
        elapsed = time_end - time_start;
        MPI_Gather(&elapsed, 1, MPI_DOUBLE, latency_buffer, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        if (my_id == 0)
        {
            for (int i = 0; i < num_procs; i++)
            {
                double actual_elapsed = latency_buffer[i];
                int from = i;
                int to = sum - i;
                if (from < to && 0 <= to && to < num_procs)
                {
                    double us = actual_elapsed * 1000 * 1000 / 2 / loop_times;
                    printf("%d <-> %d: %.2fus\n", from, to, us);
                    latency[from * num_procs + to] = us;
                    fflush(stdout);
                }
            }
        }
    }

    // stats
    int count = num_procs * (num_procs - 1) / 2;
    if (my_id == 0)
    {
        double mean, variance;
        stats(num_procs, latency, &mean, &variance);
        printf("Latency mean %.2fus, var %.2fus\n", mean, variance);

        // find anomaly
        for (auto [from, to] : comms)
        {
            double data = latency[from * num_procs + to];
            if (abs(data - mean) > 3 * variance)
            {
                printf("Found anomaly: %d <-> %d %.2fus\n", from, to, data);
            }
        }
        fflush(stdout);
    }

    if (my_id == 0)
    {
        free(latency_buffer);
        free(latency);
    }

    return;
}