CXX = g++
CXXFLAGS = -std=c++20 -O0 -g -Wall -Wextra -D_GLIBCXX_DEBUG
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

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $^
	@echo "✓ Compilación exitosa: $(TARGET)"

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	@rm -rf $(BINDIR)
	@echo "✓ Archivos de compilación eliminados"

run: $(TARGET)
	@mkdir -p results
	./$(TARGET)

debug: CXXFLAGS += -g -O0
debug: clean all

help:
	@echo "Targets disponibles:"
	@echo "  make all       - Compilar el proyecto"
	@echo "  make clean     - Eliminar archivos compilados"
	@echo "  make run       - Compilar y ejecutar"
	@echo "  make debug     - Compilar con símbolos de debug"
	@echo "  make help      - Mostrar esta ayuda"
