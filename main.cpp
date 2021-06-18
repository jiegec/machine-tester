#include <mpi.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <vector>
#include <tuple>

#include "latency.h"
#include "bandwidth.h"
#include "alltoall.h"
#include "stats.h"

int main(int argc, char *argv[])
{
    int num_procs = 0, my_id = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_id);

    if (my_id == 0)
    {
        printf("Running with %d processes\n", num_procs);

        int version = 0, subversion = 0;
        MPI_Get_version(&version, &subversion);

        char buffer[MPI_MAX_LIBRARY_VERSION_STRING] = {0};
        int len = 0;
        MPI_Get_library_version(buffer, &len);

        printf("Using MPI %d.%d library %s\n", version, subversion, buffer);
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

    // pairs of communications
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


    latency_test(num_procs, my_id, comms);
    bandwidth_test(num_procs, my_id, comms);
    alltoall_test(num_procs, my_id);

    if (my_id == 0)
    {
        free(all_hostnames);
    }

    MPI_Finalize();
    return 0;
}