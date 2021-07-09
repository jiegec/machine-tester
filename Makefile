CXX := g++
MPICXX := mpicxx
CXXFLAGS := --std=c++17 -O3

.PHONY: all clean

all: machine_tester memory_latency

machine_tester: main.o latency.o stats.o bandwidth.o alltoall.o alltoallv.o cpu.o
	$(MPICXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp
	$(MPICXX) $(CXXFLAGS) -c $^ -o $@

memory_latency: memory_latency.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -rf *.o machine_tester memory_latency
