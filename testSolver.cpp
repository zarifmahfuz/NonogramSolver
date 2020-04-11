#include <iostream>
#include <vector>

#include <fstream> 
#include <string> 
#include <sstream>   

#include "buildpuzzle.h"

using namespace std;

void print_stuff(Puzzle *puzzle, bool print_ranges, bool print_puzzle) {
	uint16_t rows = (*puzzle).n_rows;
	uint16_t cols = (*puzzle).n_cols;

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

int main(int argc, char *argv[]){

	// stores puzzle dimensions
	uint16_t row_dim, col_dim;

	// creates vectors that hold restrictions in all rows and columns
	vector <vector<uint16_t>> rows, columns;

	// process input from textfile
	read_input_file(argv[1], row_dim, col_dim, rows, columns);

	// create nonogram object
	Puzzle nonogram(row_dim, col_dim, rows, columns);

	// get the range for all runs
	nonogram.initialize_run_ranges();

	// solve the puzzle

	while (nonogram.solved_indicator < row_dim*col_dim ) { 

		// RUN ALL ROW METHODS
		nonogram.check_overlap(nonogram.n_rows, nonogram.n_cols, &nonogram.row_restrictions, &nonogram.row_black_runs, false);

		if (nonogram.solved_indicator == row_dim*col_dim) {
			break;
		}

		nonogram.rule_2(1);

		if (nonogram.solved_indicator == row_dim*col_dim) {
			break;
		}

		nonogram.rule_3(1);

		if (nonogram.solved_indicator == row_dim*col_dim) {
			break;
		}

		nonogram.rule_4(1);

		if (nonogram.solved_indicator == row_dim*col_dim) {
			break;
		}


		nonogram.expand_and_limit(nonogram.n_rows, nonogram.n_cols, &nonogram.row_restrictions, &nonogram.row_black_runs, false);

		if (nonogram.solved_indicator == row_dim*col_dim) {
			break;
		}

		nonogram.rule_6(1);

		if (nonogram.solved_indicator == row_dim*col_dim) {
			break;
		}


		nonogram.rule_7(1);

		if (nonogram.solved_indicator == row_dim*col_dim) {
			break;
		}


		nonogram.rule_8(1);

		if (nonogram.solved_indicator == row_dim*col_dim) {
			break;
		}



		nonogram.fill_in_void(nonogram.n_rows, nonogram.n_cols, &nonogram.row_restrictions, &nonogram.row_black_runs, false);

		if (nonogram.solved_indicator == row_dim*col_dim) {
			break;
		}

		nonogram.mod_range_to_fit(nonogram.n_rows, nonogram.n_cols, &nonogram.row_restrictions, &nonogram.row_black_runs, false);

		if (nonogram.solved_indicator == row_dim*col_dim) {
			break;
		}


		nonogram.rule_11(1);

		if (nonogram.solved_indicator == row_dim*col_dim) {
			break;
		}

		// RUN ALL COL METHODS
		nonogram.check_overlap(nonogram.n_cols, nonogram.n_rows, &nonogram.col_restrictions, &nonogram.col_black_runs, true);

		if (nonogram.solved_indicator == row_dim*col_dim) {
			break;
		}

		nonogram.rule_2(2);

		if (nonogram.solved_indicator == row_dim*col_dim) {
			break;
		}

		nonogram.rule_3(2);

		if (nonogram.solved_indicator == row_dim*col_dim) {
			break;
		}

		nonogram.rule_4(2);

		if (nonogram.solved_indicator == row_dim*col_dim) {
			break;
		}

		nonogram.expand_and_limit(nonogram.n_cols, nonogram.n_rows, &nonogram.col_restrictions, &nonogram.col_black_runs, true);

		if (nonogram.solved_indicator == row_dim*col_dim) {
			break;
		}

		nonogram.rule_6(2);

		if (nonogram.solved_indicator == row_dim*col_dim) {
			break;
		}

		nonogram.rule_7(2);

		if (nonogram.solved_indicator == row_dim*col_dim) {
			break;
		}

		nonogram.rule_8(2);

		if (nonogram.solved_indicator == row_dim*col_dim) {
			break;
		}

		nonogram.fill_in_void(nonogram.n_cols, nonogram.n_rows, &nonogram.col_restrictions, &nonogram.col_black_runs, true);

		if (nonogram.solved_indicator == row_dim*col_dim) {
			break;
		}

		nonogram.mod_range_to_fit(nonogram.n_cols, nonogram.n_rows, &nonogram.col_restrictions, &nonogram.col_black_runs, true);

		if (nonogram.solved_indicator == row_dim*col_dim) {
			break;
		}

		nonogram.rule_11(2);
	}

	cout << endl;
	print_stuff(&nonogram, false, true);

	return 0;
}