#include <iostream>
#include <cassert>
#include <string>
#include "serialport.h"

using namespace std;

// Borrowed from Paola and Balreet's Assignment 2 Part II: Driving Route Finder
void sendMessage(string command, SerialPort *Serial) {
	// Showing what is being written to client, as well as writing to client
	cout << "Writing: " << command << " " << endl;
	bool successfulWrite = Serial->writeline(command +"\n");

	// was it successfullly written?
	while (!successfulWrite) {
		// if it wasn't, keep writing it
		successfulWrite = Serial->writeline(command +"\n");
	}
}

int main() {
    SerialPort Serial("/dev/ttyACM0");

    // Running the solver and getting the solved matrix to send 

    int row_size = 2, col_size = 2;
    int matrix[row_size][col_size];

    matrix[0][0] = 0;
    matrix[1][0] = 1;
    matrix[0][1] = 1;
    matrix[1][1] = 0;

    cout << "Sending Nonogram Board Dimensions" << endl;
    sendMessage(to_string(row_size), &Serial);
    cout << "Row Size " << to_string(row_size) << " sent" << endl;
    sendMessage(to_string(col_size), &Serial);
    cout << "Column Size " << col_size << " sent" << endl;
    cout << endl;

    cout << "Sending Solved Matrix" << endl;
    for (int i = 0; i < row_size; i++) {
        for (int j = 0; j < col_size; j++) {
            sendMessage(to_string(matrix[i][j]), &Serial);
        }
    }
    cout << "Matrix sent" << endl;
    return 0;
}