CXX := g++
MPICXX := mpicxx
CXXFLAGS := --std=c++17 -O3 -fopenmp
ifeq ($(BLAS), openblas)
CXXFLAGS += -DENABLE_BLAS -DENABLE_OPENBLAS
LDFLAGS := $(shell pkg-config --libs openblas)
else ifeq ($(BLAS),mkl)
CXXFLAGS += -DENABLE_BLAS -DENABLE_MKL
LDFLAGS :=  -L${MKLROOT}/lib/intel64 -Wl,--no-as-needed -lmkl_intel_lp64 -lmkl_gnu_thread -lmkl_core -lgomp -lpthread -lm -ldl
else ifeq ($(BLAS),)
else
$(error please use supported BLAS: openblas)
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
