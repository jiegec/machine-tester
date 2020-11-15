#include "stats.h"
#include "math.h"
#include <vector>
#include <tuple>

void stats(int num_procs, double *all_data, double *output_mean, double *output_var)
{
    int count = num_procs * (num_procs - 1) / 2;
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

    // calculate mean
    double sum = 0.0;
    for (auto [from, to] : comms)
    {
        sum += all_data[from * num_procs + to];
    }

    double mean = sum / count;
    *output_mean = mean;

    // calculate variance
    double variance = 0.0;
    for (auto [from, to] : comms)
    {
        double data = all_data[from * num_procs + to];
        variance += (data - mean) * (data - mean);
    }

    variance = sqrt(variance / count);
    *output_var = variance;
}