/**
 * File: dominosa.cpp
 * ------------------
 * This animates the brute-force discovery of a solution
 * to a 2 x n dominosa board.
 */

#include <iostream>
#include "point.h"
using namespace std;

#include "simpio.h"
#include "grid.h"
#include "random.h"
#include "set.h"
#include "vector.h"
#include "dominosa-graphics.h"

static void welcome() {
	cout << "Here we'll illustrate the use of recursive backtracking to" << endl;
	cout << "discover a solution to various 2 x n Dominosa boards.  In some" << endl;
	cout << "cases there won't be any solutions, and in the cases where there are" << endl;
	cout << "multiple solutions, we'll just find one of them." << endl;
	cout << endl;
}

static int getIntegerInRange(string prompt, int low, int high) {
	while (true) {
		int response = getInteger(prompt);
		if (response == 0 || (response >= low && response <= high)) return response;
		cout << "Sorry, but I can't use that number." << endl;
	}
}

static void populateBoard(Grid<int>& board, int low, int high) {
	for (int row = 0; row < board.numRows(); row++) {
		for (int col = 0; col < board.numCols(); col++) {
			board[row][col] = randomInteger(low, high);
		}
	}
}

static void displayPiece(DominosaDisplay& display, Vector<coord> nextpiece) {
    display.provisonallyPair(nextpiece[0], nextpiece[1]);
}

static void erasePiece(DominosaDisplay& display, Vector<coord>& nextpiece) {
    display.eraseProvisionalPairing(nextpiece[0], nextpiece[1]);
}

static coord makePair(int a, int b) {
    coord setpair;

    if (a>b) {
        // put bigval first, 5, 4 and 4, 5 will be in the same place
        setpair.row=a;
        setpair.col=b;
    } else {
        setpair.row=b;
        setpair.col=a;
    }
    return setpair;
}

static bool preparePiece(Grid<int> board, Vector<coord>& nextpiece, coord& curcoord, Set<coord>& picks, bool& horizontal) {
    int placea, placeb;
    coord placement, setcoord;

    nextpiece.add(curcoord);
    if (!horizontal) {
        placement.row=curcoord.row+1;
        placement.col=curcoord.col;
    } else {
        if (curcoord.col==board.numCols()-1) return false;
        placement.row=curcoord.row;
        placement.col=curcoord.col+1;
    }
    nextpiece.add(placement);

    //used to track pair ubiquity
    placea = board[nextpiece[0].row][nextpiece[0].col];
    placeb = board[nextpiece[1].row][nextpiece[1].col];

    setcoord = makePair(placea, placeb);

    if (nextpiece[1].col==board.numCols()) {
        // col falls off baoard
        return false;
    } else {
        if (picks.contains(setcoord)) {
            // pair already in use
            return false;
        } else picks.add(setcoord);
    }
    return true;
}

static bool canSolveBoardh(DominosaDisplay& display, Grid<int>& board, int round, Set<coord> picks, coord curcoord, bool horizontal, Stack <Vector<coord>>& breadcrumbs) {
    //get max num of pieces
    int boardsize = board.numCols();
    Vector<coord> nextpiece;

    if (round>boardsize) return true;
    if (preparePiece(board, nextpiece, curcoord, picks, horizontal)) {
        breadcrumbs.push(nextpiece);
        displayPiece(display, nextpiece);
    } else return false;

    if (!horizontal) {
        //either vertical or horizontal
        coord nextpick = curcoord;
        nextpick.col++;
        round++;
        if (canSolveBoardh(display, board, round, picks, nextpick, false, breadcrumbs)) return true;
        if (canSolveBoardh(display, board, round, picks, nextpick, true, breadcrumbs)) return true;

    } else {
        //horizontal
        coord nextpick = curcoord;
        if (curcoord.row==0) {
            nextpick.row=1;
            nextpick.col=curcoord.col;
            round++;
            if (canSolveBoardh(display, board, round, picks, nextpick, true, breadcrumbs)) return true;

        } else {
            nextpick.row=0;
            nextpick.col+=2;
            round++;
            if (canSolveBoardh(display, board, round, picks, nextpick, true, breadcrumbs)) return true;
            if (canSolveBoardh(display, board, round, picks, nextpick, false, breadcrumbs)) return true;
        }

    }
    breadcrumbs.pop();
    erasePiece(display, nextpiece);
    return false;
}


static bool canSolveBoard(DominosaDisplay& display, Grid<int>& board, Stack <Vector<coord>>& breadcrumbs) {
    int round = 1;
    Set<coord> picks;
    coord curcoord;

    curcoord.row=0;
    curcoord.col=0;
    return canSolveBoardh(display, board, round, picks, curcoord, true, breadcrumbs) || canSolveBoardh(display, board, round, picks, curcoord, false, breadcrumbs);
}

static void showCrumbs(DominosaDisplay& display, Stack <Vector<coord>>& breadcrumbs) {
    while (true) {
        if (breadcrumbs.isEmpty()) break;
        Vector<coord> v = breadcrumbs.pop();
        display.certifyPairing(v[0], v[1]);
    }
}

int main() {
	DominosaDisplay display;
    Stack <Vector<coord>> breadcrumbs;

	welcome();
	while (true) {
        int numColumns = getIntegerInRange("How many columns? [0 to exit]: ", 9, 25);
		if (numColumns == 0) break;
		Grid<int> board(2, numColumns);
		populateBoard(board, 1, ceil(2 * sqrt((double) numColumns)));
		display.drawBoard(board);

        if (canSolveBoard(display, board, breadcrumbs)) {
			cout << "The board can be solved, and one such solution is drawn above." << endl;
            showCrumbs(display, breadcrumbs);
		} else {
			cout << "This board you see can't be solved." << endl;
		}
	}
	
	cout << "Okay, thanks for watching, and come back soon." << endl;
    cout << "Click the mouse anywhere in the window to exit." << endl;
	return 0;
}
