CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra -pedantic -Iinclude

SRCS = src/main.cpp src/BTree.cpp src/Experiment.cpp
TARGET = T2

all:
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET) *.o
	rm -f $(filter-out datos.bin, $(wildcard *.bin))