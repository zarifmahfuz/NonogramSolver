#ifndef __BUILDPUZZLE_H__
#define __BUILDPUZZLE_H__

#include <vector>

using namespace std;

// lower and upper limit for the existence of each black run
typedef pair<uint16_t, uint16_t> range;

// -1: UNKNOWN, 0: BLACK, 1: WHITE

class Puzzle {
public:
	// Data members, assuming rows/columns wont be greater than 127
	uint16_t n_rows;
	uint16_t n_cols;
	vector<int> cells;
	vector<vector<uint16_t>> row_restrictions; 
	vector<vector<uint16_t>> col_restrictions; 
	vector<vector<range>> row_black_runs;
	vector<vector<range>> col_black_runs;

	// Constructor
	Puzzle(uint16_t rows, uint16_t cols, vector<vector<uint16_t>> row_restrictions,
		vector<vector<uint16_t>> col_restrictions) {

		for (int i=0; i<rows*cols; i++) {
			// initialize every cell to unknown 
			cells.push_back(-1);
		}
		this->row_restrictions = row_restrictions;
		this->col_restrictions = col_restrictions;

		n_rows = rows;
		n_cols = cols;
	}

	// this method initializes run ranges for each black run in each row/column
	// according to the formula provided in page 5 of the document
	void initialize_run_ranges() {
		// iterate over each row
		for (int i=0; i<n_rows; i++) {
			vector<uint16_t> each_row_restrictions = row_restrictions[i];
			// this vector contains the ranges of all the black runs
			vector<range> black_runs;

			// k represents the number of black runs in each row
			uint16_t k = each_row_restrictions.size();

			for (int j=0; j<k; j++) {
				uint16_t lower_lim, upper_lim;

				if (j==0) {
					lower_lim = 0;
					uint16_t cumsum = 0;

					for (int j_2 = j+1; j_2<k; j_2++) {
						cumsum = cumsum + each_row_restrictions[j_2] + 1;
					}
					upper_lim = n_cols - 1 - cumsum; 
				}

				else if (j==k-1) {
					upper_lim = n_cols - 1;

					lower_lim = 0;
					for (int j_2 = 0; j_2<j; j_2++) {
						lower_lim = lower_lim + each_row_restrictions[j_2] + 1;
					}
				}
				else {
					lower_lim = 0;
					for (int j_2 = 0; j_2<j; j_2++) {
						lower_lim = lower_lim + each_row_restrictions[j_2] + 1;
					}

					uint16_t cumsum = 0;
					for (int j_2 = j+1; j_2<k; j_2++) {
						cumsum = cumsum + each_row_restrictions[j_2] + 1;
					}
					upper_lim = n_cols - 1 - cumsum;
				}

				// cout << "Row " << i << ", " << "Restriction " << each_row_restrictions[j]
				// << ", " << "Lower limit: " << lower_lim << ", Upper limit: " << upper_lim << endl;

				black_runs.push_back(range(lower_lim, upper_lim));
			}
			row_black_runs.push_back(black_runs);
		}

		// iterate over each column
		for (int i=0; i<n_cols; i++) {
			vector<uint16_t> each_col_restrictions = col_restrictions[i];
			// this vector contains the ranges of all the black runs
			vector<range> black_runs;

			// k represents the number of black runs in each column
			uint16_t k = each_col_restrictions.size();

			for (int j=0; j<k; j++) {
				uint16_t lower_lim, upper_lim;

				if (j==0) {
					lower_lim = 0;
					uint16_t cumsum = 0;

					for (int j_2 = j+1; j_2<k; j_2++) {
						cumsum = cumsum + each_col_restrictions[j_2] + 1;
					}
					upper_lim = n_rows - 1 - cumsum; 
				}
				else if (j==k-1) {
					upper_lim = n_rows - 1;

					lower_lim = 0;
					for (int j_2 = 0; j_2<j; j_2++) {
						lower_lim = lower_lim + each_col_restrictions[j_2] + 1;
					}

				}
				else {
					lower_lim = 0;
					for (int j_2 = 0; j_2<j; j_2++) {
						lower_lim = lower_lim + each_col_restrictions[j_2] + 1;
					}

					uint16_t cumsum = 0;
					for (int j_2 = j+1; j_2<k; j_2++) {
						cumsum = cumsum + each_col_restrictions[j_2] + 1;
					}
					upper_lim = n_rows - 1 - cumsum;
				}

				cout << "Col " << i << ", " << "Restriction " << each_col_restrictions[j]
				<< ", " << "Lower limit: " << lower_lim << ", Upper limit: " << upper_lim << endl;

				black_runs.push_back(range(lower_lim, upper_lim));
			}
			col_black_runs.push_back(black_runs);
		}
	}

	void check_overlap(uint16_t total, uint16_t perpTotal, vector<vector<uint16_t>> *restrictions,
	vector<vector<range>> *black_runs, bool isCol);

