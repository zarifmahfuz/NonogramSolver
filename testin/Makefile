CC=g++
LFLAGS=
CFLAGS=-c -Wall -std=c++11 -O2
OBJS= communication.o serialport.o
EXE=server

$(EXE): $(OBJS)
	$(CC) $(OBJS) -o $(EXE) $(LFLAGS)

communication.o: communication.cpp serialport.h
	$(CC) communication.cpp -o communication.o $(CFLAGS)

serialport.o: serialport.h serialport.cpp
	g++ serialport.cpp -o serialport.o $(CFLAGS)

clean:
	@rm $(OBJS) $(EXE)
