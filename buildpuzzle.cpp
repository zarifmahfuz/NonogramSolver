#include <iostream>

#include <algorithm>

#include "buildpuzzle.h"


using namespace std;

// Rule 1.1 Checks overlap between black runs
// total is n_rows if you're checking for overlap in rows
// total is n_cols if you're checking for overlap in columns
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
			//cout << "top: " << leftmost << " bottom: " <<rightmost <<endl;
			// colour the runs when the leftmost overlaps with rightmost

			for (int16_t ind = leftmost; ind <= rightmost; ind++) {

				uint16_t converted_ind = ind;

				// convert to the numbering system that we're actually using 
				if (isCol){
					// then ind == rownum + colnum*totalRows, because it's numbering vertically
					// ind -(n_rows-1)*colnum +(n_cols-1)*rownum converts from vertical numbering to horiz
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

// private helper method
// if you find nothing, send n_cols +1
// i just realized this could be better but i'm tired. maybe i'll fix later :(((
int16_t Puzzle::find_white_or_wall(int16_t bottomLim, int16_t topLim, int8_t incr_or_decr) {

	// going up
	if (incr_or_decr == 1) {
		// try to find a white
		while (bottomLim <=topLim) {
			// if you find a white, return the cell
			if (cells[bottomLim] == 1) {
				return bottomLim;
			}
			bottomLim = bottomLim + incr_or_decr;
		}
		// still nothing? check if there's a wall
		if (topLim >=n_cols) {
			return n_cols;
		}
		// found nothing
		else{
			return n_cols +1;
		}
	}
	// going down
	else {
		// try to find a white
		while (bottomLim >=topLim) {
			// if you find a white, return the cell
			if (cells[topLim] == 1) {
				return topLim;
			}
			topLim = topLim + incr_or_decr;
		}
		// still nothing? check if there's a wall
		if (bottomLim <= -1) {
			return -1;
		}
		// found nothing
		else{
			return n_cols +1;
		}
	}
}

// private boi, colour from low to high 
void Puzzle::push_to_colour(int16_t low, int16_t high, uint16_t line) {
	for (int ind = low; ind <= high; ind++) {

		// if the cell hasn't been coloured yet
		uint16_t colind = n_cols*line+ind;
		if (cells[colind]== -1) {
			// colour it
			cells[colind] = 0;
			solved_indicator++;
		}
	}
}

// Rule 1.5
// expands black runs near whites or near walls
// limits black runs to prevent them from exceeding a size they cannot have (otherwise it wouldn't fit)
void Puzzle::expand_and_limit(uint16_t total, uint16_t perpTotal, vector<vector<uint16_t>> *restrictions,
	vector<vector<range>> *black_runs, bool isCol){

	// iterates thru all lines 
	for (uint16_t line = 0; line < n_rows; line++){

		// iterate thru all cells in the line
		for (uint16_t celli = 0; celli < n_cols; celli++) {
			//celli is the column number 0-n_col

			// find a black cell
			if (cells[n_cols*line+celli] == 1) {

				// find the smallest run that could be in celli
				uint16_t runs_per_line = row_black_runs[line].size();
				vector <uint16_t> runs_in_range;

				for (uint16_t run = 0; run <runs_per_line; run++){

					range my_range = row_black_runs[line][run];
					// if the run's range includes the cell
					if (celli >= my_range.first && celli <= my_range.second) {
						// push the length of the run
						runs_in_range.push_back(row_restrictions[line][run]);
					}
				}
				// find minimum length 
				uint16_t minlen = *min_element(runs_in_range.begin(), runs_in_range.end());

				// expand before?
				// check to see if there is a white or a wall before the black
				int16_t before = find_white_or_wall(celli-minlen+1, celli-1, -1);

				// expand after?
				// check to see if there is a white or a wall after the black
				int16_t after = find_white_or_wall(celli+1, celli+minlen-1, 1);

				// if there's a white or a wall, go colour
				if (before != n_cols+1) {
					push_to_colour(celli+1, before + minlen, line);
				}
				if (after != n_cols+1) {
					push_to_colour(after-minlen, celli-1, line);
				}
				// try to find another black cell
			}

			// if there are no black cells, rule does not apply
		}
	}
}
