#include "serialcom.h"

/* 
    Description: Waits for a certain number of bytes on Serial3 or timeout
    Arguments:
        nbytes : the number of bytes we want
        timeout : timeout period ( ms ) ; specifying a negative number turns 
        off timeouts ( the function waits indefinitely

    Returns:
        True if the required number of bytes have arrived .

    Borrowed from: Paola and Balreet's Assignment 2 Part 1: Encrypted 
    Arduino Communication
*/
bool wait_on_serial(uint8_t nbytes, long timeout) {
  unsigned long deadline = millis() + timeout;  // wraparound not a problem
  while (Serial.available() < nbytes && (timeout <0 || millis() < deadline)) {
    delay(1);  // be nice , no busy loop
  }
  return Serial.available() >= nbytes;
}

int** solvedNono(int *dimensions) {
    /* Communicates with desktop C++ to read in information about the nonogram
    and returns a nonogram of 1s and 0s to be displayed on the tft display.
    Recieved help with returning a matrix from https://stackoverflow.com/quest
    ions/8617683/return-a-2d-array-from-a-function

    Arguments: *dimensions: array to store row and column sizes, runtime, and
                    an end of loop marker.
    
    Returns: nonogram: A pointer to a matrix of size row*col that contains 1s
                and 0s pertaining to how the nonogram will be displayed on the
                tft screen.
    */
    int** nonogram = 0;  // Initialize pointer, we will build this later on

    // Send acknowledgment to begin communication
    Serial.println("A");

    // Keep it running and monitor timeout
    if (wait_on_serial(1, 10000)) {
        String read = Serial.readString();  // Read input string for row size
        int row = read.toInt();  // Convert it to integer
        Serial.println("R");  // Print received signal

        // Repeat the same steps to get the column size and runtime
        read = Serial.readString();
        int col = read.toInt();
        Serial.println("R");

        // Runtime
        read = Serial.readString();
        int runtime = read.toInt();
        Serial.println("R");

        // Store the received input in dimensions so it can be analyzed wherever
        // it is called
        dimensions[0] = row;
        dimensions[1] = col;
        dimensions[2] = runtime;

        // Make an array of nonogram
        nonogram = new int*[row];
        for (int i=0; i < row; i++) {
            // Now make it a proper matrix
            nonogram[i] = new int[col];
            for (int j=0; j < col; j++) {
                // Send Acknowledgment to send a matrix input
                Serial.println("S");

                // Read value, convert to int and store in correct position
                read = Serial.readString();
                int val = read.toInt();
                nonogram[i][j] = val;
            }
        }
        Serial.println("M");  // Send signal for received matrix
        read = Serial.readString();  // Read signal for end of communication
        if (read == "E\n") {
            Serial.println("E");  // Send confirmation
            dimensions[3] = 0;  // Change loop marker
        }
    }
    return nonogram;  // Return matrix
}
