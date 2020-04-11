#include <iostream>
#include <vector>
#include <ctime>

#include <fstream> 
#include <string> 
#include <sstream>   

#include "buildpuzzle.h"
#include "serialport.h"

using namespace std;

void print_stuff(Puzzle *puzzle, bool print_ranges, bool print_puzzle) {
	uint16_t rows = (*puzzle).n_rows;
	uint16_t cols = (*puzzle).n_cols;
	//cout << "Rows: " << rows << ", Cols: " << cols << endl;

	if (print_ranges) {
		for (uint16_t i=0; i<rows; i++) {
			vector<range> eachrow_runranges = (*puzzle).row_black_runs[i];
			for (uint16_t j=0; j<eachrow_runranges.size(); j++) {
				cout << "Row: " << i << ", Restriction: " << (*puzzle).row_restrictions[i][j] 
				<< ", Lower limit: " << eachrow_runranges[j].first << ", Upper limit: " 
				<< eachrow_runranges[j].second << endl;
			}
		}
		cout << endl;
		for (uint16_t i=0; i<cols; i++) {
			vector<range> eachcol_runranges = (*puzzle).col_black_runs[i];
			for (uint16_t j=0; j<eachcol_runranges.size(); j++) {
				cout << "Col: " << i << ", Restriction: " << (*puzzle).col_restrictions[i][j] 
				<< ", Lower limit: " << eachcol_runranges[j].first << ", Upper limit: " 
				<< eachcol_runranges[j].second << endl;
			}
		}
	}
	if (print_puzzle) {
		for (uint16_t i=0; i<rows; i++) {
			for (uint16_t j=0; j<cols; j++) {
				if ((*puzzle).cells[i*cols+j] == -1 ){
					cout << "-" << " ";
				}
				else if ((*puzzle).cells[i*cols+j] == 0){
					cout << "■" << " ";
				}
				else{
					cout << "□" << " ";
				}
			}
			cout << endl;
		}
	}
}


void read_input_file(char filename[], uint16_t &total_rows, uint16_t &total_cols,
	vector <vector<uint16_t>> &rows, vector <vector<uint16_t>> &columns){

	// open file
	ifstream test(filename);

	// store puzzle dimensions
	test >> total_rows >> total_cols;

	// iterate through rows first
	for (int r = 0; r < total_rows; r++){

		// make a vector for this line
		vector <uint16_t> temp; 
		uint16_t blacks_num; 

		
		bool endLine = 0;
		// read input
		while(endLine != 1 && test >> blacks_num){
			temp.push_back(blacks_num);

			// until hitting newline character
			if (test.peek()== '\n'){
				endLine = 1;
			}
		}
		rows.push_back(temp);

	}


	// columns second
	for (int c = 0; c < total_cols; c++){
		// make a vector for this line
		vector <uint16_t> temp; 
		uint16_t blacks_num; 

		bool endLine = 0;
		// read input
		while(endLine != 1 && test >> blacks_num){
			temp.push_back(blacks_num);

			// until hitting newline character
			if (test.peek()== '\n'){
				endLine = 1;
			}
		}
		
		columns.push_back(temp);
	}

	// close file
	test.close();
}

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

