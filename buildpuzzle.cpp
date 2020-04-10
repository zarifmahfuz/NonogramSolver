#include <iostream>

#include <algorithm>
#include <math.h>

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
				//cout << "indicator: " << solved_indicator << "| made " << converted_ind << " black in rule 1.1" << endl;
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
	// cout << "line: " << line << endl;
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
			// cout << colind << endl;
			solved_indicator++;
			//cout << "indicator: " << solved_indicator << "| made " << colind << " black in push_to_colour" << endl;
		}
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
				//cout << "col: " << line << "| row: " << celli << "| cell:" << cellInd  << endl;
			}
			else {
				cellInd = n_cols*line+celli;
				//cout << "row: " << line << "| col: " << celli<< "| cell:" << cellInd  << endl;
			}

			
			if (cells[cellInd] == 0) {
				// find the smallest run that could be in celli
				uint16_t runs_per_line = (*black_runs)[line].size();
				vector <uint16_t> runs_in_range;

				//cout << "is black" << endl;
				for (uint16_t run = 0; run <runs_per_line; run++){

					range my_range = (*black_runs)[line][run];
					//cout << "runnum: " << run << endl;

					//cout << "top limit: " << my_range.first << "bottom limit: " << my_range.second << endl;
					// if the run's range includes the cell
					if (celli >= my_range.first && celli <= my_range.second) {
						//cout << "included" <<endl;
						// push the length of the run
						runs_in_range.push_back((*restrictions)[line][run]);
					}
				}
				// find minimum length 
				uint16_t minlen = *min_element(runs_in_range.begin(), runs_in_range.end());
				//cout << "checkpoint1 ," << endl;

				if (adjacent_find(runs_in_range.begin(), runs_in_range.end(), not_equal_to<>() ) == runs_in_range.end()) {
				    // elements in vec are all the same
				    // cout << "are same" << line << endl;


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
				    	//cout << start <<endl;
				    	start--;
				    	if (isCol) {
							cellInd = n_cols*start +line;
						}
						else {
							cellInd = n_cols*line+start;
						}
						//cout << "new cellind: " << cellInd <<endl;
				    }

				    //cout << "checkpoint2 ,";

				    if (isCol) {
						cellInd = n_cols*celli +line;
					}
					else {
						cellInd = n_cols*line+celli;
					}

				    // go after until you reach an unknown or a white
				    while (end < perpTotal && cells[cellInd] == 0) {
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

				   	// cout << "checkpoint3 ," << endl;

				    // cout << "end: " << end << endl;
				    // check if you touched the wall
				    if(end == perpTotal){
				    	//cout << "touched Bwall" << endl;
				    	atRightEnd = true;
				    }
				    if (start == -1) {
				    	//cout << "touched Twall" << endl;
				    	atLeftEnd = true;
				    }

				    //cout << cellInd <<endl;
				    //cout << "tally :" << line <<endl;

				    // if the tally is the length, we can colour some whites
				    if (tally == minlen) {
				    	// whiten the start -1
				    	if (isCol) {
				    		// cellInd currently contains the end cell
				    		
				    		if (!atRightEnd && cells[cellInd]== -1) {
								// colour it
								cells[cellInd] = 1;
								solved_indicator++;
								//cout << "indicator: " << solved_indicator << "| made " << cellInd << " black in rule 1.5" << endl;
								//cout << cellInd <<endl;
								//cout << "coloured the bottom" << endl;
							}


							// possibly whiten the start
							cellInd = n_cols*start +line;
						
							if (!atLeftEnd && cells[cellInd]== -1) {
								// colour it
								cells[cellInd] = 1;
								solved_indicator++;
								//cout << "indicator: " << solved_indicator << "| made " << cellInd << " black in rule 1.5" << endl;
								//cout << "coloured the top" << endl;
								//cout << cellInd <<endl;
							}
							
						}
						else {
							// cellInd currently contains the end cell
							if (!atRightEnd && cells[cellInd]== -1) {
								// colour it
								cells[cellInd] = 1;
								solved_indicator++;
								//cout << "indicator: " << solved_indicator << "| made " << cellInd << " black in rule 1.5" << endl;
								//cout << cellInd <<endl;
							}

							// possibly whiten the start 
							cellInd = n_cols*line+start;
							if (!atLeftEnd && cells[cellInd]== -1) {
								// colour it
								cells[cellInd] = 1;
								solved_indicator++;
								//cout << "indicator: " << solved_indicator << "| made " << cellInd << " black in rule 1.5" << endl;
								//cout << cellInd <<endl;
							}
						}

						//cout << "checkpoint4 ," << endl;
				    }
				}

				//cout << endl;

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
			//cout << "made it" <<endl;

			// if there are no black cells, rule does not apply
		}
	}
}

