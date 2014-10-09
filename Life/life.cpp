/**
 * File: life.cpp
 * --------------
 * Implements the Game of Life.
 */

#include <iostream>  // for cout
using namespace std;
#include "console.h" // required of all files that contain the main function
#include "simpio.h"  // for getLine
#include "life-constants.h"  // for kMaxAge
#include "life-graphics.h"   // for class LifeDisplay
#include "random.h" // for randomChance
#include "gevents.h" // for events
#include <gwindow.h> // for pause
#include <strlib.h> // for string functions
#include <cctype> // for string conversions

/*
 * function: getAnswerOne
 * usage: get first answer for game of life
 * -------------------
 * waiting for a y or n plus enter
 * will return a boolean, true for y, false for n
 */
static bool getAnswerOne();

/*
 * function: getDimensions
 * usage: get grid dimensions from user by passing in gridx and gridy by reference.
 * -------------------
 * ask user for two values from X_FLOOR_VAL to X_MAX_VAL and from Y_FLOOR_VAL to Y_MAX_VAL.
 */
static void getDimensions(int & gridx, int & gridy);

/*
 * function: waitForEnter
 * usage: wait for user to press enter
 */
static void waitForEnter(string message);

/*
 * function: welcome
 * usage: x, y and fn will update by reference
 * -------------------
 * assemble starting input values for game of life
 * starting grid size (x and y) and filename (fn)
 * grid sizes will be 0 if not entered, and fn will be an empty string if not supplied.
 * speedsetting will be a value from 0-3
 */
static void welcome(int & x, int & y, string & fn, int & speedsetting);

/*
 * function: getInputFileName
 * usage: called from welcome to input file name for grid load (if chosen by user)
 * -------------------
 * just takes a file name up to first whitespace character
 * and hands it back as the return value.
 */
static string getInputFileName();

/*
 * function: validFileName
 * usage: validates input
 * -------------------
 * pass in string filename
 * validate alpha numeric and no spaces
 */
static bool validFileName(string filename);

/*
 * function: getSpeedSetting
 * usage: input speed setting
 * -------------------
 * called from welcome
 */
static int getSpeedSetting();

/*
 * function: runSimulation
 * usage: pass in LifeDisplay and board by reference
 * -------------------
 * counts number of rounds
 * traps events, and calls advanceBoard
 */
static void runSimulation(LifeDisplay& display, Grid<int>& board, int speed);

/*
 * function: hasNoLivingCells
 * usage: pass in board and it will look for life
 * -------------------
 * returns true if board has finished
 * (with all cells either = 0 or >= kMaxAge)
 */
static bool hasNoLivingCells(Grid<int>& board);

/*
 * function: advanceBoard
 * usage: pass in LifeDisplay and board by reference
 * -------------------
 * copy board and set up next round in nextBoard
 * resets grid and copies next board to current board and displays it
 */
static void advanceBoard(LifeDisplay& display, Grid<int>& board);

/*
 * function: assembleNextBoard
 * usage: pass in oldboard and board by reference
 * -------------------
 * calls getNumofNeighbors for each cell.
 * calculates new values for board (nextBoard)
 */
static void assembleNextBoard(Grid<int>& oldboard, Grid<int>& newboard);

/*
 * function: getNumofNeighbors
 * usage: pass in board by reference and then x and y coordinates for the cell in question.
 * -------------------
 * performs 'game of life' logic
 * returns the number of active neighbors for the given cell.
 */
static int getNumofNeighbors(Grid<int>& board, int x, int y);

/*
 * function: isHotNeighbor
 * usage: pass in board and coordinates to return true for living cell.
 * -------------------
 * cell value > 0 means true
 */
static bool isHotNeighbor(Grid<int>& board, int row, int col);

/*
 * function: tryRestart
 * usage: prompt user for restart
 * -------------------
 * waiting for a q to quit or y to restart, plus enter
 * will return a boolean, true for y, false for q
 */
static bool tryRestart();