	// implementing rule 1.2
	// not quite sure if we can apply a rule to both a row and a col at the same time
	// PARAMETER: apply to row: 1; apply to col: 2
	void rule_2(int8_t row_or_col) {
		if (row_or_col == 1) {
			for (uint16_t i=0; i<n_rows*n_cols; i++) {
				// IF A CELL IS UNKNOWN
				if (cells[i] == -1) {
					uint16_t curr_row = i/n_cols;
					uint16_t curr_col = i%n_cols;

					vector<range> each_row_black_runs = row_black_runs[curr_row];
					uint16_t k = each_row_black_runs.size();
					
					if (k == 0) {
						// if an entire row has no black runs, every cell in that row should be empty
						cells[i] = 1;
					}
					else if (curr_col >= 0 && curr_col<each_row_black_runs[0].first) {
						cells[i] = 1;
					}
					else if (each_row_black_runs[k-1].second < curr_col && curr_col < n_cols) {
						cells[i] = 1;
					}
					for (uint16_t j=0; j<k-1; j++) {
						if (each_row_black_runs[j].second < curr_col && curr_col < each_row_black_runs[j+1].first) {
							cells[i] = 1;
						}
					}
				}
			}
		}
		else if (row_or_col == 2) {
			
			// looping over each column
			// j represents each column
			for (uint16_t j=0; j<n_cols; j++) {
				// i represents an iterator over every row for each column
				// i represents the actual index for the "cells" data structure
				uint16_t i = j;

				// counter represents the current row number
				uint16_t counter = 0;

				while (counter < n_rows) {
					// IF A CELL IS UNKNOWN
					if (cells[i] == -1) {
						vector<range> each_col_black_runs = col_black_runs[j];
						uint16_t k = each_col_black_runs.size();

						if (k == 0) {
							// if an entire col has no black runs, every cell in that col should be empty
							cells[i] = 1;
						}
						else if (counter >= 0 && counter < each_col_black_runs[0].first) {
							cells[i] = 1;
						}
						else if (each_col_black_runs[k-1].second < counter && counter < n_rows) {
							cells[i] = 1;
						}
						for (uint16_t m=0; m<k-1; m++) {
							if (each_col_black_runs[m].second < counter && counter < each_col_black_runs[m+1].first) {
								cells[i] = 1;
							}
						}
					}
					i = i + n_cols;
					counter++;
				}
			}
		}
	}

	// implementing rule 1.3
	// PARAMETER: apply to row: 1; apply to col: 2
	void rule_3(int8_t row_or_col) {

		if (row_or_col == 1) {
			// iterating over every row
			for (uint16_t i=0; i<n_rows; i++) {
				vector<range> eachrow_runranges = row_black_runs[i];
				uint16_t k = eachrow_runranges.size();

				// iterate over every black run
				for (uint16_t j=0; j<k; j++) {
					uint16_t start = eachrow_runranges[j].first;
					uint16_t end = eachrow_runranges[j].second;

					if (start != 0) {
						uint16_t look_at_cell = i*n_cols + start;

						// checking for a black run of length 1
						if (start == n_cols-1) {
							if (cells[look_at_cell] == 0 && cells[look_at_cell-1] != 0) {
								// make the previous cell empty
								cells[look_at_cell-1] = 1;
							}
						}
						else {
							if (cells[look_at_cell]==0 && cells[look_at_cell-1] != 0 && cells[look_at_cell+1] != 0) {
								// make the previous cell empty
								cells[look_at_cell-1] = 1;
							}
						}
					}
					if (end != n_cols-1) {
						uint16_t look_at_cell = i*n_cols + end;

						if (end != 0) {
							
							// checking for a black run of length 1 
							if (cells[look_at_cell] == 0 && cells[look_at_cell-1] !=0 && cells[look_at_cell+1] != 0) {
								// make the next cell empty
								cells[look_at_cell+1] = 1;
							}
							
						}
						else {
							// checking for black run of length 1
							if (cells[look_at_cell] == 0 && cells[look_at_cell+1] != 0) {
								// make the next cell empty
								cells[look_at_cell+1] = 1;
							}
						}
					}
				}
			}
		}
		else if (row_or_col == 2) {
			// iterating over every column
			for (uint16_t i=0; i<n_cols; i++) {
				vector<range> eachcol_runranges = col_black_runs[i];
				uint16_t k = eachcol_runranges.size();

				// iterating over every black run
				for (uint16_t j=0; j<k; j++) {
					uint16_t start = eachcol_runranges[j].first;
					uint16_t end = eachcol_runranges[j].second;

					if (start != 0) {
						// looks at j'th column and start'th row
						uint16_t look_at_cell = start*n_cols + i;
						uint16_t prev_row_same_col = (start-1)*n_cols + i;
						uint16_t next_row_same_col = (start+1)*n_cols + i;

						if (start == n_rows-1) {
							if (cells[look_at_cell] == 0 && cells[prev_row_same_col] != 0) {
								// make the previous cell empty
								cells[prev_row_same_col] = 1;
							}
						}
						else {
							if (cells[look_at_cell] == 0 && cells[prev_row_same_col] != 0 
								&& cells[next_row_same_col] != 0) {
								// make the previous cell empty
								cells[prev_row_same_col] = 1;

								cout << "cell that is being emptied: ";
								cout << prev_row_same_col << endl;
							}
						}
					}
					if (end != n_rows-1) {
						// looks at j'th column and end'th row
						uint16_t look_at_cell = end*n_cols + i;
						uint16_t prev_row_same_col = (end-1)*n_cols + i;
						uint16_t next_row_same_col = (end+1)*n_cols + i;

						if (end != 0) {
							if (cells[look_at_cell] == 0 && cells[prev_row_same_col] != 0 
								&& cells[next_row_same_col] != 0) {
								// make the next cell empty
								cells[next_row_same_col] = 1;
							}
						}
						else {
							if (cells[look_at_cell] == 0 && cells[next_row_same_col] != 0) {
								// make the next cell empty 
								cells[next_row_same_col] = 1;
							}
						}
					}
				}
			}
		}
	}