int main(int argc, char *argv[]){

    SerialPort Serial("/dev/ttyACM0");
	// stores puzzle dimensions
	uint16_t row_dim, col_dim;

	// creates vectors that hold restrictions in all rows and columns
	vector <vector<uint16_t>> rows, columns;

	// process input from textfile
	read_input_file(argv[1], row_dim, col_dim, rows, columns);

	// create nonogram object
	Puzzle nonogram(row_dim, col_dim, rows, columns);

	// starting counter
    clock_t time;
    time = clock();

	nonogram.initialize_run_ranges();

	// apply rule 1.1
	nonogram.check_overlap(nonogram.n_rows, nonogram.n_cols, &nonogram.row_restrictions, &nonogram.row_black_runs, false);
	nonogram.check_overlap(nonogram.n_cols, nonogram.n_rows, &nonogram.col_restrictions, &nonogram.col_black_runs, true);

	// apply rule 1.2 to the puzzle now
	nonogram.rule_2(1);
	nonogram.rule_2(2);

	// apply rule 1.3 to the puzzle now
	nonogram.rule_3(1);
	nonogram.rule_3(2);

	// apply rule 1.4 to the puzzle now
	nonogram.rule_4(1);
	nonogram.rule_4(2);

	// applying rule 1.5 on rows and columns,respectively 
	nonogram.expand_and_limit(nonogram.n_rows, nonogram.n_cols, &nonogram.row_restrictions, &nonogram.row_black_runs, false);
	nonogram.expand_and_limit(nonogram.n_cols, nonogram.n_rows, &nonogram.col_restrictions, &nonogram.col_black_runs, true);

	// applying rule 2.1 
	nonogram.rule_6(1);
	nonogram.rule_6(2);

	// applying rule 2.2
	nonogram.rule_7(1); 
	nonogram.rule_7(2);

	// applying rule 2.3
	nonogram.rule_8(1);
	nonogram.rule_8(2);

	// applying rule 3.1
	nonogram.fill_in_void(nonogram.n_rows, nonogram.n_cols, &nonogram.row_restrictions, &nonogram.row_black_runs, false);
	nonogram.fill_in_void(nonogram.n_cols, nonogram.n_rows, &nonogram.col_restrictions, &nonogram.col_black_runs, true);

	// appplying 3.2
	// on rows
	nonogram.mod_range_to_fit(nonogram.n_rows, nonogram.n_cols, &nonogram.row_restrictions, &nonogram.row_black_runs, false);
	// on cols
	nonogram.mod_range_to_fit(nonogram.n_cols, nonogram.n_rows, &nonogram.col_restrictions, &nonogram.col_black_runs, true);

	while (nonogram.solved_indicator < row_dim*col_dim ) {

		// just rows
		nonogram.check_overlap(nonogram.n_rows, nonogram.n_cols, &nonogram.row_restrictions, &nonogram.row_black_runs, false);
		nonogram.rule_2(1);
		nonogram.rule_3(1);
		nonogram.rule_4(1);
		nonogram.expand_and_limit(nonogram.n_rows, nonogram.n_cols, &nonogram.row_restrictions, &nonogram.row_black_runs, false);
		nonogram.rule_6(1);
		nonogram.rule_7(1);
		nonogram.rule_8(1);
		nonogram.fill_in_void(nonogram.n_rows, nonogram.n_cols, &nonogram.row_restrictions, &nonogram.row_black_runs, false);
		nonogram.mod_range_to_fit(nonogram.n_rows, nonogram.n_cols, &nonogram.row_restrictions, &nonogram.row_black_runs, false);
		nonogram.rule_11(1);

		// just columns
		nonogram.check_overlap(nonogram.n_cols, nonogram.n_rows, &nonogram.col_restrictions, &nonogram.col_black_runs, true);
		nonogram.rule_2(2);
		nonogram.rule_3(2);
		nonogram.rule_4(2);
		nonogram.expand_and_limit(nonogram.n_cols, nonogram.n_rows, &nonogram.col_restrictions, &nonogram.col_black_runs, true);
		nonogram.rule_6(2);
		nonogram.rule_7(2);
		nonogram.rule_8(2);
		nonogram.fill_in_void(nonogram.n_cols, nonogram.n_rows, &nonogram.col_restrictions, &nonogram.col_black_runs, true);
		nonogram.mod_range_to_fit(nonogram.n_cols, nonogram.n_rows, &nonogram.col_restrictions, &nonogram.col_black_runs, true);
		nonogram.rule_11(2);

	}
    time = clock() - time;  // end counter
    int runtime = (time*1000000)/CLOCKS_PER_SEC;  // get runtime
	print_stuff(&nonogram, false, true);

	cout << "Time taken to solve the puzzle: " << runtime << " microseconds" << endl;

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
            sendMessage(to_string(row_dim), &Serial);
            // Wait for received acknowledgment, and display it
            line = Serial.readline();
            cout << "Received: " << line << endl;
            if (line == "R\n") {  // Row size was successfully sent
                cout << "Row Sent\n" << endl;
            }
            // Now to do the same thing with the column
            sendMessage(to_string(col_dim), &Serial);
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
            for (uint16_t i=0; i<row_dim*col_dim; i++) {
            	// Wait for acknowledgment to send from client
                line = Serial.readline();
                if (line == "S\n") {  // Send matrix state
                    cout << "Recieved send request" << endl;
                    sendMessage(to_string(nonogram.cells[i]), &Serial);
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
    print_stuff(&nonogram, false, true);

	return 0;
}