#include <mpi.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <vector>
#include <tuple>

int main(int argc, char *argv[])
{
    int num_procs = 0, my_id = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_id);

    if (my_id == 0)
    {
        printf("Running with %d processes\n", num_procs);
    }

    // get hostname
    char my_hostname[HOST_NAME_MAX] = {0};
    int res = gethostname(my_hostname, sizeof(my_hostname));
    if (res < 0)
    {
        perror("Failed to call gethostname()");
    }

    // gather hostnames
    char *all_hostnames = nullptr;
    if (my_id == 0)
    {
        all_hostnames = (char *)malloc(HOST_NAME_MAX * num_procs);
        assert(all_hostnames != nullptr);
    }
    MPI_Gather(my_hostname, HOST_NAME_MAX, MPI_CHAR, all_hostnames, HOST_NAME_MAX, MPI_CHAR, 0, MPI_COMM_WORLD);
    if (my_id == 0)
    {
        for (int i = 0; i < num_procs; i++)
        {
            printf("Host %d: %s\n", i, &all_hostnames[i * HOST_NAME_MAX]);
        }
    }

    // latency test
    const int N = 1024;
    const int loop_times = 10240;
    const int skip_times = 2048;

    double *latency = nullptr;
    if (my_id == 0)
    {
        latency = (double *)malloc(sizeof(double) * num_procs * num_procs);
        assert(latency != nullptr);
    }

    std::vector<std::tuple<int, int>> comms;
    for (int from = 0; from < num_procs; from++)
    {
        for (int to = 0; to < num_procs; to++)
        {
            if (from >= to)
            {
                continue;
            }
            comms.emplace_back(from, to);
        }
    }

    char send_buffer[N] = {0};
    char recv_buffer[N] = {0};
    for (auto [from, to] : comms)
    {
        double elapsed = 0.0;
        MPI_Status status;
        MPI_Barrier(MPI_COMM_WORLD);
        double time_start = 0.0;

        if (my_id == from)
        {
            for (int i = 0; i < loop_times + skip_times; i++)
            {
                if (i == skip_times)
                {
                    time_start = MPI_Wtime();
                }
                MPI_Send(send_buffer, N, MPI_CHAR, to, 0, MPI_COMM_WORLD);
                MPI_Recv(recv_buffer, N, MPI_CHAR, to, 0, MPI_COMM_WORLD, &status);
            }
        }
        else if (my_id == to)
        {
            for (int i = 0; i < loop_times + skip_times; i++)
            {
                MPI_Recv(recv_buffer, N, MPI_CHAR, from, 0, MPI_COMM_WORLD, &status);
                MPI_Send(send_buffer, N, MPI_CHAR, from, 0, MPI_COMM_WORLD);
            }
        }

        double time_end = MPI_Wtime();
        MPI_Barrier(MPI_COMM_WORLD);
        if (my_id == from)
        {
            elapsed = time_end - time_start;
        }

        double actual_elapsed = 0.0;
        MPI_Reduce(&elapsed, &actual_elapsed, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
        if (my_id == 0)
        {
            double us = actual_elapsed * 1000 * 1000 / 2 / loop_times;
            // printf("%d <-> %d: %.2fus\n", from, to, us);
            latency[from * num_procs + to] = us;
        }
    }

    // stats
    if (my_id == 0)
    {
        // calculate mean
        double sum = 0.0;
        int count = num_procs * (num_procs - 1) / 2;
        for (auto [from, to] : comms)
        {
            double data = latency[from * num_procs + to];
            sum += data;
        }

        double mean = sum / count;

        // calculate variance
        double variance = 0.0;
        for (auto [from, to] : comms)
        {
            double data = latency[from * num_procs + to];
            variance += (data - mean) * (data - mean);
        }

        variance = sqrt(variance / count);
        printf("Latency mean %.2fus, var %.2f us\n", mean, variance);

        // find anomaly
        for (auto [from, to] : comms)
        {
            double data = latency[from * num_procs + to];
            if (abs(data - mean) > 3 * variance)
            {
                printf("Found anomaly: %d <-> %d %.2fus\n", from, to, data);
            }
        }
    }

    if (my_id == 0)
    {
        free(all_hostnames);
        free(latency);
    }
    return 0;
}