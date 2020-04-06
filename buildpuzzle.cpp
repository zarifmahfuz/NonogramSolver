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
			//cout << "index: " << index << endl;
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
			//cout << "NOPE ";
			return -2;
		}
	}
	// going down
	else {
		// cout << "bottom? " << bottomLim << " top? " <<topLim <<endl;
		// cout <<endl; 
		// try to find a white
		while (bottomLim <=topLim && topLim >= 0) {
			uint16_t index = n_cols*line+topLim;
			if (isCol) {
				index = n_cols*topLim+line;
			}

			//cout << "index: " << index << endl;
			// if you find a white, return the cell
			if (cells[index] == 1) {
				return topLim;
			}
			//cout << "top: " << topLim << ", ";
			topLim--;

		}
		// still nothing? check if there's a wall
		if (bottomLim <= -1) {
			return -1;
		}
		// found nothing
		else{
			//cout << "not in range ";
			return -2;
		}
	}
}

// private boi, colour from low to high 
void Puzzle::push_to_colour(int16_t low, int16_t high, uint16_t line, bool isCol) {
	//cout << "col: " << line << endl;
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
			//cout << colind << endl;
			solved_indicator++;
		}
	}
}

void Puzzle::fill_in_void(uint16_t total, uint16_t perpTotal, vector<vector<uint16_t>> *restrictions, vector<vector<range>> *black_runs, bool isCol) {
	// iterate thru each line
	for (uint16_t line = 0; line < total; line++) {

		uint16_t runs_per_line = (*restrictions)[line].size();

		if (runs_per_line >= 3) {
			// iterate thru all runs in the line
			// skip the first and last runs because those won't have runs prior or after themselves
			for (uint16_t runs = 1; runs <runs_per_line-1; runs++) {

				// assume that there is some black
				bool noBlackBox = false;

				// get index of the startpoint
				uint16_t startLine = (*black_runs)[line][runs-1].second;
				uint16_t start = n_cols*line+ startLine;
				if (isCol) {
					start = n_cols*startLine+ line;
				}
				// iterate thru cells in the line until finding black box
				while (cells[start] != 0 && startLine < perpTotal) {
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
					break;
				}

				// get index of endpoint
				uint16_t endLine = (*black_runs)[line][runs+1].first;
				int16_t end = n_cols*line+ endLine;
				if (isCol) {
					end = n_cols*endLine+ line;
				}

				// iterate thru cells in the line backwards until finding black box
				while (cells[end] != 0 && endLine >= 0) {
					endLine--;

					if (isCol) {
						end = n_cols*endLine+ line;
					}
					else {
						end = n_cols*line+ endLine;
					}

					// if you found a black box its position is saved in endLine
				}
				// if you found nothing, or the black box is next to the wall endLine == -1

				// colour everything between startpoint and endpoint
				push_to_colour(startLine, endLine, line, isCol);

				//change the range of the current run that you just filled in
				(*black_runs)[line][runs].first = endLine + 1 - (*restrictions)[line][runs];
				(*black_runs)[line][runs].second = (*restrictions)[line][runs] + startLine -1;
			}
		}
		// if there isn't a minimum of 3 runs in the line, rule won't apply
	}
}


// Rule 1.5
// expands black runs near whites or near walls
// limits black runs to prevent them from exceeding a size they cannot have (otherwise it wouldn't fit)
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
				    while (cells[cellInd] ==0 && start >= -1) {
				    	tally++;
				    	//cout << "ran" << " ";
				    	// move on
				    	start--;
				    	if (isCol) {
							cellInd = n_cols*start +line;
						}
						else {
							cellInd = n_cols*line+start;
						}
				    }

				    if (isCol) {
						cellInd = n_cols*celli +line;
					}
					else {
						cellInd = n_cols*line+celli;
					}
				    // go after until you reach an unknown or a white
				    while (cells[cellInd] == 0 && end <= perpTotal) {
				    	tally++;
				    	//cout << "run" << " ";
				    	// move on
				    	end++;
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

				    //cout << cellInd <<endl;
				    //cout << "tally :" << tally <<endl;

				    // if the tally is the length, we can colour some whites
				    if (tally == minlen) {
				    	// whiten the start -1
				    	if (isCol) {
				    		// cellInd currently contains the end cell
				    		
				    		if (cells[cellInd]== -1 && !atRightEnd) {
								// colour it
								cells[cellInd] = 1;
								solved_indicator++;
								//cout << cellInd <<endl;
							}
						

							// possibly whiten the start
							cellInd = n_cols*start +line;
						
							if (cells[cellInd]== -1 && !atLeftEnd) {
								// colour it
								cells[cellInd] = 1;
								solved_indicator++;
								//cout << cellInd <<endl;
							}
							
						}
						else {
							// cellInd currently contains the end cell
							if (cells[cellInd]== -1 && !atRightEnd) {
								// colour it
								cells[cellInd] = 1;
								solved_indicator++;
								//cout << cellInd <<endl;
							}

							// possibly whiten the start 
							cellInd = n_cols*line+start;
							if (cells[cellInd]== -1 && !atLeftEnd) {
								// colour it
								cells[cellInd] = 1;
								solved_indicator++;
								//cout << cellInd <<endl;
							}
							
						}
				    }
				}

				//cout << endl;
				// expand before?
				// check to see if there is a white or a wall before the black
				//cout << "celli: " << celli << "|| minlen: " << minlen << endl;
				int16_t before = find_white_or_wall(celli-minlen+1, celli-1, -1, perpTotal, line, isCol);

				//cout << "before: " << before << endl;

				// expand after?
				// check to see if there is a white or a wall after the black
				int16_t after = find_white_or_wall(celli+1, celli+minlen-1, 1, perpTotal, line, isCol);

				// cout << "after: " << after << endl;
				// cout << endl;

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

// Rule 3.1

