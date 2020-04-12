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
		vector<vector<uint16_t>> col_restrictions);

	// this method initializes run ranges for each black run in each row/column
	// according to the formula provided in page 5 of the document
	void initialize_run_ranges();

	// Rule 1.1 Checks overlap between black runs
	// total: if you're iterating through columns, total = n_cols, if you're iterating through rows total = n_rows
	// perpTotal: if you're iterating through columns perpTotal = n_rows, if you're iterating through rows, perpTotal = n_cols
	// *restrictions: if checking rows, row_restrictions. if checking cols, col_restrictions 
	// *black_runs: if checking rows, row_black_runs. if checking cols, col_black_runs
	// isCol is true if checking cols, is false if checking rows
	void check_overlap(uint16_t total, uint16_t perpTotal, vector<vector<uint16_t>> *restrictions,
	vector<vector<range>> *black_runs, bool isCol);

	// implementing rule 1.2
	// not quite sure if we can apply a rule to both a row and a col at the same time
	// PARAMETER: apply to row: 1; apply to col: 2
	void rule_2(int8_t row_or_col);

	// implementing rule 1.3
	// PARAMETER: apply to row: 1; apply to col: 2
	void rule_3(int8_t row_or_col);

	// implementing rule 1.4 
	// Assumption: When we are looking at cell i, and cell Ci-1 and Ci+1 are coloured
	// Based on the above assumption, if we color cell i and the length of this black run
	// becomes greater the the maximum length of black run possible in the row/column,
	// the cell i should be left EMPTY
	// PARAMETER: apply to row: 1; apply to col: 2
	void rule_4(int8_t row_or_col);

	// Rule 1.5
	// expands black runs near whites or near walls
	// limits black runs to prevent them from exceeding a size they cannot have (otherwise it wouldn't fit)
	// Params:
	// 
	// total: if you're iterating through columns, total = n_cols, if you're iterating through rows total = n_rows
	// perpTotal: if you're iterating through columns perpTotal = n_rows, if you're iterating through rows, perpTotal = n_cols
	// *restrictions: if checking rows, row_restrictions. if checking cols, col_restrictions 
	// *black_runs: if checking rows, row_black_runs. if checking cols, col_black_runs
	// isCol is true if checking cols, is false if checking rows
	void expand_and_limit(uint16_t total, uint16_t perpTotal, vector<vector<uint16_t>> *restrictions,
		vector<vector<range>> *black_runs, bool isCol);

	// SECOND PHASE OF RULES
	// Objective: Refine the ranges of the black runs

	// implementing rule 2.1
	// PARAMETER: apply to row: 1; apply to col: 2
	void rule_6(int8_t row_or_col);

	// implementing rule 2.2
	// PARAMETER: apply to row: 1; apply to col: 2
	void rule_7(int8_t row_or_col);

	// implementing rule 2.3
	// PARAMETER: apply to row: 1; apply to col: 2
	void rule_8(int8_t row_or_col);

	// Rule 3.1
	// colours between black cells that are guaranteed to be an item
	// total: if you're iterating through columns, total = n_cols, if you're iterating through rows total = n_rows
	// perpTotal: if you're iterating through columns perpTotal = n_rows, if you're iterating through rows, perpTotal = n_cols
	// *restrictions: if checking rows, row_restrictions. if checking cols, col_restrictions 
	// *black_runs: if checking rows, row_black_runs. if checking cols, col_black_runs
	// isCol is true if checking cols, is false if checking rows
	void fill_in_void(uint16_t total, uint16_t perpTotal, vector<vector<uint16_t>> *restrictions,
	vector<vector<range>> *black_runs, bool isCol);

	// Rule 3.2
	// modifies the range of a run so that it is only hypothesized to fit somewhere where it actually CAN fit
	// total: if you're iterating through columns, total = n_cols, if you're iterating through rows total = n_rows
	// perpTotal: if you're iterating through columns perpTotal = n_rows, if you're iterating through rows, perpTotal = n_cols
	// *restrictions: if checking rows, row_restrictions. if checking cols, col_restrictions 
	// *black_runs: if checking rows, row_black_runs. if checking cols, col_black_runs
	// isCol is true if checking cols, is false if checking rows
	void mod_range_to_fit(uint16_t total, uint16_t perpTotal, vector<vector<uint16_t>> *restrictions,
	vector<vector<range>> *black_runs, bool isCol);

	// implementing rule 3.3
	// PARAMETER: apply to row: 1; apply to col: 2
	void rule_11(int8_t row_or_col);



private:
	// private boi, colour black all cells from low to high
	// low: the starting index
	// high: the final index
	// line: your current col/row (whatever you're iterating through)
	// isCol: true if line is a col, false if line is a row
	void push_to_colour(int16_t low, int16_t high, uint16_t line, bool isCol);

	// Private helper method
	// tries to find the closest (increasing or decreasing) wall or white cell between bottomLim and topLim
	// if you find nothing, send -2
	// incr_or_decr indicates whether you're searching from bottomLim to topLim or from topLim to bottomLim
	// perpTotal is the opposite of what you are iterating through
	// line is your current column or row number (whatever it is you're iterating through)
	// isCol is true if line is a column, is false if line is a row
	int16_t find_white_or_wall(int16_t bottomLim, int16_t topLim, int8_t incr_or_decr, uint16_t perpTotal,
	uint16_t line, bool isCol);

	/* PARAMETERS: 
		cell_number: actual cell index of the current cell in the cells vector
		row_or_col: 1 applies to row, 2 to column
		current: current row/column number
		set_of_segments: vector in which all black segments (with starting and ending points) are inserted
	*/
	uint16_t find_and_insert(uint16_t cell_number, int8_t row_or_col, uint16_t current, vector<range> *set_of_segments,
	uint16_t &locate);
};



#endif