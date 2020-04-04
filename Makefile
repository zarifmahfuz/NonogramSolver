CC=g++
LFLAGS=
CFLAGS=-c -Wall -O2 -std=c++14
OBJS= testSolver.o
PROGS= test

all: $(PROGS)

test: testSolver.o
	$(CC) testSolver.o -o test $(LFLAGS)

testSolver.o: testSolver.cpp buildpuzzle.h
	$(CC) testSolver.cpp -o testSolver.o $(CFLAGS)

clean:
	@rm -f $(PROGS) $(OBJS)

