#include <iostream>

#include <algorithm>
#include <math.h>

#include "buildpuzzle.h"


using namespace std;


Puzzle::Puzzle(uint16_t rows, uint16_t cols, vector<vector<uint16_t>> row_restrictions,
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


void Puzzle::initialize_run_ranges() {
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
				int16_t cumsum = 0;

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

			black_runs.push_back(range(lower_lim, upper_lim));
		}
		col_black_runs.push_back(black_runs);
	}
}

// Some Private Methods:

int16_t Puzzle::find_white_or_wall(int16_t bottomLim, int16_t topLim, int8_t incr_or_decr, uint16_t perpTotal, 
	uint16_t line, bool isCol) {

	// going up
	if (incr_or_decr == 1) {
		// try to find a white
		while (bottomLim <=topLim && bottomLim < perpTotal) {
			uint16_t index = n_cols*line+bottomLim;
			if (isCol) {
				index = n_cols*bottomLim+line;
			}
			// if you find a white, return the cell
			if (cells[index] == 1) {
				return bottomLim;
			}
			bottomLim++;
		}
		// still nothing? check if there's a wall
		if (topLim >=perpTotal) {
			return perpTotal;
		}
		// found nothing
		else{
			return -2;
		}
	}
	// going down
	else { 
		// try to find a white
		while (bottomLim <=topLim && topLim >= 0) {
			uint16_t index = n_cols*line+topLim;
			if (isCol) {
				index = n_cols*topLim+line;
			}

			// if you find a white, return the cell
			if (cells[index] == 1) {
				return topLim;
			}
			topLim--;

		}
		// still nothing? check if there's a wall
		if (bottomLim <= -1) {
			return -1;
		}
		// found nothing
		else{
			return -2;
		}
	}
}

void Puzzle::push_to_colour(int16_t low, int16_t high, uint16_t line, bool isCol) {
	for (int ind = low; ind <= high; ind++) {

		// if the cell hasn't been coloured yet
		uint16_t colind = n_cols*line+ind;
		// adjust index accordingly if you're iterating thru columns
		if (isCol) {
			colind = n_cols*ind + line;
		}
		// if the cell hasn't been coloured yet
		if (cells[colind]== -1) {
			// colour it
			cells[colind] = 0;
			solved_indicator++;
		}
	}
}

uint16_t Puzzle::find_and_insert(uint16_t cell_number, int8_t row_or_col, uint16_t current, vector<range> *set_of_segments,
uint16_t &locate) {
	if (row_or_col == 1) {
		uint16_t go_left = cell_number - 1;
		uint16_t go_right = cell_number + 1;

		// spreadout in both directions from the current cell position, while staying on the same row
		// and continue in each direction until a cell is black in each direction
		while ((go_left%n_cols < current && go_left >= 0 && cells[go_left] == 0) 
			|| (go_right%n_cols > current && go_right < n_rows*n_cols && cells[go_right] == 0)) {

			if (go_left%n_cols < current && go_left >= 0 && cells[go_left] == 0) {
				go_left -= 1;
			}
			if (go_right%n_cols > current && go_right < n_rows*n_cols && cells[go_right] == 0) {
				go_right += 1;
			}
		}
		// computing the column number and then inserting into the array
		(*set_of_segments).push_back(range((go_left+1)%n_cols, (go_right-1)%n_cols));
		// returning a cell which is either empty or unknown and we will carry on finding 
		// segments of black run from that position
		locate = go_right/n_cols;

		return go_right % n_cols;
	}
	else if (row_or_col == 2) {
		int16_t go_up = cell_number - n_cols;
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
		locate = go_down % n_cols;

		return (go_down)/n_cols;
	}
}

void Puzzle::check_overlap(uint16_t total, uint16_t perpTotal, vector<vector<uint16_t>> *restrictions,
	vector<vector<range>> *black_runs, bool isCol){

	// iterate thru each line
	for (uint16_t line = 0; line < total; line++) {

		uint16_t runs_per_line = (*restrictions)[line].size();
		// iterate thru all runs in the line
		for (uint16_t runs = 0; runs <runs_per_line; runs++) {

			// find leftmost and rightmost possibly coloured index
			// rje  + 1 - LBj <= i
			int16_t leftmost = ( (*black_runs)[line][runs].second + 1- (*restrictions)[line][runs]) + line*perpTotal;
			// i <= rjs - 1 + LBj
			int16_t rightmost = ( (*black_runs)[line][runs].first - 1+ (*restrictions)[line][runs]) + line*perpTotal;
			// colour the runs when the leftmost overlaps with rightmost

			for (int16_t ind = leftmost; ind <= rightmost; ind++) {

				uint16_t converted_ind = ind;
				// convert to the numbering system that we're actually using (horiz)
				if (isCol){
					converted_ind = ind -(n_rows-1)*line +(n_cols-1)*(ind - line*n_rows);
				}
				// if it's alr coloured, skip
				if (cells[converted_ind]!= -1) {
					continue;
				}

				// colour it
				cells[converted_ind] = 0;
				solved_indicator++;
			}
		}
	}
}

