#include <mpi.h>
#include <set>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

int num_cores()
{
    return sysconf(_SC_NPROCESSORS_ONLN);
}

void cpu_test(int num_procs, int my_id)
{
    int cores = num_cores();

    // gather cores
    int *all_cores = nullptr;
    if (my_id == 0)
    {
        all_cores = (int *)malloc(sizeof(int) * num_procs);
        assert(all_cores != nullptr);
    }

    MPI_Gather(&cores, 1, MPI_INT, all_cores, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (my_id == 0)
    {
        std::set<int> num_cores;
        for (int i = 0; i < num_procs; i++)
        {
            num_cores.insert(all_cores[i]);
        }
        if (num_cores.size() == 1)
        {
            printf("All hosts have the same core count: %d\n", all_cores[0]);
        } else {
            printf("Not all hosts have the same core count:");
            for (auto core : num_cores) {
                printf(" %d", core);
            }
            printf("\n");
        }
        free(all_cores);
    }
}