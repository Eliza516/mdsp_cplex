# =============================================================================
# Makefile for the MDSP (Minimum Distance Superset Problem) solver
# Requires IBM ILOG CPLEX Optimization Studio (Concert Technology, C++ API)
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
                src/models/FeasModel.cpp \
                src/models/BinarySearchSolver.cpp \
                src/runner/CsvExporter.cpp \
                src/runner/BatchRunner.cpp

OBJ           = $(SRC:.cpp=.o)
TARGET        = bin/mdsp_solver

.PHONY: all clean run run-batch

all: $(TARGET)

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
