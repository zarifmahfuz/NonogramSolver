CC=g++
LFLAGS=
CFLAGS=-c -Wall -O2 -std=c++14
OBJS= testSolver.o buildpuzzle.o
PROGS= test

all: $(PROGS)

test: testSolver.o buildpuzzle.o
	$(CC) testSolver.o buildpuzzle.o -o test $(LFLAGS)

testSolver.o: testSolver.cpp buildpuzzle.o
	$(CC) testSolver.cpp -o testSolver.o $(CFLAGS)

buildpuzzle.o: buildpuzzle.cpp buildpuzzle.h
	$(CC) buildpuzzle.cpp -o buildpuzzle.o $(CFLAGS)

clean:
	@rm -f $(PROGS) $(OBJS)

