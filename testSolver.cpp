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

	//cout << " CHECK COLS:" << endl;

	nonogram.check_overlap(nonogram.n_cols, nonogram.n_rows, &nonogram.col_restrictions, &nonogram.col_black_runs, true);

	// apply rule 1.2 to the puzzle now
	nonogram.rule_2(1);
	nonogram.rule_2(2);

	// at the end of rule 1.2, cells 12, 17 should be empty
	// uncomment for verification of rule 1.2
	// for (uint16_t i=0; i<row_dim*col_dim; i++) {
	// 	cout << "Cell " << i << ", State: " << nonogram.cells[i] << endl;
	// }

	// apply rule 1.3 to the puzzle now
	nonogram.rule_3(1);
	nonogram.rule_3(2);

	// at the end of rule 1.3 cells 12, 17, 23, 25, 26 should be empty
	// uncomment for verification of rule 1.3
	// for (uint16_t i=0; i<row_dim*col_dim; i++) {
	// 	if (nonogram.cells[i] == 1) {
	// 		cout << "Cell " << i << ", State: " << nonogram.cells[i] << endl;
	// 	}
	// }

	// apply rule 1.4 to the puzzle now
	nonogram.rule_4(1);
	nonogram.rule_4(2);

	cout << "solved_indicator: " << nonogram.solved_indicator << endl;

	// rule 1.4 does not apply to the state of the puzzle at the end of 
	// rule 1.3, so the output of this for loop should be the same as the prev one
	// for (uint16_t i=0; i<row_dim*col_dim; i++) {
	// 	if (nonogram.cells[i] == 1) {
	// 		cout << "Cell " << i << ", State: " << nonogram.cells[i] << endl;
	// 	}
	// }

	// aplpying rule 1.5 on rows and columns,respectively 
	nonogram.expand_and_limit(nonogram.n_rows, nonogram.n_cols, &nonogram.row_restrictions, &nonogram.row_black_runs, false);

	// cout << " CHECK COLS:" << endl;
	nonogram.expand_and_limit(nonogram.n_cols, nonogram.n_rows, &nonogram.col_restrictions, &nonogram.col_black_runs, true);
	// cout << " DONE " << endl;

	// applying rule 3.1
	// on rows
	nonogram.fill_in_void(nonogram.n_rows, nonogram.n_cols, &nonogram.row_restrictions, &nonogram.row_black_runs, false);
	// on cols
	nonogram.fill_in_void(nonogram.n_cols, nonogram.n_rows, &nonogram.col_restrictions, &nonogram.col_black_runs, true);


	// appplying 3.2
	nonogram.mod_range_to_fit(nonogram.n_rows, nonogram.n_cols, &nonogram.row_restrictions, &nonogram.row_black_runs, false);
	// on cols
	// cout << " CHECK COLS:" << endl;
	nonogram.mod_range_to_fit(nonogram.n_cols, nonogram.n_rows, &nonogram.col_restrictions, &nonogram.col_black_runs, true);
	
	return 0;
}