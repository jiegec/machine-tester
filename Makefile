CXX := g++
MPICXX := mpicxx
CXXFLAGS := --std=c++17 -O3 -fopenmp
ifeq ($(BLAS), openblas)
CXXFLAGS += -DENABLE_BLAS -DENABLE_OPENBLAS
LDFLAGS := $(shell pkg-config --libs openblas)
else ifeq ($(BLAS),mkl)
CXXFLAGS += -DENABLE_BLAS -DENABLE_MKL
LDFLAGS := -L${MKLROOT}/lib/intel64 -Wl,--no-as-needed -lmkl_intel_lp64 -lmkl_gnu_thread -lmkl_core -lgomp -lpthread -lm -ldl
else ifeq ($(BLAS),blis)
CXXFLAGS += -DENABLE_BLAS -DENABLE_BLIS
LDFLAGS := -L$(BLIS_PREFIX)/lib -lblis
else ifeq ($(BLAS),)
else
$(error please use supported BLAS: openblas)
endif

CONFIG_FILE := .config_$(BLAS)

.PHONY: all clean

all: machine_tester memory_latency

machine_tester: main.o latency.o stats.o bandwidth.o alltoall.o alltoallv.o cpu.o gemm.o
	$(MPICXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

$(CONFIG_FILE):
	rm -rf .config_*
	touch $@

%.o: %.cpp $(CONFIG_FILE)
	$(MPICXX) $(CXXFLAGS) -c $< -o $@

memory_latency: memory_latency.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -rf *.o machine_tester memory_latency