/*
 * function: setUpGridFromFile
 * usage: ingest grid and build display
 * -------------------
 * pass in display, board, and filename as fn
 * file must reside in files/ directory within resouces.
 */
static void setUpGridFromFile(LifeDisplay & display, Grid<int> & board, string fn);

/*
 * function: setUpRandomGrid
 * usage: randomize grid, using x and y coordinates.
 * -------------------
 * pass in display, board, and filename as fn
 * cells are .50 likely to have life
 * LIKELY_TO_HAVE_LIFE is the constant being used so that is adjustable
 */
static void setUpRandomGrid(LifeDisplay & display, Grid<int> & board);

/*
 * function: getDimensionsFromFile
 * usage: pull in x and y from file
 * -------------------
 * pass in stream, holders for x and y, and filename
 * file must reside in files/ directory within resouces.
 */
static void getDimensionsFromFile(ifstream & infile, int & x, int & y, string fullfilename);

/*
 * function: ingestBoardFromFile
 * usage: pass in stream, grid, and filename
 * -------------------
 * pull in values from file and populate board.
 */
static void ingestBoardFromFile(ifstream & infile, Grid<int> & returnboard, string fullfilename);

/*
 * function: loadFromFile
 * usage: returns grid from file name
 * -------------------
 * file must be in local files/ directory
 */
static Grid<int> loadFromFile(string filename);

const int X_FLOOR_VAL = 10;
const int Y_FLOOR_VAL = 10;
const int X_MAX_VAL = 200;
const int Y_MAX_VAL = 200;
const double LIKELY_TO_HAVE_LIFE = 0.50;

int main() {
    bool restart = false;

    while (true) {
        int initx = 0;
        int inity = 0;
        LifeDisplay display;
        string fn = "";
        int speed;

        //start with a space for readability
        cout << endl;
        welcome(initx, inity, fn, speed);

        if (initx > 0) {
            Grid<int> board(initx, inity);
            setUpRandomGrid(display, board);
            runSimulation(display, board, speed);
        } else {
            Grid<int> board;
            setUpGridFromFile(display, board, fn);
            runSimulation(display, board, speed);
        };

        restart = tryRestart();
        if (!restart) break;
    };
    return 0;
}

static bool getAnswerOne() {
    string answer1;
    while (true) {
        answer1 = getLine("Would you like to use a random board? (enter 'y' for a random board or 'n' to load from a file and then press [enter]): \n");

        if (answer1 == "y") {
            return true;

        } else if (answer1 == "n") {
            return false;
        };
        cout << "please try again." << endl;
    };

}

static void getDimensions(int & gridx, int & gridy) {

    int xvalue = 0, yvalue = 0;

    // get x first
    while (true) {
        xvalue = getInteger("please enter an x value and press [enter]. \n");

        if (xvalue>=X_FLOOR_VAL and xvalue<=X_MAX_VAL) {
            gridx = xvalue;
            break;
        };
        cout << "please try again." << endl;
    };

    // get y
    while (true) {
        yvalue = getInteger("please enter a y value and press [enter]. \n");

        if (yvalue>=Y_FLOOR_VAL and yvalue<=Y_MAX_VAL) {
            gridy = yvalue;
            break;
        };
        cout << "please try again." << endl;
    };

}

static void waitForEnter(string message) {

    cout << message;
    (void) getLine();
}

static void welcome(int & x, int & y, string & fn, int & speedsetting) {

    cout << "Welcome to the game of Life, a simulation of the lifecycle of a bacteria colony." << endl;
    cout << "Cells live and die by the following rules:" << endl << endl;
    cout << "\tA cell with 1 or fewer neighbors dies of loneliness" << endl;
    cout << "\tLocations with 2 neighbors remain stable" << endl;
    cout << "\tLocations with 3 neighbors will spontaneously create life" << endl;
    cout << "\tLocations with 4 or more neighbors die of overcrowding" << endl << endl;
    cout << "In the animation, new cells are dark and fade to gray as they age." << endl << endl;
    waitForEnter("Hit [enter] to continue....   ");

    bool reaction1;

    reaction1 = getAnswerOne();

    if (reaction1) {
        getDimensions(x, y);
    } else {
        fn = getInputFileName();
    };

    //speedsetting
    speedsetting = getSpeedSetting();
}

