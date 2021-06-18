#ifndef __LATENCY_H__
#define __LATENCY_H__

#include <vector>
#include <tuple>

void latency_test(int num_procs, int my_id, const std::vector<std::tuple<int, int>> &comms);

#endif