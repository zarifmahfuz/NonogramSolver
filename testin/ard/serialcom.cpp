#include "serialcom.h"

// max size of buffer, including null terminator
const uint16_t buf_size = 256;
// current number of chars in buffer, not counting null terminator
uint16_t buf_len = 0;

// input buffer
char* buffer = (char *) malloc(buf_size);
void process_line() {
  // print what's in the buffer back to server
  Serial.print("Got: ");
  Serial.println(buffer);

  // clear the buffer
  buf_len = 0;
  buffer[buf_len] = 0;
}

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

int32_t str_to_int32(char delimiter) {
  bool done = false;
  int32_t result;

  String inString = "";

  while (!done) {
    int inChar = Serial.read();
    if (isDigit(inChar) || inChar == 45) {
      // convert the incoming byte to a char and add it to the string:
      inString += (char)inChar;
    }
    // if you get a newline save the string's value:
    if (inChar == delimiter) {
      done = true;
      result = inString.toInt();
      // clear the string for new input:
      inString = "";
    }
  }

  return result;
}

int rowSize() {
    int row;
    if (wait_on_serial(1, 1000)) {
        //String row_size = Serial.read();
        //return row_size.toInt();
        //char in_char = Serial.read();

         row = str_to_int32('\n');

    }
        return row;
}

int colSize() {
    int col;
    if (wait_on_serial(1, 1000)) {
        col = str_to_int32('\n');

    }
        return col;
}

int** solvedNono(int row_size, int col_size) {
    if (wait_on_serial(1, 10000)) {

        int** nonogram = 0;

        nonogram = new int*[row_size];

        for (int i = 0; i < row_size; i++) {
        	if (wait_on_serial(1, 5000)) {
        	    nonogram[i] = new int[col_size];
        	    for (int j = 0; j < col_size; j++) {
        	        int val = str_to_int32('\n');
        	        nonogram[i][j] = val;
        	    }
            }
        }
        return nonogram;
    }
}