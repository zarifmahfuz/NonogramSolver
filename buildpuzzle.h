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

	// if solved_indicator > n_rows * n_cols, we have solved the puzzle
	uint16_t solved_indicator;

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

		// initialize the solve counter
		solved_indicator = 0;
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
						solved_indicator++;
					}
					else if (curr_col >= 0 && curr_col<each_row_black_runs[0].first) {
						cells[i] = 1;
						solved_indicator++;
					}
					else if (each_row_black_runs[k-1].second < curr_col && curr_col < n_cols) {
						cells[i] = 1;
						solved_indicator++;
					}
					for (uint16_t j=0; j<k-1; j++) {
						if (each_row_black_runs[j].second < curr_col && curr_col < each_row_black_runs[j+1].first) {
							cells[i] = 1;
							solved_indicator++;
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
							solved_indicator++;
						}
						else if (counter >= 0 && counter < each_col_black_runs[0].first) {
							cells[i] = 1;
							solved_indicator++;
						}
						else if (each_col_black_runs[k-1].second < counter && counter < n_rows) {
							cells[i] = 1;
							solved_indicator++;
						}
						for (uint16_t m=0; m<k-1; m++) {
							if (each_col_black_runs[m].second < counter && counter < each_col_black_runs[m+1].first) {
								cells[i] = 1;
								solved_indicator++;
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
								solved_indicator++;
							}
						}
						else {
							if (cells[look_at_cell]==0 && cells[look_at_cell-1] != 0 && cells[look_at_cell+1] != 0) {
								// make the previous cell empty
								cells[look_at_cell-1] = 1;
								solved_indicator++;
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
								solved_indicator++;
							}
							
						}
						else {
							// checking for black run of length 1
							if (cells[look_at_cell] == 0 && cells[look_at_cell+1] != 0) {
								// make the next cell empty
								cells[look_at_cell+1] = 1;
								solved_indicator++;
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
								solved_indicator++;
							}
						}
						else {
							if (cells[look_at_cell] == 0 && cells[prev_row_same_col] != 0 
								&& cells[next_row_same_col] != 0) {
								// make the previous cell empty
								cells[prev_row_same_col] = 1;
								solved_indicator++;

								// cout << "cell that is being emptied: ";
								// cout << prev_row_same_col << endl;
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
								solved_indicator++;
							}
						}
						else {
							if (cells[look_at_cell] == 0 && cells[next_row_same_col] != 0) {
								// make the next cell empty 
								cells[next_row_same_col] = 1;
								solved_indicator++;
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
			// cout << "Row " << curr_row << ", Max length: " << max_length << endl;
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
						// cout << "Row " << curr_row << ", Max length: " << max_length << endl;
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
								solved_indicator++;
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

				//cout << "Col " << j << ", Max length: " << max_length << endl;

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
									solved_indicator++;
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
	// rule 1.5
	void expand_and_limit(uint16_t total, uint16_t perpTotal, vector<vector<uint16_t>> *restrictions,
		vector<vector<range>> *black_runs, bool isCol);

	// SECOND PHASE OF RULES
	// Objective: Refine the ranges of the black runs

	// implementing rule 2.1
	// PARAMETER: apply to row: 1; apply to col: 2
	void rule_6(int8_t row_or_col) {
		if (row_or_col == 1) {
			// iterating over every row
			for (uint16_t i=0; i<n_rows; i++) {
				vector<range> eachrow_runranges = row_black_runs[i];
				uint16_t k = eachrow_runranges.size();

				// iterating over every black run
				for (uint16_t j=0; j<k; j++) {
					uint16_t start = eachrow_runranges[j].first;
					uint16_t end = eachrow_runranges[j].second;

					if (j==0) {
						// starting black run

						//uint16_t end_next = eachrow_runranges[j+1].second;
						if (end >= eachrow_runranges[j+1].second) {
							end = eachrow_runranges[j+1].second - row_restrictions[i][j+1] - 1;
						}
					}
					else if (j==k-1) {
						// ending black run

						//uint16_t start_prev = eachrow_runranges[j-1].first;
						if (start <= eachrow_runranges[j-1].first) {
							start = eachrow_runranges[j-1].first + row_restrictions[i][j-1] + 1;
						}
					}
					else {
						// a black run somewhere in the middle

						if (start <= eachrow_runranges[j-1].first) {
							start = eachrow_runranges[j-1].first + row_restrictions[i][j-1] + 1;
						}
						if (end >= eachrow_runranges[j+1].second) {
							end = eachrow_runranges[j+1].second - row_restrictions[i][j+1] - 1;
						}
					}
					// refine the run range of this particular black run
					eachrow_runranges[j] = range(start, end);
				}
				// refine the run ranges for all the runs in this particular row
				row_black_runs[i] = eachrow_runranges;
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

					if (j==0) {
						// starting black run

						//uint16_t end_next = eachcol_runranges[j+1].second;
						if (end >= eachcol_runranges[j+1].second) {
							end = eachcol_runranges[j+1].second - col_restrictions[i][j+1] - 1;
						}
					}
					else if (j==k-1) {
						// ending black run

						//uint16_t start_prev = eachcol_runranges[j-1].first;
						if (start <= eachcol_runranges[j-1].first) {
							start = eachcol_runranges[j-1].first + col_restrictions[i][j-1] + 1;
						}
					}
					else {
						// a black run somewhere in the middle

						if (start <= eachcol_runranges[j-1].first) {
							start = eachcol_runranges[j-1].first + col_restrictions[i][j-1] + 1;
						}
						if (end >= eachcol_runranges[j+1].second) {
							end = eachcol_runranges[j+1].second - col_restrictions[i][j+1] - 1;
						}
					}
					// refine the run range of this particular black run
					eachcol_runranges[j] = range(start, end);
				}
				// refine the run ranges for all the runs in this particular column
				col_black_runs[i] = eachcol_runranges;
			}
		}
	}
	// implementing rule 2.2
	// PARAMETER: apply to row: 1; apply to col: 2
	void rule_7(int8_t row_or_col) {
		if (row_or_col == 1) {
			// iterating over every row
			for (uint16_t i=0; i<n_rows; i++) {
				vector<range> eachrow_runranges = row_black_runs[i];
				uint16_t k = eachrow_runranges.size();

				// iterating over every black run
				for (uint16_t j=0; j<k; j++) {
					uint16_t start = eachrow_runranges[j].first;
					uint16_t end = eachrow_runranges[j].second;

					if (j==0) {
						// only need to look at the (end+1)'th cell
						// need to check if (end+1)'th cell is in the same row
						uint16_t after_end = i*n_cols + end + 1;

						if (after_end/n_cols == i) {
							if (cells[after_end] == 0) {
								end -= 1;
							}
						}
					}
					else if (j==k-1) {
						// need to look at the (start-1)'th cell
						uint16_t before_start = i*n_cols + start - 1;

						// if prev cell is in the same row
						if (before_start/n_cols == i) {
							if (cells[before_start] == 0) {
								start += 1;
							}
						}
					}
					else {
						uint16_t before_start = i*n_cols + start - 1;

						// if prev cell is in the same row
						if (before_start/n_cols == i) {
							if (cells[before_start] == 0) {
								start += 1;
							}
						}
						uint16_t after_end = i*n_cols + end + 1;
						
						if (after_end/n_cols == i) {
							if (cells[after_end] == 0) {
								end -= 1;
							}
						}
					}
					// refine run range for this particular black run
					eachrow_runranges[j] = range(start, end);
				}
				// refine all run ranges for this particular row
				row_black_runs[i] = eachrow_runranges;
			}
		}
		else if (row_or_col == 2) {
			// iterating over every column
			for (uint16_t i=0; i<n_cols; i++) {
				vector<range> eachcol_runranges = col_black_runs[i];
				uint16_t k = eachcol_runranges.size();

				// iterating over every black run in the column 
				for (uint16_t j=0; j<k; j++) {
					uint16_t start = eachcol_runranges[j].first;
					uint16_t end = eachcol_runranges[j].second;

					if (j==0) {
						// first black run
						uint16_t after_end = (end+1)*n_cols + i;

						// if it's in the same column;
						if (after_end%n_cols == i) {
							if (cells[after_end] == 0) {
								end -= 1;
							}
						}
					}
					else if (j==k-1) {
						// last black run
						uint16_t before_start = (start-1)*n_cols + i;

						if (before_start%n_cols == i) {
							if (cells[before_start] == 0) {
								start += 1;
							}
						}
					}
					else {
						uint16_t before_start = (start-1)*n_cols + i;

						if (before_start%n_cols == i) {
							if (cells[before_start] == 0) {
								start += 1;
							}
						}
						uint16_t after_end = (end+1)*n_cols + i;

						if (after_end%n_cols == i) {
							if (cells[after_end] == 0) {
								end -= 1;
							}
						}
					}
					// refine run range for this particular black run
					eachcol_runranges[j] = range(start, end);
				}
				// refine all run ranges for this particular column
				col_black_runs[i] = eachcol_runranges;
			}
		}
	}

	// implementing rule 2.3
	// PARAMETER: apply to row: 1; apply to col: 2
	void rule_8(int8_t row_or_col) {
		if (row_or_col == 1) {
			// iterating over every row
			for (uint16_t i=0; i<n_rows; i++) {
				vector<range> eachrow_runranges = row_black_runs[i];
				uint16_t k = eachrow_runranges.size();

				// iterating over every black run in the row
				for (uint16_t j=0; j<k; j++) {
					// lower and upper limits of the range for each black run
					uint16_t start = eachrow_runranges[j].first;
					uint16_t end = eachrow_runranges[j].second;

					// we want to find out all the black segments covered by the range 
					// of this particular black run and find the starting and ending points
					// of each black segment; so we have to start by looking at the first
					// cell covered by the range of this black run
					vector<range> temp;
					uint16_t iter = start;
					uint16_t look_at_cell = i*n_cols + start;

					while (iter <= end) {
						if (cells[look_at_cell] == 0) {
							iter = find_and_insert(look_at_cell, 1, iter, &temp);
							look_at_cell = i*n_cols + iter;
						}
						else {
							iter++;
							look_at_cell++;
						}
					}

					// iterating over all the black segments covered by the range of black run j
					// how do i determine if a black segment belongs to an earlier or later run ???
					// first of all, the length of the black segment must be greater the length
					// of the black run j

					if (j==0) {
						// if it's the first black run in the row, only the upper limit of its 
						// range can get affected
						if (temp.size() > 0) {
							if (temp[0].second - temp[0].first + 1 > row_restrictions[i][j]) {
								end = temp[0].first - 2;
							}
						}
					}
					else if (j==k-1) {
						// if it's the last black run in the row, only the lower limit of its
						// range can get affected
						if (temp.size() > 0) {
							uint16_t ind = temp.size();
							if (temp[ind-1].second - temp[ind-1].first + 1 > row_restrictions[i][j]) {
								start = temp[ind-1].second + 2;
							}
						}
					}
					else {
						for (uint16_t m=0; m<temp.size(); m++) {
							// former black runs 
							if (temp[m].second < end) {
								// js = ie + 2
								start = temp[m].second + 2;
							}
							// later black runs
							else if (temp[m].second > end) {
								// je = is - 2
								end = temp[m].first - 2;
							}
						}
					}
					// refine the run range for this particular black run
					eachrow_runranges[j] = range(start, end);
				}
				// refine run ranges for the entire row
				row_black_runs[i] = eachrow_runranges;
			}
		}
		else if (row_or_col == 2) {
			// iterating over every column
			for (uint16_t i=0; i<n_cols; i++) {
				vector<range> eachcol_runranges = col_black_runs[i];
				uint16_t k = eachcol_runranges.size();

				// iterating over every black run in the column
				for (uint16_t j=0; j<k; j++) {
					uint16_t start = eachcol_runranges[j].first;
					uint16_t end = eachcol_runranges[j].second;

					vector<range> temp;
					uint16_t iter = start;
					uint16_t look_at_cell = start*n_cols + j;

					while (iter <= end) {
						if (cells[look_at_cell] == 0) {
							iter = find_and_insert(look_at_cell, 2, iter, &temp);
							look_at_cell = iter*n_cols + j;
						}
						else {
							iter++;
							look_at_cell += n_cols;
						}
					}

					if (j==0) {
						// if its the first black run, only je can be changed
						if (temp.size() > 0) {
							if (temp[0].second - temp[0].first + 1 > col_restrictions[i][j]) {
								// je = is - 2
								end = temp[0].first - 2;
							}
						}
					}
					else if (j==k-1) {
						// if its the last black run, only js can be changed
						if (temp.size() > 0) {
							uint16_t ind = temp.size();
							if (temp[ind-1].second - temp[ind-1].first + 1 > col_restrictions[i][j]) {
								// js = ie + 2
								start = temp[ind-1].second + 2;
							}
						}
					}
					else {
						for (uint16_t m=0; m<temp.size(); m++) {
							// former black runs 
							if (temp[m].second < end) {
								// js = ie + 2
								start = temp[m].second + 2;
							}
							// later black runs
							else if (temp[m].second > end) {
								// je = is - 2
								end = temp[m].first - 2;
							}
						}
					}
					// refine the run range for this particular black run
					eachcol_runranges[j] = range(start, end);
				}
				// refine run ranges for the entire column
				col_black_runs[i] = eachcol_runranges;
			}
		}
	}

	// Rule 3.1
	void fill_in_void(uint16_t total, uint16_t perpTotal, vector<vector<uint16_t>> *restrictions,
	vector<vector<range>> *black_runs, bool isCol);

	// Rule 3.2
	void mod_range_to_fit(uint16_t total, uint16_t perpTotal, vector<vector<uint16_t>> *restrictions,
	vector<vector<range>> *black_runs, bool isCol);

	// implementing rule 3.3
	// PARAMETER: apply to row: 1; apply to col: 2
	void rule_11(int8_t row_or_col) {
		if (row_or_col == 1) {
			// iterating over each row
			for (uint16_t i=0; i<n_rows; i++) {
				vector<range> eachrow_runranges = row_black_runs[i];
				uint16_t k = eachrow_runranges.size();

				// iterating over each black run
				for (uint16_t j=0; j<k; j++) {
					// lower and upper limits of the range for each black run
					uint16_t start = eachrow_runranges[j].first;
					uint16_t end = eachrow_runranges[j].second;

					if (j > 0) {
						// if (j-1)e < js; j's range does not overlap with j-1's range
						if (eachrow_runranges[j-1].second < start) {
							
							// CASE 1: Crjs is coloured
							
							if (cells[i*n_cols+start] == 0) {
								// length of this black run
								//uint16_t length_of_j = row_restrictions[i][j];
								for (uint16_t m = start+1; m < row_restrictions[i][j]; m++) {
									// COLOUR THE CELL
									cells[i*n_cols+m] = 0;
									solved_indicator++;
								}
								// refine the run range of this black run
								// je = js + LBj - 1
								end = start + row_restrictions[i][j] - 1;

								// leave cells js-1 and je+1 empty
								cells[i*n_cols+start-1] = 0;
								solved_indicator += 1;

								// if the (end+1)'th cell is in the same row
								if ((i*n_cols + end + 1)/n_cols == i) {
									cells[i*n_cols+end+1] = 0;
									solved_indicator += 1;
								}
								

								if (j < k-1) {
									if (eachrow_runranges[j+1].first <= end) {
										// run range of j+1 overlaps with j, need to update 
										// the run range of j+1
										// (j+1)s = je + 2
										eachrow_runranges[j+1].first = end + 2;
									}
								}

								if (eachrow_runranges[j-1].second == start-1) {
									eachrow_runranges[j-1].second = start - 2;
								}
							}

							// CASE 2
							for (uint16_t n=0; n < end-1; n++) {
								if (cells[i*n_cols+n]!=1 && cells[i*n_cols+n+1]==1) {
									// if an empty cell appears after a black cell in the run range of j
									// je = n+1 - 1
									end = n;
									break;
								}
							}

							// CASE 3
							uint16_t iter = start;
							vector<range> temp;
							while (iter <= end) {
								if (cells[i*n_cols+iter] == 0) {
									// start of a black segment
									uint16_t start_seg = iter;
									while (iter <= end && cells[i*n_cols+iter] == 0) {
										iter++;
									}
									uint16_t end_seg = iter - 1;
									// if (iter > end) {
									// 	end_2 = end;
									// }
									// else {
									// 	end_2 = iter - 1;
									// }

									// insert the start and end positions of this black segments
									// in the set of all black segments in the run range of j
									temp.push_back(range(start_seg, end_seg));
								}
								else {
									iter++;
								}
							}
							if (temp.size() >= 2) {
								//uint16_t length_of_j = row_restrictions[i][j];
								uint16_t start_0 = temp[0].first;
								uint16_t iter_2 = 1;

								while (iter_2 < temp.size()) {
									if (temp[iter_2].second - start_0 + 1 > row_restrictions[i][j]) {
										end = temp[iter_2].second - 2;
										break;
									}
									else {
										iter_2++;
									}
								}
							}

						}
					}
				}
			}
		}
	}



private:
	void push_to_colour(int16_t low, int16_t high, uint16_t line, bool isCol);
	int16_t find_white_or_wall(int16_t bottomLim, int16_t topLim, int8_t incr_or_decr, uint16_t perpTotal,
		uint16_t line, bool isCol);

	/* PARAMETERS: 
		cell_number: actual cell index of the current cell in the cells vector
		row_or_col: 1 applies to row, 2 to column
		current: current row/column number
		set_of_segments: vector in which all black segments (with starting and ending points) are inserted
	*/
	uint16_t find_and_insert(uint16_t cell_number, int8_t row_or_col, uint16_t current, vector<range> *set_of_segments) {
		if (row_or_col == 1) {
			//uint16_t counter_left = 0;
			//uint16_t counter_right = 0;
			uint16_t go_left = cell_number - 1;
			uint16_t go_right = cell_number + 1;

			// spreadout in both directions from the current cell position, while staying on the same row
			// and continue in each direction until a cell is black in each direction
			while ((go_left%n_cols < current && go_left >= 0 && cells[go_left] == 0) 
				|| (go_right%n_cols > current && go_right < n_rows*n_cols && cells[go_right] == 0)) {

				if (go_left%n_cols < current && go_left >= 0 && cells[go_left] == 0) {
					go_left -= 1;
					//counter_left += 1;
				}
				if (go_right%n_cols > current && go_right < n_rows*n_cols && cells[go_right] == 0) {
					go_right += 1;
					//counter_right += 1;
				}
			}
			// computing the column number and then inserting into the array
			(*set_of_segments).push_back(range((go_left+1)%n_cols, (go_right-1)%n_cols));
			// returning a cell which is either empty or unknown and we will carry on finding 
			// segments of black run from that position
			return go_right % n_cols;
		}
		else if (row_or_col == 2) {
			uint16_t go_up = cell_number - n_cols;
			uint16_t go_down = cell_number + n_cols;

			while ((go_up>=0 && cells[go_up]==0) || (go_down<n_rows*n_cols && cells[go_down]==0)) {
				if (go_up>=0 && cells[go_up]==0) {
					go_up -= n_cols;
				}
				if (go_down<n_rows*n_cols && cells[go_down]==0) {
					go_down += n_cols;
				}
			}
			// computing the row number and then inserting into the array
			(*set_of_segments).push_back(range((go_up+n_cols)/n_cols, (go_down-n_cols)/n_cols));
			return (go_down)/n_cols;
		}
	}
};



#endif