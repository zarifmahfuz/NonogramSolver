#include <iostream>
#include <string>
#include "serialport.h"

using namespace std;

// Borrowed from Paola and Balreet's Assignment 2 Part II: Driving Route Finder
void sendMessage(string command, SerialPort *Serial) {
    // Showing what is being written to client, as well as writing to client
    cout << "Writing: " << command << " " << endl;
    bool successfulWrite = Serial->writeline(command +"\n");

    // was it successfully written?
    while (!successfulWrite) {
        // if it wasn't, keep writing it
        successfulWrite = Serial->writeline(command +"\n");
    }
}

int main() {
    SerialPort Serial("/dev/ttyACM0");

    // Running the solver and getting the solved matrix to send

    // This is for testing purposes, once its fully combined we can delete.
    int runtime = 1200;
    int row = 3, col = 3;
    int nono[row][col];

    nono[0][0] = 1;
    nono[1][0] = 0;
    nono[2][0] = 1;
    nono[0][1] = 0;
    nono[1][1] = 1;
    nono[2][1] = 1;
    nono[0][2] = 1;
    nono[1][2] = 0;
    nono[2][2] = 0;

    // Begin communication loop

    bool done = true;  // Bool to leave loop once communication is complete
    while (done) {
        cout << "Awaiting Results" << endl;  // Waiting for initial ACK
        // Read what was sent from client and display it.
        string line = Serial.readline();
        cout << "Received: " << line << endl;

        // If the acknowledgment character is sent, we are good to go
        if (line == "A\n") {
            // Begin by sending row information
            cout << "sending row" << endl;
            sendMessage(to_string(row), &Serial);
            // Wait for received acknowledgment, and display it
            line = Serial.readline();
            cout << "Received: " << line << endl;
            if (line == "R\n") {  // Row size was successfully sent
                cout << "Row Sent\n" << endl;
            }
            // Now to do the same thing with the column
            sendMessage(to_string(col), &Serial);
            line = Serial.readline();
            cout << "Received: " << line << endl;
            if (line == "R\n") {
                cout << "Column Sent\n" << endl;
            }
            // And do the same with the runtime of DFS
            sendMessage(to_string(runtime), &Serial);
            line = Serial.readline();
            cout << "Received: " << line << endl;
            if (line == "R\n") {
                cout << "Runtime Sent\n" << endl;
            }
            // Now to send the contents of the solved nonogram matrix
            for (int i = 0; i < row; i++) {
                for (int j = 0; j < col; j++) {
                    // Wait for acknowledgment to send from client
                    line = Serial.readline();
                    if (line == "S\n") {  // Send matrix state
                        cout << "Recieved send request" << endl;
                        sendMessage(to_string(nono[i][j]), &Serial);
                    }
                }
            }
            // Wait for acknowledgment of fully sent matrix
            line = Serial.readline();
            cout << "Received: " << line << endl;
            if (line == "M\n") {  // Send end of communication signal
                cout << "Matrix Sent\n" << endl;
                sendMessage("E", &Serial);
            }
            // Receive confirmation of end of communication
            line = Serial.readline();
            cout << "Received: " << line << endl;
            if (line == "E\n") {
                done = false;  // End communication
            }
        }
    }
    return 0;
}
