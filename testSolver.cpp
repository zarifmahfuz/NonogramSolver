#include <iostream>
#include <vector>

#include <fstream> 
#include <string> 
#include <sstream>   

#include "buildpuzzle.h"

using namespace std;

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

int main(int argc, char *argv[]){

	// stores puzzle dimensions
	uint16_t row_dim, col_dim;

	// creates vectors that hold restrictions in all rows and columns
	vector <vector<uint16_t>> rows, columns;

	// process input from textfile
	read_input_file(argv[1], row_dim, col_dim, rows, columns);

	// create nonogram object
	Puzzle nonogram(row_dim, col_dim, rows, columns);

	nonogram.initialize_run_ranges();

	nonogram.check_overlap(nonogram.n_rows, nonogram.n_cols, &nonogram.row_restrictions, &nonogram.row_black_runs, false);

	cout << " CHECK COLS:" << endl;

	nonogram.check_overlap(nonogram.n_cols, nonogram.n_rows, &nonogram.col_restrictions, &nonogram.col_black_runs, true);

	return 0;
}