	// implementing rule 1.4 
	// Assumption: When we are looking at cell i, and cell Ci-1 and Ci+1 are coloured
	// Based on the above assumption, if we color cell i and the length of this black run
	// becomes greater the the maximum length of black run possible in the row/column,
	// the cell i should be left EMPTY
	// PARAMETER: apply to row: 1; apply to col: 2
	void rule_4(int8_t row_or_col) {

		if (row_or_col == 1) {
			
			uint16_t curr_row = 0;
			uint16_t curr_col = 0;
			uint16_t max_length = 0;
			vector<uint16_t> each_row_restrictions = row_restrictions[curr_row];

			// find the max length of a black run in a row
			for (int i=0; i<each_row_restrictions.size(); i++) {
				if (each_row_restrictions[i] > max_length) {
					max_length = each_row_restrictions[i];
				}
			}
			// iterating over every row in the puzzle
			for (uint16_t i=0; i<n_rows*n_cols; i++) {
				if (cells[i] == -1) {
					
					curr_col = i%n_cols;

					if (i/n_cols != curr_row) {
						// we have moved to a new row, update curr_row and find the new max length
						// of a black run in this row
						curr_row = i/n_cols;
						each_row_restrictions = row_restrictions[curr_row];
						max_length = 0;

						for(int i=0; i<each_row_restrictions.size(); i++) {
							if (each_row_restrictions[i] > max_length) {
								max_length = each_row_restrictions[i];
							}
						}
					}

					if (curr_col > 0 && curr_col < n_cols - 1) {
						if (cells[i-1]==0 && cells[i+1]==0) {
							uint16_t go_left = i-1;
							uint16_t go_right = i+1;
							uint16_t counter_left = 0;
							uint16_t counter_right = 0;

							// while we are still in the current row and either cells to the left or right are
							// black
							while ((go_left%n_cols < curr_col && go_left >= 0 && cells[go_left] == 0)
								|| (go_right%n_cols > curr_col && go_right < n_rows*n_cols && cells[go_right] == 0)) {

								if (go_left%n_cols < curr_col && go_left >= 0 && cells[go_left] == 0) {
									go_left -= 1;
									counter_left += 1;
								}
								if (go_right%n_cols > curr_col && go_right < n_rows*n_cols && cells[go_right] == 0) {
									go_right += 1;
									counter_right += 1;
								}
							}
							if (counter_left + counter_right + 1 > max_length) {
								// MAKE THE CURRENT CELL EMPTY!
								cells[i] = 1;
							}
						}
					}
				}
			}
		}
		else if (row_or_col == 2) {

			// iterate over each column
			// j represents each column
			for (uint16_t j=0; j<n_cols; j++) {

				uint16_t max_length = 0;
				vector<uint16_t> each_col_restrictions = col_restrictions[j];

				// find the max length of a black run in a column
				for (int i=0; i<each_col_restrictions.size(); i++) {
					if (each_col_restrictions[i] > max_length) {
						max_length = each_col_restrictions[i];
					}
				}

				// i represents an iterator over every row for each column
				// i represents the actual index for the "cells" data structure
				uint16_t i = j;

				// counter represents the current row number
				uint16_t counter = 0;

				while (counter<n_rows) {
					if (cells[i] == -1) {
						// this rule only applies if we are not in the first/last row
						if (counter > 0 && counter < n_rows-1) {
							if (cells[i-n_cols]==0 && cells[i+n_cols]==0) {

								uint16_t go_up = i - n_cols;
								uint16_t go_down = i + n_cols;
								uint16_t counter_up = 0;
								uint16_t counter_down = 0;

								while ((go_up>=0 && cells[go_up]==0) || (go_down<n_rows*n_cols && cells[go_down]==0)) {
									
									if (go_up>=0 && cells[go_up]==0) {
										go_up -= n_cols;
										counter_up += 1;
									}
									if (go_down<n_rows*n_cols && cells[go_down]==0) {
										go_down += n_cols;
										counter_down += 1;
									}
								}

								if (counter_up + counter_down + 1 > max_length) {
									// MAKE THE CURRENT CELL EMPTY!
									cells[i] = 1;
								}
							}
						}
					}
					// MOVE TO THE NEXT ROW
					counter++;
					i = i + n_cols;
				}

			}
		}
	}

};



#endif