static string getInputFileName() {

    string filename;

    while (true) {
        filename = getLine("please enter a file name and press [enter]: \n");

        if (validFileName(filename)) {
            return filename;
        };
        cout << "please try again." << endl;
    };

}

static bool validFileName(string filename) {
    bool retval = true;
    int len = 0;

    len = filename.length();
    //test each char
    for (int i = 0; i < len; i++) {
        if (!isalnum(filename[i]) && !isspace(filename[i])) {
            retval = false;
        };
    };
    return retval;
}

static int getSpeedSetting() {
    int setting;

    while (true) {
        setting = getInteger(("please enter a speed setting: 0 for manual, and 1-10 where 10 is fastest. \n"));

        if (setting>=0 && setting<11) {
            return setting;
        };
        cout << "please try again." << endl;
    };
}

static void runSimulation(LifeDisplay& display, Grid<int>& board, int speed) {
    int numRounds = 1;
    bool isManual = false;

    if (speed==0) isManual = true
            ;

    while (true) {

        GEvent me = getNextEvent(MOUSE_EVENT);
        if (me.getEventType() == MOUSE_CLICKED)
        {
            cout << "ended by mouse click." << endl;
            return;
        }
        else if (me.getEventType() == NULL_EVENT) {

            if (isManual) {
                waitForEnter("Press [enter] to continue...");
            } else {
                //move this around to affect what the range of speed 1-10 covers
                int coefficient = 200;
                pause((coefficient*10)-(speed*coefficient));
            };
            // only advance board if there aren’t any outstanding mouse events
            advanceBoard(display, board);
            if (hasNoLivingCells(board)) return;
            cout << "advanced round." << numRounds << endl;
            numRounds++;
        };
    };

}

static bool hasNoLivingCells(Grid<int>& board) {
    //return true if every cell is either dead or max age
    for (int curx=0; curx<board.numRows(); curx++) {
        //each row
        for (int cury=0; cury<board.numCols(); cury++) {
            if (board[curx][cury] > 0){
                if (board[curx][cury] < kMaxAge) return false;
            };
        };
    };
    cout << "board is finished." << endl;
    return true;

}

static void advanceBoard(LifeDisplay& display, Grid<int>& board) {

    int curval = 0;
    Grid<int> nextBoard = board;
    int rows = board.numRows();
    int cols = board.numCols();

    cout << endl;

    //assemble next board
    assembleNextBoard(board, nextBoard);

    //draw display from board
    board = nextBoard;

    //clear previous
    display.setDimensions(rows, cols);

    for (int curx=0; curx<rows; curx++) {
        //each row
        for (int cury=0; cury<cols; cury++) {
            //each col
            curval = board[curx][cury];
            if (curval>0) {
                display.drawCellAt(curx, cury, curval);
            };

        };
    };

}

static void assembleNextBoard(Grid<int>& oldboard, Grid<int>& newboard) {
    int rows = oldboard.numRows();
    int cols = oldboard.numCols();
    int oldval = 0;
    int newval = 0;
    int numofneighbors = 0;

    for (int curx=0; curx<rows; curx++) {
        //each row
        for (int cury=0; cury<cols; cury++) {

            oldval = oldboard[curx][cury];

            numofneighbors = getNumofNeighbors(oldboard, curx, cury);

            switch (numofneighbors)
            {
            case 1:
                //die (lonliness)
                newval = 0;
                break;
            case 2:
                //remains stable
                if (oldval > 0) {
                    newval = oldval + 1;
                } else {
                    newval=0;
                };
                break;
            case 3:
                //create life
                newval = oldval + 1;
                break;
            case 4:
                //die (overcrowding)
                newval = 0;
                break;
            default:
                newval = 0;
                break;
            };

            newboard[curx][cury] = newval;

        };
    };
}

