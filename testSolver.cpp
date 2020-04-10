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

	//cout << "solved_indicator: " << nonogram.solved_indicator << endl;

	// rule 1.4 does not apply to the state of the puzzle at the end of 
	// rule 1.3, so the output of this for loop should be the same as the prev one
	// for (uint16_t i=0; i<row_dim*col_dim; i++) {
	// 	if (nonogram.cells[i] == 1) {
	// 		cout << "Cell " << i << ", State: " << nonogram.cells[i] << endl;
	// 	}
	// }

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
	// on rows
	nonogram.fill_in_void(nonogram.n_rows, nonogram.n_cols, &nonogram.row_restrictions, &nonogram.row_black_runs, false);
	// on cols
	cout << "cols" << endl;
	nonogram.fill_in_void(nonogram.n_cols, nonogram.n_rows, &nonogram.col_restrictions, &nonogram.col_black_runs, true);

	// appplying 3.2
	// on rows
	nonogram.mod_range_to_fit(nonogram.n_rows, nonogram.n_cols, &nonogram.row_restrictions, &nonogram.row_black_runs, false);
	// on cols
	nonogram.mod_range_to_fit(nonogram.n_cols, nonogram.n_rows, &nonogram.col_restrictions, &nonogram.col_black_runs, true);

	print_stuff(&nonogram, false, true);
	cout << endl;


	while (nonogram.solved_indicator < row_dim*col_dim ) {

		// just rows
		nonogram.check_overlap(nonogram.n_rows, nonogram.n_cols, &nonogram.row_restrictions, &nonogram.row_black_runs, false);
		// cout << "rule 1.1 success" <<endl;
		// cout << nonogram.col_black_runs[3][0].second << endl;

		nonogram.rule_2(1);
		// cout << "rule 1.2 success" <<endl;
		// cout << nonogram.col_black_runs[3][0].second << endl;

		nonogram.rule_3(1);
		// cout << "rule 1.3 success" <<endl;
		// cout << nonogram.col_black_runs[3][0].second << endl;

		nonogram.rule_4(1);
		// cout << "rule 1.4 success" <<endl;
		// cout << nonogram.col_black_runs[3][0].second << endl;

		nonogram.expand_and_limit(nonogram.n_rows, nonogram.n_cols, &nonogram.row_restrictions, &nonogram.row_black_runs, false);
		// cout << "rule 1.5 success" <<endl;
		// cout << nonogram.col_black_runs[3][0].second << endl;

		nonogram.rule_6(1);
		// cout << "rule 2.1 success" <<endl;
		// cout << nonogram.col_black_runs[3][0].second << endl;


		nonogram.rule_7(1);
		// cout << "rule 2.2 success" <<endl;
		// cout << nonogram.col_black_runs[3][0].second << endl;

		nonogram.rule_8(1);
		// cout << "rule 2.3 success" <<endl;
		// cout << nonogram.col_black_runs[3][0].second << endl;

		nonogram.fill_in_void(nonogram.n_rows, nonogram.n_cols, &nonogram.row_restrictions, &nonogram.row_black_runs, false);
		// cout << "rule 3.1 success" <<endl;
		// cout << nonogram.col_black_runs[3][0].second << endl;

		nonogram.mod_range_to_fit(nonogram.n_rows, nonogram.n_cols, &nonogram.row_restrictions, &nonogram.row_black_runs, false);
		// cout << "rule 3.2 success" <<endl;
		// cout << nonogram.col_black_runs[3][0].second << endl;

		nonogram.rule_11(1);
		// cout << "rule 3.3 success" <<endl;
		// cout << nonogram.col_black_runs[3][0].second << endl;

		cout << "----------------------------------------" << endl;
		cout << "COLS" << endl;

		// just columns
		nonogram.check_overlap(nonogram.n_cols, nonogram.n_rows, &nonogram.col_restrictions, &nonogram.col_black_runs, true);
		// cout << "rule 1.1 success" <<endl;
		// cout << nonogram.col_black_runs[3][0].second << endl;

		nonogram.rule_2(2);
		// cout << "rule 1.2 success" <<endl;
		// cout << nonogram.col_black_runs[3][0].second << endl;

		nonogram.rule_3(2);
		// cout << "rule 1.3 success" <<endl;
		// cout << nonogram.col_black_runs[3][0].second << endl;

		nonogram.rule_4(2);
		// cout << "rule 1.4 success" <<endl;
		// cout << nonogram.col_black_runs[3][0].second << endl;

		nonogram.expand_and_limit(nonogram.n_cols, nonogram.n_rows, &nonogram.col_restrictions, &nonogram.col_black_runs, true);
		// cout << "rule 1.5 success" <<endl;
		// cout << nonogram.col_black_runs[3][0].second << endl;

		nonogram.rule_6(2);
		// cout << "rule 2.1 success" <<endl;
		// cout << nonogram.col_black_runs[3][0].second << endl;

		nonogram.rule_7(2);
		// cout << "rule 2.2 success" <<endl;
		// cout << nonogram.col_black_runs[3][0].second << endl;

		nonogram.rule_8(2);
		// cout << "rule 2.3 success" <<endl;
		// cout << nonogram.col_black_runs[3][0].second << endl;

		nonogram.fill_in_void(nonogram.n_cols, nonogram.n_rows, &nonogram.col_restrictions, &nonogram.col_black_runs, true);
		cout << "rule 3.1 success" <<endl;
		//cout << nonogram.col_black_runs[3][0].second << endl;

		nonogram.mod_range_to_fit(nonogram.n_cols, nonogram.n_rows, &nonogram.col_restrictions, &nonogram.col_black_runs, true);
		// cout << "rule 3.2 success" <<endl;
		//cout << nonogram.col_black_runs[3][0].second << endl;

		nonogram.rule_11(2);
		// cout << "rule 3.3 success" <<endl;
		// cout << nonogram.col_black_runs[3][0].second << endl;

		print_stuff(&nonogram, false, true);
		cout  << endl << endl;
	}

	print_stuff(&nonogram, false, true);

	return 0;
}