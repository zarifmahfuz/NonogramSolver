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

					uint16_t lower_lim = 0;
					for (int j_2 = 0; j_2<j; j_2++) {
						lower_lim = lower_lim + each_row_restrictions[j_2] + 1;
					}
				}
				else {
					uint16_t lower_lim = 0;
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
					uint16_t cumsum = 0;

					for (int j_2 = j+1; j_2<k; j_2++) {
						cumsum = cumsum + each_col_restrictions[j_2] + 1;
					}
					upper_lim = n_rows - 1 - cumsum; 
				}
				else if (j==k-1) {
					upper_lim = n_rows - 1;

					uint16_t lower_lim = 0;
					for (int j_2 = 0; j_2<j; j_2++) {
						lower_lim = lower_lim + each_col_restrictions[j_2] + 1;
					}

				}
				else {
					uint16_t lower_lim = 0;
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
};



#endif