static int getNumofNeighbors(Grid<int>& board, int x, int y) {

    int count = 0;

    //count up neighbors
    for (int xx=-1; xx<2; xx++) {
        for (int yy=-1; yy<2; yy++) {
            if (isHotNeighbor(board, (x+xx), (y+yy)) && !(xx==0 && yy==0)) {
            count +=  1;
            };
        };
    };
    return count;
}

static bool isHotNeighbor(Grid<int>& board, int row, int col) {
    int maxrows = board.numRows()-1;
    int maxcols = board.numCols()-1;
    bool answer;

    if (row < 0 || col < 0 || row > maxrows || col > maxcols) {
        return false; // falls off the grid
    }
    else {
        answer = board[row][col]>0 ? 1 : 0;
        return answer;
    };

}

static bool tryRestart() {
    string tryagain = "";
    while (true) {
        tryagain = getLine("Play again? (press q and [enter] to quit or press y and [enter] to restart): \n");
        if (tryagain=="q") {
            cout << "Thank you and goodbye!" << endl;
            return false;
            break;
        } else if (tryagain=="y") {
            return true;
            break;
        } else {
            cout << "try again please." << endl;
        };
    };
}

static void setUpGridFromFile(LifeDisplay & display, Grid<int> & board, string fn) {
    board = loadFromFile(fn);
    display.setDimensions(board.numRows(), board.numCols());
    display.setTitle("Game of Life");

    //initialize
    for (int curx=0; curx<board.numRows(); curx++) {
        //each row
        for (int cury=0; cury<board.numCols(); cury++) {
            if (board[curx][cury] == 1){
                display.drawCellAt(curx, cury, 1);
            };
        };
    };
}

static void setUpRandomGrid(LifeDisplay & display, Grid<int> & board) {

    display.setDimensions(board.numRows(), board.numCols());
    display.setTitle("Game of Life");

    //initialize
    for (int curx=0; curx<board.numRows(); curx++) {
        //each row
        for (int cury=0; cury<board.numCols(); cury++) {
            //each col
            //draw cell
            if (randomChance(LIKELY_TO_HAVE_LIFE)) {
                display.drawCellAt(curx, cury, 1);
                board[curx][cury] = 1;
            };
        };
    };
}

static void getDimensionsFromFile(ifstream & infile, int & x, int & y, string fullfilename) {
    int countofleadint = 0;
    string thisline;

    infile.open(fullfilename.c_str());
    //get dimensions
    while (infile) {
        getline(infile, thisline);

        if (isdigit(thisline[0])) {
            if (countofleadint==0) {
                x = stringToInteger(thisline);
                countofleadint+=1;
            } else {
                y = stringToInteger(thisline);
            };
        };
    };
    infile.close();
}

static void ingestBoardFromFile(ifstream & infile, Grid<int> & returnboard, string fullfilename) {
    string thisline;
    string compare;
    int curi = 0;

    infile.open(fullfilename.c_str());
    //read lines for grid contruction
    while (infile) {
        getline(infile, thisline);
        compare = thisline[0];
        if (toUpperCase(compare) == "X" || compare=="-") {
            //process valid row of cell values
            for (int cury=0; cury<returnboard.numCols(); cury++) {
                compare = thisline[cury];
                if (toUpperCase(compare) == "X") {
                    if (curi < returnboard.numRows()) {
                        returnboard[curi][cury] = 1;
                    };
                } else {
                    if (curi < returnboard.numRows()) {
                        returnboard[curi][cury] = 0;
                    };
                };
            };
            curi+=1;
        };
    };
    infile.close();
}

static Grid<int> loadFromFile(string filename) {
    ifstream infile;
    string fullfilename = "files/";
    int gridx = 0;
    int gridy = 0;

    fullfilename +=  filename;
    getDimensionsFromFile(infile, gridx, gridy, fullfilename);
    Grid<int> returnboard(gridx, gridy);
    ingestBoardFromFile(infile, returnboard, fullfilename);
    return returnboard;
}
