#include <iostream>

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
			}

		}
	}
}