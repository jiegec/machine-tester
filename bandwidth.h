#ifndef __BANDWIDTH_H__
#define __BANDWIDTH_H__

#include <vector>
#include <tuple>

void bandwidth_test(int num_procs, int my_id, const std::vector<std::tuple<int, int>> &comms);

#endif