// Rule 3.1
// colours between black cells that are guaranteed to be an item
void Puzzle::fill_in_void(uint16_t total, uint16_t perpTotal, vector<vector<uint16_t>> *restrictions, vector<vector<range>> *black_runs, bool isCol) {
	// iterate thru each line
	for (uint16_t line = 0; line < total; line++) {

		uint16_t runs_per_line = (*restrictions)[line].size();

		//if (runs_per_line >= 3) {
			// iterate thru all runs in the line
			// skip the first and last runs because those won't have runs prior or after themselves
			for (uint16_t runs = 0; runs <runs_per_line; runs++) {

				//cout << "line: " << line << "| run: " << runs << "| my range: " << (*black_runs)[line][runs].first << "-" << (*black_runs)[line][runs].second<< endl;
				// assume that there is some black
				bool noBlackBox = false;

				// get index of the startpoint
				uint16_t startLine;
				// if it's the first run in the line
				if (runs == 0){
					startLine = 0;
					//cout << "start loop 0" << endl;
				}
				// if it's any other run
				else {
					startLine = (*black_runs)[line][runs-1].second + 1; // +1 if you wanna exclude the black run AT r(j-1)e
					//cout << "start loop at " << start << endl;
				}
				uint16_t start = n_cols*line+ startLine;

				if (isCol) {
					start = n_cols*startLine+ line;
				}
				//cout << "start loop at " << start << endl;

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
					//cout << "no black in this line yet" << endl;
					cout << endl;
					break;
				}

				//cout << "start black at: " << start << endl;
				//cout << endl;

				// get index of endpoint
				int16_t endLine;
				// if it's the last run in the line
				if (runs == runs_per_line -1){
					// start from the highest place
					endLine = perpTotal-1;
					//cout << "end loop at " << endLine << endl;
				}
				// if it's any other run
				else {
					endLine = (*black_runs)[line][runs+1].first -1; // -1 to avoid counting yourself
					//cout << "end loop at " << end << endl;
				}

				uint16_t end = n_cols*line+ endLine;
				if (isCol) {
					//cout << "the col: " << line <<  " the run: " << runs-1 <<  "|| the row: " << endLine << endl;
					end = n_cols*endLine+ line;
				}
				//cout << "end loop at " << end << endl;

				// iterate thru cells in the line backwards until finding black box
				while (endLine >= 0 && cells[end] != 0) {
					endLine--;

					if (isCol) {
						end = n_cols*endLine+ line;
					}
					else {
						end = n_cols*line+ endLine;
					}
					//cout << end << " " << endLine << endl;
					// you've reached a wall and still found no black
					if (endLine <= -1 ) {
						noBlackBox = true;
					}

					// if you found a black box its position is saved in endLine
				}

				// if you found no black boxes in the line, go to the next line.
				if (noBlackBox) {
					//cout << "no black in this line yet" << endl;
					cout << endl;
					break;
				}

				//cout << "end black at: " << end << endl << endl;
				// cout << endl;

				// if you found nothing, or the black box is next to the wall endLine == -1
				// colour everything between startpoint and endpoint
				push_to_colour(startLine, endLine, line, isCol);

				if (endLine >= startLine) {
					//change the range of the current run that you just filled in
					(*black_runs)[line][runs].first = endLine + 1 - (*restrictions)[line][runs];
					(*black_runs)[line][runs].second = (*restrictions)[line][runs] + startLine -1;
				}
			}
		//}
		// if there isn't a minimum of 3 runs in the line, rule won't apply
	}
}

