CXX := mpicxx
CXXFLAGS := --std=c++17 -O3

all: machine_tester

machine_tester: main.o
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@