void Puzzle::rule_2(int8_t row_or_col) {
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
					if (cells[i] == -1) {
						cells[i] = 1;
						solved_indicator++;
					}
				}
				else if (curr_col >= 0 && curr_col<each_row_black_runs[0].first) {
					if (cells[i] == -1) {
						cells[i] = 1;
						solved_indicator++;
					}
				}
				else if (each_row_black_runs[k-1].second < curr_col && curr_col < n_cols) {
					if (cells[i] == -1) {
						cells[i] = 1;
						solved_indicator++;
					}
				}
				for (uint16_t j=0; j<k-1; j++) {
					if (each_row_black_runs[j].second < curr_col && curr_col < each_row_black_runs[j+1].first) {
						if (cells[i] == -1) {
							cells[i] = 1;
							solved_indicator++;
						}
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
						if (cells[i] == -1) {
							cells[i] = 1;
							solved_indicator++;
						}
					}
					else if (counter >= 0 && counter < each_col_black_runs[0].first) {
						if (cells[i] == -1) {
							cells[i] = 1;
							solved_indicator++;
						}
					}
					else if (each_col_black_runs[k-1].second < counter && counter < n_rows) {
						if (cells[i] == -1) {
							cells[i] = 1;
							solved_indicator++;
						}
					}
					for (uint16_t m=0; m<k-1; m++) {
						if (each_col_black_runs[m].second < counter && counter < each_col_black_runs[m+1].first) {
							if (cells[i] == -1) {
								cells[i] = 1;
								solved_indicator++;
							}
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
void Puzzle::rule_3(int8_t row_or_col) {

	if (row_or_col == 1) {
		// iterating over every row
		for (uint16_t i=0; i<n_rows; i++) {
			vector<range> eachrow_runranges = row_black_runs[i];
			uint16_t k = eachrow_runranges.size();

			// iterate over every black run
			for (uint16_t j=0; j<k; j++) {
				uint16_t start = eachrow_runranges[j].first;
				uint16_t end = eachrow_runranges[j].second;

				if (j > 0) {
					if (start != 0) {
						uint16_t look_at_cell = i*n_cols + start;

						// checking for a black run of length 1
						if (start == n_cols-1) {
							if (cells[look_at_cell] == 0 && cells[look_at_cell-1] != 0) {
								if (j > 0) {
									if (eachrow_runranges[j-1].second <= start && row_restrictions[i][j-1] == 1) {
										// make the previous cell empty
										if (cells[look_at_cell-1] == -1) {
											cells[look_at_cell-1] = 1;
											solved_indicator++;
										}
									}
								}	
							}
						}
						else {
							if (cells[look_at_cell]==0 && cells[look_at_cell-1] != 0 && cells[look_at_cell+1] != 0) {
								if (j > 0) {
									if (eachrow_runranges[j-1].second <= start && row_restrictions[i][j-1] == 1) {
										// make the previous cell empty
										if (cells[look_at_cell-1] == -1) {
											cells[look_at_cell-1] = 1;
											solved_indicator++;
										}
									}
								}
							}
						}
					}
				}

				if (j < k-1) {
					if (end != n_cols-1) {
						uint16_t look_at_cell = i*n_cols + end;

						if (end != 0) {
							
							// checking for a black run of length 1 
							if (cells[look_at_cell] == 0 && cells[look_at_cell-1] !=0 && cells[look_at_cell+1] != 0) {
								if (j < k-1) {
									if (eachrow_runranges[j+1].first >= end && row_restrictions[i][j+1] == 1) {
										// make the next cell empty
										if (cells[look_at_cell+1] == -1) {
											cells[look_at_cell+1] = 1;
											solved_indicator++;
										}
									}
								}
							}
						}
						else {
							// assuming puzzle size will be atleast 2x2 
							// checking for black run of length 1
							if (cells[look_at_cell] == 0 && cells[look_at_cell+1] != 0) {
								if (j < k-1) {
									if (eachrow_runranges[j+1].first >= end && row_restrictions[i][j+1] == 1) {
										// make the next cell empty
										if (cells[look_at_cell+1] == -1) {
											cells[look_at_cell+1] = 1;
											solved_indicator++;
										}
									}
								}
							}
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
					int16_t prev_row_same_col = (start-1)*n_cols + i;
					int16_t next_row_same_col = (start+1)*n_cols + i;

					if (start == n_rows-1) {
						if (cells[look_at_cell] == 0 && cells[prev_row_same_col] != 0) {
							// make the previous cell empty
							if (j > 0) {
								if (eachcol_runranges[j-1].second <= start && col_restrictions[i][j-1] == 1) {
									if (cells[prev_row_same_col] == -1) {
										cells[prev_row_same_col] = 1;
										solved_indicator++;
									}
								}
							}	
						}
					}
					else {
						if (cells[look_at_cell] == 0 && cells[prev_row_same_col] != 0 
							&& cells[next_row_same_col] != 0) {
							if (j > 0) {
								if (eachcol_runranges[j-1].second <= start && col_restrictions[i][j-1] == 1) {
									// make the previous cell empty
									if (cells[prev_row_same_col] == -1) {
										cells[prev_row_same_col] = 1;
										solved_indicator++;
									}
								}
							}
						}
					}
				}
				if (end < n_rows - 1) {
					// looks at j'th column and end'th row
					uint16_t look_at_cell = end*n_cols + i;
					int16_t prev_row_same_col = (end-1)*n_cols + i;
					int16_t next_row_same_col = (end+1)*n_cols + i;

					if (end != 0) {
						if (cells[look_at_cell] == 0 && cells[prev_row_same_col] != 0 
							&& cells[next_row_same_col] != 0) {
							if (j < k-1) {
								if (eachcol_runranges[j+1].first >= end && col_restrictions[i][j+1] == 1) {
									// make the next cell empty
									if (cells[next_row_same_col] == -1) {
										cells[next_row_same_col] = 1;
										solved_indicator++;
									}
								}
							}
						}
					}
					else {
						if (cells[look_at_cell] == 0 && cells[next_row_same_col] != 0) {
							if (j < k-1) {
								if (eachcol_runranges[j+1].first >= end && col_restrictions[i][j+1] == 1) {
									// make the next cell empty 
									if (cells[next_row_same_col] == -1) {
										cells[next_row_same_col] = 1;
										solved_indicator++;
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void Puzzle::rule_4(int8_t row_or_col) {

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
						int16_t go_left = i-1;
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
							if (cells[i] == -1) {
								cells[i] = 1;
								solved_indicator++;
							}
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

							int16_t go_up = i - n_cols;
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
								if (cells[i] == -1) {
									cells[i] = 1;
									solved_indicator++;
								}
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

void Puzzle::expand_and_limit(uint16_t total, uint16_t perpTotal, vector<vector<uint16_t>> *restrictions,
	vector<vector<range>> *black_runs, bool isCol) {

	// iterates thru all lines 
	for (uint16_t line = 0; line < total; line++){

		// iterate thru all cells in the line
		for (uint16_t celli = 0; celli < perpTotal; celli++) {
			// if doing it with rows celli is the column number 0-n_col
			// if doing it with cols, celli is the row number
			// find a black cell

			uint16_t cellInd;
			// alter index accordingly depending if you're iterating thru columns or rows
			if (isCol) {
				cellInd = n_cols*celli +line;
			}
			else {
				cellInd = n_cols*line+celli;
			}
			
			if (cells[cellInd] == 0) {
				// find the smallest run that could be in celli
				uint16_t runs_per_line = (*black_runs)[line].size();
				vector <uint16_t> runs_in_range;

				for (uint16_t run = 0; run <runs_per_line; run++){

					range my_range = (*black_runs)[line][run];

					// if the run's range includes the cell
					if (celli >= my_range.first && celli <= my_range.second) {
						// push the length of the run
						runs_in_range.push_back((*restrictions)[line][run]);
					}
				}
				// find minimum length 
				uint16_t minlen = *min_element(runs_in_range.begin(), runs_in_range.end());

				if (adjacent_find(runs_in_range.begin(), runs_in_range.end(), not_equal_to<>() ) == runs_in_range.end()) {
				    // elements in vec are all the same


				    // -1 because it is counted twice
				    int16_t tally = -1;
				    int16_t start = celli;
				    uint16_t end = celli;

				    bool atRightEnd = false;
				    bool atLeftEnd = false;
				    // go before until you reach an unknown or a white
				    while (start > -1 && cells[cellInd] == 0) {
				    	tally++;
				    	// move on
				    	start--;
				    	if (isCol) {
							cellInd = n_cols*start +line;
						}
						else {
							cellInd = n_cols*line+start;
						}
				    }

				    // adjust appropiately
				    if (isCol) {
						cellInd = n_cols*celli +line;
					}
					else {
						cellInd = n_cols*line+celli;
					}

				    // go after until you reach an unknown or a white
				    while (end < perpTotal && cells[cellInd] == 0) {
				    	tally++;
				    	// move on
				    	end++;

				    	// adjust appropiately
				    	if (isCol) {
							cellInd = n_cols*end +line;
						}
						else {
							cellInd = n_cols*line+end;
						}
				    }

				    // check if you touched the wall
				    if(end == perpTotal){
				    	atRightEnd = true;
				    }
				    if (start == -1) {
				    	atLeftEnd = true;
				    }

				    // if the tally is the length, we can colour some whites
				    if (tally == minlen) {
				    	// whiten the start -1
				    	if (isCol) {
				    		// cellInd currently contains the end cell
				    		
				    		if (!atRightEnd && cells[cellInd]== -1) {
								// colour it
								cells[cellInd] = 1;
								solved_indicator++;
							}


							// possibly whiten the start
							cellInd = n_cols*start +line;
						
							if (!atLeftEnd && cells[cellInd]== -1) {
								// colour it
								cells[cellInd] = 1;
								solved_indicator++;
							}
						}
						else {
							// cellInd currently contains the end cell
							if (!atRightEnd && cells[cellInd]== -1) {
								// colour it
								cells[cellInd] = 1;
								solved_indicator++;
							}

							// possibly whiten the start 
							cellInd = n_cols*line+start;
							if (!atLeftEnd && cells[cellInd]== -1) {
								// colour it
								cells[cellInd] = 1;
								solved_indicator++;
							}
						}
				    }
				}

				// expand before?
				// check to see if there is a white or a wall before the black
				int16_t before = find_white_or_wall(celli-minlen+1, celli-1, -1, perpTotal, line, isCol);

				// expand after?
				// check to see if there is a white or a wall after the black
				int16_t after = find_white_or_wall(celli+1, celli+minlen-1, 1, perpTotal, line, isCol);

				// if there's a white or a wall, go colour
				if (before != -2) {
					push_to_colour(celli+1, before + minlen, line, isCol);
				}
				if (after != -2) {
					push_to_colour(after-minlen, celli-1, line, isCol);
				}
				//try to find another black cell
			}
			// if there are no black cells, rule does not apply
		}
	}
}

void Puzzle::rule_6(int8_t row_or_col) {
	if (row_or_col == 1) {
		// iterating over every row
		for (uint16_t i=0; i<n_rows; i++) {
			vector<range> eachrow_runranges = row_black_runs[i];
			uint16_t k = eachrow_runranges.size();

			// iterating over every black run
			for (uint16_t j=0; j<k; j++) {
				uint16_t start = eachrow_runranges[j].first;
				uint16_t end = eachrow_runranges[j].second;

				if (k > 1) {
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

				if (k > 1) {
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
				}
				// refine the run range of this particular black run
				eachcol_runranges[j] = range(start, end);
			}
			// refine the run ranges for all the runs in this particular column
			col_black_runs[i] = eachcol_runranges;
		}
	}
}

void Puzzle::rule_7(int8_t row_or_col) {
	if (row_or_col == 1) {
		// iterating over every row
		for (uint16_t i=0; i<n_rows; i++) {
			vector<range> eachrow_runranges = row_black_runs[i];
			uint16_t k = eachrow_runranges.size();

			// iterating over every black run
			for (uint16_t j=0; j<k; j++) {
				uint16_t start = eachrow_runranges[j].first;
				uint16_t end = eachrow_runranges[j].second;

				if (k != 1) {
					// run range can only get modified if there is more than one black run
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

				if (k > 1) {
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
				}
				// refine run range for this particular black run
				eachcol_runranges[j] = range(start, end);
			}
			// refine all run ranges for this particular column
			col_black_runs[i] = eachcol_runranges;
		}
	}
}

void Puzzle::rule_8(int8_t row_or_col) {
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

				if (k > 1) {
					// we want to find out all the black segments covered by the range 
					// of this particular black run and find the starting and ending points
					// of each black segment; so we have to start by looking at the first
					// cell covered by the range of this black run
					vector<range> temp;
					uint16_t iter = start;
					uint16_t look_at_cell = i*n_cols + start;
					uint16_t curr_row = i;

					while (iter <= end && curr_row == i) {
						if (cells[look_at_cell] == 0) {
							iter = find_and_insert(look_at_cell, 1, iter, &temp, curr_row);
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
							if (temp[m].second - temp[m].first + 1 > row_restrictions[i][j]) {
								uint16_t length = temp[m].second - temp[m].first + 1;
								int16_t curr_run = j;
								int16_t left = j - 1;
								int16_t right = j + 1;
								bool is_left = false;
								bool is_right = false;
								while (left >= 0 || right <= k) {
									if (left >= 0 && length <= row_restrictions[i][left] && 
										eachrow_runranges[left].first <= temp[m].first
										&& eachrow_runranges[left].second >= temp[m].second) {
										
										if (right <= k && length <= row_restrictions[i][right] && 
										eachrow_runranges[right].first <= temp[m].first
										&& eachrow_runranges[right].second >= temp[m].second) {

											is_right = true;
											
										}
										is_left = true;
										break;
									}
									if (right <= k && length <= row_restrictions[i][right] && 
										eachrow_runranges[right].first <= temp[m].first
										&& eachrow_runranges[right].second >= temp[m].second) {

										if (left >= 0 && length <= row_restrictions[i][left] && 
										eachrow_runranges[left].first <= temp[m].first
										&& eachrow_runranges[left].second >= temp[m].second) {

											is_left = true;
										}

										is_right = true;
										break;
									}
									left -= 1;
									right += 1;
								}
								if (is_left && is_right) {
									continue;
								}
								else if (is_left) {
									// the black seg belongs to former black runs
									// js = ie + 2
									start = temp[m].second + 2;
								}
								else if (is_right) {
									// the black seg belongs to later black runs
									// je = is - 2
									end = temp[m].first - 2;
								}
							}
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

				if (k > 1) {
					vector<range> temp;
					uint16_t iter = start;
					uint16_t look_at_cell = start*n_cols + i;
					uint16_t curr_col = i;

					while (iter <= end && curr_col == i) {
						if (cells[look_at_cell] == 0) {
							iter = find_and_insert(look_at_cell, 2, iter, &temp, curr_col);
							look_at_cell = iter*n_cols + i;
						}
						else {
							iter++;
							look_at_cell += n_cols;
							
							curr_col = look_at_cell % n_cols;
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
							if (temp[m].second - temp[m].first + 1 > col_restrictions[i][j]) {
								uint16_t length = temp[m].second - temp[m].first + 1;
								int16_t curr_run = j;
								int16_t left = j - 1;
								int16_t right = j + 1;
								bool is_left = false;
								bool is_right = false;
								while (left >= 0 || right <= k) {
									if (left >= 0 && length <= col_restrictions[i][left] && 
										eachcol_runranges[left].first <= temp[m].first
										&& eachcol_runranges[left].second >= temp[m].second) {
										
										if (right <= k && length <= col_restrictions[i][right] && 
										eachcol_runranges[right].first <= temp[m].first
										&& eachcol_runranges[right].second >= temp[m].second) {

											is_right = true;
										}

										is_left = true;
										break;
									}
									if (right <= k && length <= col_restrictions[i][right] && 
										eachcol_runranges[right].first <= temp[m].first
										&& eachcol_runranges[right].second >= temp[m].second) {

										if (left >= 0 && length <= col_restrictions[i][left] && 
										eachcol_runranges[left].first <= temp[m].first
										&& eachcol_runranges[left].second >= temp[m].second) {

											is_left = true;
										}


										is_right = true;
										break;
									}
									left -= 1;
									right += 1;
								}
								if (is_left && is_right) {
									continue;
								}
								else if (is_left) {
									// the black seg belongs to former black runs
									// js = ie + 2
									start = temp[m].second + 2;
								}
								else if (is_right) {
									// the black seg belongs to later black runs
									// je = is - 2
									end = temp[m].first - 2;
								}
							}
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

void Puzzle::fill_in_void(uint16_t total, uint16_t perpTotal, vector<vector<uint16_t>> *restrictions, vector<vector<range>> *black_runs, bool isCol) {
	// iterate thru each line
	for (uint16_t line = 0; line < total; line++) {

		uint16_t runs_per_line = (*restrictions)[line].size();

		// iterate thru all runs in the line
		// skip the first and last runs because those won't have runs prior or after themselves
		for (uint16_t runs = 0; runs <runs_per_line; runs++) {
			// assume that there is some black
			bool noBlackBox = false;

			// get index of the startpoint
			uint16_t startLine;
			// if it's the first run in the line
			if (runs == 0){
				startLine = 0;
			}
			// if it's any other run
			else {
				startLine = (*black_runs)[line][runs-1].second + 1; 
				// +1 if you wanna exclude the black run AT r(j-1)e
			}
			uint16_t start = n_cols*line+ startLine;

			if (isCol) {
				start = n_cols*startLine+ line;
			}

			// iterate thru cells in the line until finding black box
			while (startLine < perpTotal && cells[start] != 0) {
				startLine++;

				if (isCol) {
					start = n_cols*startLine+ line;
				}
				else {
					start = n_cols*line+ startLine;
				}

				// you've reached a wall and still found no black
				if (startLine >= perpTotal ) {
					noBlackBox = true;
				}
				// if you found a black box its position is saved in startLine
			}

			// if you found no black boxes in the line, go to the next line.
			if (noBlackBox) {
				continue;
			}


			// get index of endpoint
			int16_t endLine;
			// if it's the last run in the line
			if (runs == runs_per_line -1){
				// start from the highest place
				endLine = perpTotal-1;
			}
			// if it's any other run
			else {
				endLine = (*black_runs)[line][runs+1].first -1;
				// -1 to avoid counting yourself

			}

			uint16_t end = n_cols*line+ endLine;
			if (isCol) {
				end = n_cols*endLine+ line;
			}

			// iterate thru cells in the line backwards until finding black box
			while (endLine >= 0 && cells[end] != 0) {
				endLine--;

				if (isCol) {
					end = n_cols*endLine+ line;
				}
				else {
					end = n_cols*line+ endLine;
				}
				// you've reached a wall and still found no black
				if (endLine <= -1 ) {
					noBlackBox = true;
				}
				// if you found a black box its position is saved in endLine
			}

			// if you found no black boxes in the line, go to the next line.
			if (noBlackBox) {
				continue;
			}

			bool connected = true;

			// check that the indices can be connected (no whites in between)
			uint16_t keepChecking = startLine;
			while (keepChecking < endLine) {
				// if there is white between them
				if (cells[start] == 1) {

					connected = false;
					// to get out of the loop
					keepChecking = endLine;
				}

				keepChecking++;
				// adjust accordingly
				if (isCol) {
					start = n_cols*keepChecking+ line;
				}
				else {
					start = n_cols*line+ keepChecking;
				}
			}

			// the case where, we leave 3.2 to deal with this issue because there is no way
			// of connecting startLine to endLine (there are whites in between)
			if (!connected) {
				continue;
			}

			// if you found nothing, or the black box is next to the wall endLine == -1
			// colour everything between startpoint and endpoint
			push_to_colour(startLine, endLine, line, isCol);

			if (endLine >= startLine) {
				//change the range of the current run that you just filled in
				(*black_runs)[line][runs].first = endLine + 1 - (*restrictions)[line][runs];

				(*black_runs)[line][runs].second = (*restrictions)[line][runs] + startLine -1;
			}		
		}
	}
}

void Puzzle::mod_range_to_fit(uint16_t total, uint16_t perpTotal, vector<vector<uint16_t>> *restrictions,
	vector<vector<range>> *black_runs, bool isCol) {
	 // iterate thru each line
	for (uint16_t line = 0; line < total; line++) {

		uint16_t runs_per_line = (*restrictions)[line].size();
		// iterate thru all runs in the line
		for (uint16_t runs = 0; runs <runs_per_line; runs++) {
			vector <range> white_segs;
			// TASK 1: Count segments enclosed by whites

			// find low_range: the index of the first nonwhite cell
			// get the lower line index
			int16_t low_range =(*black_runs)[line][runs].first; // rjs, 

			// find high_range: the index of the last nonwhite cell
			int16_t high_range =(*black_runs)[line][runs].second; // rje
			
			// get the high cell index
			uint16_t high_range_cell;
			if (isCol) {
				high_range_cell = n_cols*high_range + line;
			}
			else {
				high_range_cell = n_cols*line+high_range;
			}

			// get the lower cell index
			uint16_t low_range_cell;
			if (isCol) {
				low_range_cell = n_cols*low_range + line;
			}
			else {
				low_range_cell = n_cols*line+low_range;
			}

			// trim until you find a cell that is not white
			while (cells[low_range_cell] == 1 && low_range <= high_range) {
				low_range++;

				// make sure you're checking the correct cell next iteration
				if (isCol) {
					low_range_cell = n_cols*low_range + line;
				}
				else {
					low_range_cell = n_cols*line+low_range;
				}
			}

			// the case where all cells in the range are white, which doesn't really make sense
			// probably will never happen?
			if (cells[low_range_cell] == 1) {
				// get out, this rule doesn't apply to this range
				continue; 
			}

			// trim until you find a cell that is not white
			while (cells[high_range_cell] == 1 && high_range >= low_range) {

				high_range--;

				// make sure you're checking the correct cell next iteration
				if (isCol) {
					high_range_cell = n_cols*high_range + line;
				}
				else {
					high_range_cell = n_cols*line+high_range;
				}
			}

			// stores the number of segments enclosed by whites
			uint16_t white_count = 0;
			// stores the indices of the segment
			uint16_t seg_start = low_range_cell;
			bool wasWhite = false;

			// iterate from lower cell to higher cell to find the number of segments
			for (uint16_t rising = low_range_cell; rising <= high_range_cell; rising++, low_range++)
			{

				// adjust to appropiate cell
				if (isCol) {
					rising = n_cols*low_range + line;
				}

				// if you find a white cell, count it
				if (cells[rising] == 1) {

					// make sure that it counts the actual different segments,
					// not just the number of white cells
					if (!wasWhite) {
						uint16_t uplim;
						// adjust accordingly
						if (isCol) {
							uplim = rising- n_cols;
						}
						else{
							uplim = rising -1;
						}

						white_segs.push_back(range(seg_start, uplim));
						white_count++;
					}
					wasWhite= true;
					if (!isCol) {
						// move one to the right
						seg_start = rising +1;
					}
					else{
						// move one down
						seg_start = rising + n_cols;
					}
				}
				// the current cell isn't white
				else if (cells[rising] != 1) {
					wasWhite = false;
				}

				// the last segment
				if (rising == high_range_cell){
					white_segs.push_back(range(seg_start, rising));
					white_count++;
				}
			}

			// END OF TASK 1
			// Start looking thru the segments
			// STEP 1
			int16_t seg_num = 0;

			uint16_t seg_len;
			if (isCol) {
				// get row ind
				seg_len = white_segs[seg_num].second - white_segs[seg_num].first +n_cols;
				seg_len= floor ((seg_len- line)/n_cols) + 1;
			}
			else {
				// get column ind
				seg_len = white_segs[seg_num].second - white_segs[seg_num].first +1;
			}

			// STEP 2
			// if the the current run doesn't fit in the segment
			while (seg_len < (*restrictions)[line][runs] && seg_num < white_count) {
				// go to the next segment
				seg_num++;
				seg_len = white_segs[seg_num].second - white_segs[seg_num].first +1;
			}

			// change the range of this run accordingly
			if (isCol) {
				(*black_runs)[line][runs].first= floor ((white_segs[seg_num].first- line)/n_cols); 
			}
			else {
				// get column ind
				(*black_runs)[line][runs].first=  (white_segs[seg_num].first- line*n_cols); 
			}

			// save it for later
			uint16_t init_seg = seg_num;
			// STEP 3, now start from the top 
			seg_num = white_count -1;

			// get the index
			if (isCol) {
				seg_len = white_segs[seg_num].second - white_segs[seg_num].first +n_cols;
				seg_len= floor ((seg_len- line)/n_cols) + 1;
			}
			else {
				seg_len = white_segs[seg_num].second - white_segs[seg_num].first +1;
			}

			
			// STEP 4
			// if the the current run doesn't fit in the segment
			while (seg_len < (*restrictions)[line][runs] && seg_num >= 0) {
				// go to the next segment
				seg_num--;	
				seg_len = white_segs[seg_num].second - white_segs[seg_num].first +1;	
			}

			// change the range of this run accordingly
			if (isCol) {
				(*black_runs)[line][runs].second= floor ((white_segs[seg_num].second- line)/n_cols); 
			}
			else {
				// get column ind
				(*black_runs)[line][runs].second=  (white_segs[seg_num].second- line*n_cols); 
			}

			// Step 5
			// iterate thru remaining segs
			for (uint16_t seg = init_seg; seg < seg_num; seg++) {

				// turns cell number into an index
				uint16_t perplinestart, perplinend;
				if (isCol) {
					// get row ind
					perplinestart= floor ((white_segs[seg].first- line)/n_cols); 
					perplinend = floor ((white_segs[seg].second- line)/n_cols); 
				}
				else {
					// get column ind
					perplinestart=  (white_segs[seg].first- line*n_cols); 
					perplinend = (white_segs[seg].second- line*n_cols); 
				}

				// get length of the segment
				if (isCol) {
					seg_len = white_segs[seg].second - white_segs[seg].first +n_cols;
					seg_len= floor ((seg_len- line)/n_cols) + 1;
				}
				else {
					seg_len = white_segs[seg].second - white_segs[seg].first +1;
				}

				// is the segment smaller than the run?
				if (seg_len < (*restrictions)[line][runs]) {

					// find if segment covered by any other ranges
					uint16_t tracker = 0;

					// iterate thru all runs in the line
					for (uint16_t each_run = 0; each_run < runs_per_line; each_run++) {

						// is the remaining seg enclosed within anything 
						// other than the already established black run?
						range my_range = (*black_runs)[line][each_run];
						if (my_range.first <= perplinestart && my_range.second >= perplinend) {
							tracker++;
						}
					}

					// is it covered by someone else?
					if (tracker > 1) {

						// colour it white from the segment's start till the segment's end
						for (int ind = perplinestart; ind <= perplinend; ind++) {

							uint16_t colind = n_cols*line+ind;
							// adjust index accordingly if you're iterating thru columns
							if (isCol) {
								colind = n_cols*ind + line;
							}
							// if the cell hasn't been coloured yet
							if (cells[colind]== -1) {
								// colour it
								cells[colind] = 1;
								solved_indicator++;
							}
						}
					}
				}
			}
		}
	}
}


void Puzzle::rule_11(int8_t row_or_col) {
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

				// rj not overlapping with rj-1
				if (j > 0) {
					// if (j-1)e < js; j's range does not overlap with j-1's range
					if (eachrow_runranges[j-1].second < start) {
						
						// CASE 1: Crjs is coloured
						
						if (cells[i*n_cols+start] == 0) {
							// length of this black run
							//uint16_t length_of_j = row_restrictions[i][j];
							for (uint16_t m = start+1; m <= start + row_restrictions[i][j] - 1; m++) {
								// COLOUR THE CELLS
								if (cells[i*n_cols+m] == -1) {
									cells[i*n_cols+m] = 0;
									solved_indicator++;
								}
							}
							// refine the run range of this black run
							// je = js + LBj - 1
							end = start + row_restrictions[i][j] - 1;

							// leave cells js-1 and je+1 empty
							if (cells[i*n_cols+start-1] == -1) {
								cells[i*n_cols+start-1] = 1;
								solved_indicator += 1;
							}

							// if the (end+1)'th cell is in the same row
							if ((i*n_cols + end + 1)/n_cols == i) {
								if (cells[i*n_cols+end+1] == -1) {
									cells[i*n_cols+end+1] = 1;
									solved_indicator += 1;
								}
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
						for (uint16_t n=start; n < end-1; n++) {
							if (cells[i*n_cols+n]==0 && cells[i*n_cols+n+1]==1) {
								// if an empty cell appears after a black cell in the run range of j
								// je = n+1 - 1
								end = n;
								break;
							}
						}

						// CASE 3
						uint16_t iter = start;
						vector<range> temp;
						uint16_t curr_row = i;
						while (iter <= end && curr_row == i) {
							if (cells[i*n_cols+iter] == 0) {
								// start of a black segment
								uint16_t start_seg = iter;
								while (iter <= end && cells[i*n_cols+iter] == 0 && curr_row == i) {
									iter++;
									curr_row = (i*n_cols+iter)/n_cols;
								}
								uint16_t end_seg = iter - 1;

								// insert the start and end positions of this black segments
								// in the set of all black segments in the run range of j
								temp.push_back(range(start_seg, end_seg));
							}
							else {
								iter++;
								curr_row = (i*n_cols+iter)/n_cols;
							}
						}
						//cout << "got here" << endl;
						if (temp.size() >= 2) {
							//uint16_t length_of_j = row_restrictions[i][j];
							uint16_t start_0 = temp[0].first;
							uint16_t iter_2 = 1;

							while (iter_2 < temp.size()) {
								if (temp[iter_2].second - start_0 + 1 > row_restrictions[i][j]) {
									end = temp[iter_2].first - 2;
									break;
								}
								else {
									iter_2++;
								}
							}
						}
					}
				}
				// rj not overlapping with rj+1
				if (j < k-1) {
					// if (j+1)s > je, rj does not overlap with rj+1
					if (eachrow_runranges[j+1].first > end) {

						// CASE 1: Crje is coloured
						if (cells[i*n_cols+end] == 0) {
							// length of this black run
							//uint16_t length_of_j = row_restrictions[i][j];
							
							for (int16_t m=end-1; m >= end-row_restrictions[i][j] + 1; m--) {
								// COLOUR THE CELLS
								if (cells[i*n_cols+m] == -1) {
									cells[i*n_cols+m] = 0;
									solved_indicator++;
								}
							}
							// refine the run range of this black run
							// js = je - LBj + 1
							start = end - row_restrictions[i][j] + 1;

							// leave cells js-1 and je+1 empty
							if (cells[i*n_cols+end+1] == -1) {
								cells[i*n_cols+end+1] = 1;
								solved_indicator++;
							}

							// if (start-1)'th cell is in the same row
							if (start - 1 > 0) {
								if (cells[i*n_cols+start-1] == -1) {
									cells[i*n_cols+start-1] = 1;
									solved_indicator++;
								}
							}
							// if rj-1 overlaps with rj
							if (j > 0) {
								if (eachrow_runranges[j-1].second >= start) {
									// (j-1)e = js - 2;
									eachrow_runranges[j-1].second = start - 2;
								}
							}
							if (eachrow_runranges[j+1].first == end + 1) {
								eachrow_runranges[j+1].first = end + 2;
							}
						}
						// CASE 2
						for (uint16_t n=end; n > start; n--) {
							// if an empty cell appears before a black or unknown cell 
							if (cells[i*n_cols+n] == 0 && cells[i*n_cols+n-1] == 1) {
								// js = n-1+1
								start = n;
								break;
							}
						}

						// CASE 3
						uint16_t iter = start;
						vector<range> temp;
						uint16_t curr_row = i;
						while (iter <= end && curr_row == i) {
							if (cells[i*n_cols+iter] == 0) {
								// start of a black segment
								uint16_t start_seg = iter;
								while (iter <= end && cells[i*n_cols+iter] == 0 && curr_row == i) {
									iter++;
									curr_row = (i*n_cols+iter)/n_cols;
								}
								uint16_t end_seg = iter - 1;

								// insert the start and end positions of this black segments
								// in the set of all black segments in the run range of j
								temp.push_back(range(start_seg, end_seg));
							}
							else {
								iter++;
								curr_row = (i*n_cols+iter)/n_cols;
							}
						}

						if (temp.size() >= 2) {
							uint16_t b = temp.size();
							uint16_t end_2 = temp[b-1].second;
							uint16_t iter_2 = b - 2;

							while (iter_2 >= 0) {
								if (end_2 - temp[iter_2].first + 1 > row_restrictions[i][j]) {
									start = temp[iter_2].second + 2;
									break;
								}
								else {
									iter_2++;
								}
							}
						}
					}
				}
				eachrow_runranges[j] = range(start, end);

			}
			row_black_runs[i] = eachrow_runranges;
		}
	}
	else if (row_or_col == 2) {
		// iterating over every column
		for (uint16_t i=0; i<n_cols; i++) {
			vector<range> eachcol_runranges = col_black_runs[i];
			uint16_t k = eachcol_runranges.size();

			for (uint16_t j=0; j<k; j++) {
				uint16_t start = eachcol_runranges[j].first;
				uint16_t end = eachcol_runranges[j].second;

				// rj not overlapping with rj-1
				if (j > 0) {
					// if (j-1)e < js, j's range does not overlap with (j-1)'s range
					if (eachcol_runranges[j-1].second < start) {

						// CASE 1: Crjs is coloured
						if (cells[start*n_cols+i] == 0) {
							// length of the black run
							//uint16_t length_of_j = col_restrictions[i][j];
							for (uint16_t m=start+1; m <= col_restrictions[i][j] + start - 1; m++) {
								// COLOUR THE CELLS!
								if (cells[m*n_cols+i] == -1) {
									cells[m*n_cols+i] = 0;
									solved_indicator++;
								}
							}
							// refine the run range of this black run
							end = start + col_restrictions[i][j] - 1;

							// leave the cells js-1 and je+1 empty
							if (cells[(start-1)*n_cols + i] == -1) {
								cells[(start-1)*n_cols + i] = 1;
								solved_indicator++;
							}
							if (end + 1 < n_rows) {
								if (cells[(end+1)*n_cols+i] == -1) {
									cells[(end+1)*n_cols+i] = 1;
									solved_indicator++;
								}
							}
							if (j < k-1) {
								// if the range of j+1 overlaps with j
								if (eachcol_runranges[j+1].first <= end) {
									// (j+1)s = je + 2
									eachcol_runranges[j+1].first = end + 2;
								}
							}
							if (eachcol_runranges[j-1].second == start-1) {
								eachcol_runranges[j-1].second = start - 2;
							}
						}
						// CASE 2
						for (uint16_t n=start; n<end-1; n++) {
							if (cells[n*n_cols+i] == 0 && cells[(n+1)*n_cols+i] == 1) {
								end = n;
								break;
							}
						}
						// CASE 3
						uint16_t iter = start; 
						vector<range> temp;
						uint16_t curr_col = i;
						while (iter <= end && curr_col == i) {
							if (cells[iter*n_cols+i] == 0) {
								// start of a black segment
								uint16_t start_seg = iter;
								while (iter <= end && cells[iter*n_cols+i] == 0 && curr_col == i) {
									iter++;
									curr_col = (iter*n_cols+i) % n_cols;
								}
								uint16_t end_seg = iter - 1;
								temp.push_back(range(start_seg, end_seg));
							}
							else { 
								iter++; 
								curr_col = (iter*n_cols+i) % n_cols;
							}
						}
						if (temp.size() >= 2) {
							uint16_t start_0 = temp[0].first;
							uint16_t iter_2 = 1;
							while (iter_2 < temp.size()) {
								if (temp[iter_2].second - start_0 + 1 > col_restrictions[i][j]) {
									end = temp[iter_2].first - 2;
									break;
								}
								else { iter_2++; }
							}
						}
					}
				}
				// rj not overlapping with rj+1;
				if (j < k-1) {
					// if (j+1)s > je, rj does not overlap with rj+1
					if (eachcol_runranges[j+1].first > end) {

						// CASE 1: Crje is coloured
						if (cells[end*n_cols+i] == 0) {
							for (int16_t m=end-1; m >= end - col_restrictions[i][j] + 1; m--) {
								// COLOUR THE CELLS
								if (cells[m*n_cols+i] == -1) {
									cells[m*n_cols+i] = 0;
									solved_indicator++;
								}
							}
							// refine the run range of this black run
							// js = je - LBj + 1
							start = end - col_restrictions[i][j] + 1;

							// leave cells js-1 and je+1 empty
							if (cells[(end+1)*n_cols+i] == -1) {
								cells[(end+1)*n_cols+i] = 1;
								solved_indicator++;
							} 
							// if (start-1)'th cell is in the same column
							if (start - 1 > 0) {
								if (cells[(start-1)*n_cols + i] == -1) {
									cells[(start-1)*n_cols + i] = 1;
									solved_indicator++;
								}
							}
							// if rj-1 overlaps with rj 
							if (j > 0) {
								if (eachcol_runranges[j-1].second >= start) {
									// (j-1)e = js - 2
									eachcol_runranges[j-1].second = start - 2;
								}
							}
							if (eachcol_runranges[j+1].first == end + 1) {
								eachcol_runranges[j+1].first = end + 2;
							}
						}
						// CASE 2
						for (uint16_t n=end; n > start; n--) {
							// if an empty cell appears before a black or unknown cell
							if  (cells[n*n_cols+i] == 0 && cells[(n-1)*n_cols+i] == 1) {
								start = n;
								break;
							}
						}
						// CASE 3
						uint16_t iter = start; 
						vector<range> temp;
						uint16_t curr_col = i;
						while (iter <= end && curr_col == i) {
							if (cells[iter*n_cols+i] == 0) {
								// start of a black segment
								uint16_t start_seg = iter;
								while (iter <= end && cells[iter*n_cols+i] == 0 && curr_col == i) {
									iter++;
									curr_col = (iter*n_cols+i) % n_cols;
								}
								uint16_t end_seg = iter - 1;
								temp.push_back(range(start_seg, end_seg));
							}
							else { 
								iter++; 
								curr_col = (iter*n_cols+i) % n_cols;
							}
						}
						if (temp.size() >= 2) {
							uint16_t b = temp.size();
							uint16_t end_2 = temp[b-1].second;
							uint16_t iter_2 = b - 2;

							while (iter_2 >= 0) {
								if (end_2 - temp[iter_2].first + 1 > col_restrictions[i][j]) {
									start = temp[iter_2].second + 2;
									break;
								}
								else { iter_2++; }
							}
						}
					}
				}
				eachcol_runranges[j] = range(start, end);
			}
			col_black_runs[i] = eachcol_runranges;
		}
	}
}