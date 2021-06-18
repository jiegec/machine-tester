CXX := mpicxx
CXXFLAGS := --std=c++17 -O3

.PHONY: all clean

all: machine_tester

machine_tester: main.o latency.o stats.o bandwidth.o alltoall.o
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

clean:
	rm -rf *.o machine_tester
