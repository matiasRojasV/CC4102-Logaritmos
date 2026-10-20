CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -pedantic -Iinclude

SRCS = src/main.cpp src/BTree.cpp src/Experiment.cpp
TARGET = tarea2

all:
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f *.o $(filter-out datos.bin, $(wildcard *.bin)) mi_programa