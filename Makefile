CC := gcc
CXX := g++

CFLAGS := -std=c17 -masm=att -msse4 -mavx2 -march=native -fopenmp -fverbose-asm -fno-asynchronous-unwind-tables -fno-exceptions -fcf-protection=none -O3
CXXFLAGS := -std=c++17 -O3 -Wall -Wextra -msse4.1 -mavx2 -march=native

PROB2_SRC := problem2/261110069-prob2.cpp
PROB3_SRC := problem3/261110069-prob3.cpp
PROB4_SRC := problem4/261110069-prob4.c

PROB2_EXE := problem2/261110069-prob2
PROB3_EXE := problem3/261110069-prob3
PROB4_EXE := problem4/261110069-prob4

.PHONY: all problem2 problem3 problem4 run2 run3 run4 clean

all: problem2 problem3 problem4

problem2: $(PROB2_EXE)

problem3: $(PROB3_EXE)

problem4: $(PROB4_EXE)

$(PROB2_EXE): $(PROB2_SRC)
	$(CXX) $(CXXFLAGS) $< -o $@

$(PROB3_EXE): $(PROB3_SRC)
	$(CXX) $(CXXFLAGS) $< -o $@

$(PROB4_EXE): $(PROB4_SRC)
	$(CC) $(CFLAGS) $< -o $@

run2: problem2
	./$(PROB2_EXE)

run3: problem3
	./$(PROB3_EXE)

run4: problem4
	cd problem4 && ./261110069-prob4

clean:
	rm -f $(PROB2_EXE) $(PROB3_EXE) $(PROB4_EXE)