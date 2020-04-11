CC=g++
LFLAGS=
CFLAGS=-c -Wall -O2 -std=c++14
OBJS= testSolver.o buildpuzzle.o serialport.o
PROGS= test

all: $(PROGS)

test: testSolver.o buildpuzzle.o serialport.o
	$(CC) testSolver.o buildpuzzle.o serialport.o -o test $(LFLAGS)

testSolver.o: testSolver.cpp buildpuzzle.h serialport.h
	$(CC) testSolver.cpp -o testSolver.o $(CFLAGS)

buildpuzzle.o: buildpuzzle.cpp buildpuzzle.h
	$(CC) buildpuzzle.cpp -o buildpuzzle.o $(CFLAGS)

serialport.o: serialport.h serialport.cpp
	$(CC) serialport.cpp -o serialport.o $(CFLAGS)

clean:
	@rm -f $(PROGS) $(OBJS)

