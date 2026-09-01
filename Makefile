# =============================================================================
# Makefile for the MDSP (Minimum Distance Superset Problem) solver
# Requires IBM ILOG CPLEX Optimization Studio (Concert Technology, C++ API)
# =============================================================================
#
# NOTE FOR TESTING:
# The target below is isolated for ThornArcPruner unit tests. It compiles only the
# pruner source and the standalone test file, so the full program is not affected.
# This is intentionally separate from the main solver build and can be removed
# after testing finishes.
# =============================================================================

CPLEXDIR      = /opt/ibm/ILOG/CPLEX_Studio2211/cplex
CONCERTDIR    = /opt/ibm/ILOG/CPLEX_Studio2211/concert

SYSTEM        = x86-64_linux
LIBFORMAT     = static_pic

CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)

CXX           = g++
CXXFLAGS      = -O2 -std=c++17 -Wall -DIL_STD \
                -I$(CPLEXDIR)/include -I$(CONCERTDIR)/include -Iinclude

LDFLAGS       = -L$(CPLEXLIBDIR) -L$(CONCERTLIBDIR) \
                -lilocplex -lconcert -lcplex -lm -lpthread -ldl

SRC           = src/main.cpp \
                src/core/Reader.cpp \
                src/bounds/Bounds.cpp \
                src/bounds/ThornArcPruner.cpp \
                src/bounds/CyclicLowerBound.cpp \
                src/bounds/ArithSequenceUB.cpp \
                src/bounds/LNSUpperBound.cpp \
                src/models/Model.cpp \
                src/models/MaxModel.cpp \
                src/models/FeasModel.cpp \
                src/models/BinarySearchSolver.cpp \
                src/runner/CsvExporter.cpp \
                src/runner/BatchRunner.cpp

OBJ           = $(SRC:.cpp=.o)
TARGET        = bin/mdsp_solver

.PHONY: all clean run run-batch test-thorn

all: $(TARGET)

# Standalone unit test for ThornArcPruner.
# This does not link the main solver and therefore does not alter the rest of the program.
test-thorn:
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -o bin/test_thorn_arc_pruner tests/thorn_arc_pruner_tests.cpp src/bounds/ThornArcPruner.cpp
	./bin/test_thorn_arc_pruner

$(TARGET): $(OBJ)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET) data/full-5-15-1.txt 3600

run-batch: $(TARGET)
	./$(TARGET) --batch data/ --out results.csv --time 3600

clean:
	rm -rf bin $(OBJ) results.csv
