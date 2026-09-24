CXX = g++
CXXFLAGS = -std=c++20 -O3 -Wall -Wextra
CPPFLAGS = -I./include
BINDIR = build
SRCDIR = src
OBJDIR = $(BINDIR)/obj

SOURCES = $(SRCDIR)/Graph.cpp \
          $(SRCDIR)/BinomialHeap.cpp \
          $(SRCDIR)/FibonacciHeap.cpp \
          $(SRCDIR)/PrimAlgorithm.cpp \
          $(SRCDIR)/main.cpp

OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
TARGET = $(BINDIR)/prim_experiment

.PHONY: all clean run debug help

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $^
	@echo "✓ Compilación exitosa (Release -O3): $(TARGET)"

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	@rm -rf $(BINDIR)
	@echo "✓ Archivos de compilación eliminados"

run: $(TARGET)
	@mkdir -p results
	./$(TARGET)

debug: CXXFLAGS = -std=c++20 -O0 -g -Wall -Wextra -D_GLIBCXX_DEBUG
debug: clean all