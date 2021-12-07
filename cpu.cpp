#include <assert.h>
#include <map>
#include <mpi.h>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>

int num_cores() { return sysconf(_SC_NPROCESSORS_ONLN); }

std::string cpu_model() {
  char buffer[1024];
  FILE *fp = fopen("/proc/cpuinfo", "r");
  if (!fp) {
    return "unknown";
  }
  const char prefix[] = "model name\t";
  while (fgets(buffer, sizeof(buffer), fp)) {
    if (strncmp(buffer, prefix, sizeof(prefix) - 1) == 0) {
      std::string line = buffer;
      int index = line.find_first_of(':');
      if (index == std::string::npos) {
        return "unknown";
      }

      std::string res = line.substr(index + 1);
      // trim
      const char *trimmed = " \n";
      res.erase(res.find_last_not_of(trimmed) + 1);
      res.erase(0, res.find_first_not_of(trimmed));
      return res;
    }
  }
  fclose(fp);
  return "unknown";
}

void cpu_test(int num_procs, int my_id) {
  int cores = num_cores();

  // gather cores
  int *all_cores = nullptr;
  if (my_id == 0) {
    all_cores = (int *)malloc(sizeof(int) * num_procs);
    assert(all_cores != nullptr);
  }

  MPI_Gather(&cores, 1, MPI_INT, all_cores, 1, MPI_INT, 0, MPI_COMM_WORLD);
  if (my_id == 0) {
    std::set<int> num_cores;
    for (int i = 0; i < num_procs; i++) {
      num_cores.insert(all_cores[i]);
    }
    if (num_cores.size() == 1) {
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

  std::string model = cpu_model();
  printf("CPU model: %s\n", model.c_str());

  std::map<std::string, double> peak_dp;
  // core * freq * flop/cycle
  peak_dp["Intel(R) Xeon(R) CPU E5-2699 v4 @ 2.20GHz"] = 22 * 2.2 * 16;
  peak_dp["Intel(R) Xeon(R) CPU E5-2670 v3 @ 2.30GHz"] = 12 * 2.3 * 16;
  peak_dp["AMD EPYC 7742 64-Core Processor"] = 64 * 2.25 * 16;

  auto it = peak_dp.find(model);
  if (it != peak_dp.end()) {
    printf("Core Peak DP FLOPS: %.1f GFLOPS\n", it->second);
  }
}