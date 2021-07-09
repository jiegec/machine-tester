// reference:
// https://github.com/intel/lmbench/blob/master/src/lat_mem_rd.c

#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <math.h>
#include <vector>
#include <algorithm>

// learned from lmbench
#define ONE p = (char **)*p;
#define FIVE ONE ONE ONE ONE ONE
#define TEN FIVE FIVE
#define FIFTY TEN TEN TEN TEN TEN
#define HUNDRED FIFTY FIFTY

uint64_t get_time_ns()
{
    struct timeval tv = {};
    gettimeofday(&tv, nullptr);
    return (uint64_t)tv.tv_sec * 1000000000 + (uint64_t)tv.tv_usec * 1000;
}

// measure memory latency with pointer chasing
void test(int size)
{
    int count = size / sizeof(char *);
    char **buffer = new char *[count];
    int *index = new int[count];

    // init index and shuffle
    for (int i = 0; i < count; i++)
    {
        index[i] = i;
    }
    for (int i = 1; i < count; i++)
    {
        int j = rand() % i;
        int temp = index[i];
        index[i] = index[j];
        index[j] = temp;
    }

    // init circular list
    for (int i = 0; i < count - 1; i++)
    {
        buffer[index[i]] = (char *)&buffer[index[i + 1]];
    }
    buffer[index[count - 1]] = (char *)&buffer[index[0]];

    const int warmup = 50000;
    const int iterations = 500000;
    char **p = (char **)buffer[0];

    // warmup
    for (int i = 0; i < warmup; i++)
    {
        HUNDRED;
    }

    // benchmark
    uint64_t before = get_time_ns();

    for (int i = 0; i < iterations; i++)
    {
        HUNDRED;
    }

    // avoid optimization
    *(volatile char *)*p;
    uint64_t after = get_time_ns();
    printf("%d,%.2f\n", size, (double)(after - before) / iterations / 100);
    fflush(stdout);

    delete[] index;
    delete[] buffer;
}

int main(int argc, char *argv[])
{
    int keys[] = {
        _SC_LEVEL1_DCACHE_SIZE,
        _SC_LEVEL2_CACHE_SIZE,
        _SC_LEVEL3_CACHE_SIZE};
    const char *names[] = {
        "L1d",
        "L2",
        "L3",
    };
    for (int i = 0; i < 3; i++)
    {
        long size = sysconf(keys[i]);
        if (size != -1) {
            printf("%s cache: %d bytes\n", names[i], size);
        }
    }

    test(1024);
    test(1024 * 2);
    test(1024 * 4);
    test(1024 * 16);
    test(1024 * 24);
    test(1024 * 32);
    test(1024 * 28);
    test(1024 * 36);
    test(1024 * 40);
    test(1024 * 44);
    test(1024 * 48);
    test(1024 * 64);
    test(1024 * 96);
    test(1024 * 128);
    test(1024 * 192);
    test(1024 * 224);
    test(1024 * 256);
    test(1024 * 288);
    test(1024 * 384);
    test(1024 * 512);
    test(1024 * 768);
    test(1024 * 1024);
    test(1024 * 1024 * 16);
    test(1024 * 1024 * 32);
    test(1024 * 1024 * 48);
    test(1024 * 1024 * 64);
    test(1024 * 1024 * 128);
    test(1024 * 1024 * 192);
    test(1024 * 1024 * 256);
    test(1024 * 1024 * 384);
    test(1024 * 1024 * 512);
    test(1024 * 1024 * 1024);
    return 0;
}