// Rule 3.2
// modifies the range of a run so that it is only hypothesized to fit somewhere where it actually CAN fit
void Puzzle::mod_range_to_fit(uint16_t total, uint16_t perpTotal, vector<vector<uint16_t>> *restrictions,
	vector<vector<range>> *black_runs, bool isCol)
{

	 // iterate thru each line
	for (uint16_t line = 0; line < total; line++)
	{

		uint16_t runs_per_line = (*restrictions)[line].size();
		// iterate thru all runs in the line
		for (uint16_t runs = 0; runs <runs_per_line; runs++)
		{
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

			//cout << "starting at: " << low_range_cell << " ending at: " << high_range_cell<<endl;

			// trim until you find a cell that is not white
			while (cells[low_range_cell] == 1 && low_range <= high_range) {
				//cout << "whit cell at: " << low_range_cell << endl;
				low_range++;

				// make sure you're checking the correct cell next iteration
				if (isCol) {
					low_range_cell = n_cols*low_range + line;
				}
				else {
					low_range_cell = n_cols*line+low_range;
				}
			}

			// cout << "for run: " << (*restrictions)[line][runs] << " in line" << line <<endl;

			// the case where all cells in the range are white, which doesn't really make sense
			// probably will never happen?
			if (cells[low_range_cell] == 1) {
				// get out, this rule doesn't apply to this range
				cout << "everything is white" << endl << endl;
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

			//cout << " || first nonwhite: " << low_range_cell <<endl <<endl;
			

			// stores the number of segments enclosed by whites
			uint16_t white_count = 0;
			// stores the indices of the segment
			uint16_t seg_start = low_range_cell;


			bool wasWhite = false;

			// cout << "range: " << low_range_cell << " - " << high_range_cell <<endl <<endl;
			// iterate from lower cell to higher cell to find the number of segments
			for (uint16_t rising = low_range_cell; rising <= high_range_cell; rising++, low_range++)
			{

				// cout << "went in?" << rising <<endl;
				// adjust to appropiate cell
				if (isCol) {
					rising = n_cols*low_range + line;
				}

				// if you find a white cell, count it
				if (cells[rising] == 1) {
					if (!wasWhite) {
						uint16_t uplim;
						if (isCol) {
							uplim = rising- n_cols;
						}
						else{
							uplim = rising -1;
						}

						white_segs.push_back(range(seg_start, uplim));
						// cout << "pushed: " << seg_start << ", " << uplim << endl; 
						//cout << endl;
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
					// cout << "added last" <<endl;
					white_segs.push_back(range(seg_start, rising));
					white_count++;
					// cout << "pushed: " << seg_start << ", " << rising << endl;
				}

			}


			// cout << " there are #" << white_segs.size() << "," << white_count<< endl << endl;

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

			
			//(*black_runs)[line][runs].first = white_segs[seg_num].first;

			// change the range of this run accordingly
			if (isCol) {
				(*black_runs)[line][runs].first= floor ((white_segs[seg_num].first- line)/n_cols); 
			}
			else {
				// get column ind
				(*black_runs)[line][runs].first=  (white_segs[seg_num].first- line*n_cols); 
			}

			//cout << "rjs: " << (*black_runs)[line][runs].first << endl;
			// save it for later
			uint16_t init_seg = seg_num;


			// STEP 3, now start from the top 
			seg_num = white_count -1;

			// get the index
			if (isCol) {
				seg_len = white_segs[seg_num].second - white_segs[seg_num].first +n_cols;
				seg_len= floor ((seg_len- line)/n_cols) + 1;
				// cout << "len: " << seg_len <<endl;
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

			//(*black_runs)[line][runs].second = white_segs[seg_num].second;

			// change the range of this run accordingly
			if (isCol) {
				(*black_runs)[line][runs].second= floor ((white_segs[seg_num].second- line)/n_cols); 
			}
			else {
				// get column ind
				(*black_runs)[line][runs].second=  (white_segs[seg_num].second- line*n_cols); 
			}

			//cout << "rje: " << (*black_runs)[line][runs].second <<endl << endl;

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
								// cout << "coloured" << colind << endl;
								solved_indicator++;
								//cout << "indicator: " << solved_indicator << "| made " << colind << " white in rule 3.2" << endl;
							}
						}
					}
				}
			}
		}
	}
}