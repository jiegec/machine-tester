CXX := g++
MPICXX := mpicxx
CXXFLAGS := --std=c++17 -O3 -fopenmp
ifeq ($(BLAS), openblas)
	CXXFLAGS += -DENABLE_BLAS
	LDFLAGS := $(shell pkg-config --libs openblas)
else ifeq ($(BLAS), mkl)
	CXXFLAGS += -DENABLE_BLAS
else
endif

.PHONY: all clean

all: machine_tester memory_latency

machine_tester: main.o latency.o stats.o bandwidth.o alltoall.o alltoallv.o cpu.o gemm.o
	$(MPICXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

%.o: %.cpp
	$(MPICXX) $(CXXFLAGS) -c $^ -o $@

memory_latency: memory_latency.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -rf *.o machine_tester memory_latency
