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
 * performs 'game of life' logic by calculating special cases
 * (corners, left row, right row, top row and bottom row)
 * and returns the number of active neighbors for the given cell.
 */
static int getNumofNeighbors(Grid<int>& board, int x, int y);

static bool isHotNeighbor(Grid<int>& board, int row, int col);

const int X_FLOOR_VAL = 10;
const int Y_FLOOR_VAL = 10;
const int X_MAX_VAL = 100;
const int Y_MAX_VAL = 100;

int main() {

    int initx = 0;
    int inity = 0;
    LifeDisplay display;
    string fn = "";
    int speed;
    string tryagain = "";

    while (true) {
        //start with a space for readability
        cout << endl;
        welcome(initx, inity, fn, speed);

        cout << "initx = " << initx << endl << "inity = " << inity << endl << "fn = " << fn << endl;

        if (initx > 0) {

            Grid<int> board(initx, inity);
            display.setDimensions(initx, inity);
            display.setTitle("Game of Life");

            //initialize
            for (int curx=0; curx<initx; curx++) {
                //each row
                for (int cury=0; cury<inity; cury++) {
                    //each col
                    //draw cell
                    if (randomChance(0.50)) {
                        display.drawCellAt(curx, cury, 1);
                        board[curx][cury] = 1;
                    };
                };
            };

            runSimulation(display, board, speed);
            cout << endl;


        } else {
            //load from file;
            cout << "load from file fn = " << fn << endl;
        };

        cout << "." << endl;
        cout << "Play again? (press q and [enter] to quit)" << endl;
        cin >> tryagain;

        if (tryagain=="q") {
            break;
        };
    };

    return 0;

}


static bool getAnswerOne() {
    string answer1;
    while (true) {
        cin >> answer1;

        if (answer1 == "y") {
            return true;

        } else if (answer1 == "n") {
            return false;
        };
        cout << "please try again." << endl;
    };

}

static void getDimensions(int & gridx, int & gridy) {

    int xvalue, yvalue;

    // get x first
    cout << "please enter an x value and press enter." << endl;

    while (true) {
        cin >> xvalue;

        if (xvalue>=X_FLOOR_VAL and xvalue<=X_MAX_VAL) {
            gridx = xvalue;
            break;
        };
        cout << "please try again." << endl;
    };

    // get y
    cout << "please enter a y value and press enter." << endl;

    while (true) {
        cin >> yvalue;

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

    cout << "Would you like to use a random board? (enter 'y' or 'n' and then press return)" << endl;

    bool reaction1;

    reaction1 = getAnswerOne();

    if (reaction1) {
        cout << "please enter the dimensions of your random grid using values from 10 to 100." << endl;
        getDimensions(x, y);
        cout << "x will be " << x << " and y will be " << y << endl;
    } else {
        cout << "please specify the name of the file to use." << endl;
        fn = getInputFileName();
    };

    //speedsetting
    cout << "please enter a speed setting: 0 for manual, 1 for slow, 2 for normal, and 3 for fast." << endl;
    speedsetting = getSpeedSetting();
}

static string getInputFileName() {
    string filename;
    cout << "please enter a file name and press enter." << endl;
    cin >> filename;
    return filename;
}

static int getSpeedSetting() {
    int setting;

    while (true) {
        cin >> setting;

        if (setting>=0 && setting<4) {
            return setting;
        };
        cout << "please try again." << endl;
    };
}

static void runSimulation(LifeDisplay& display, Grid<int>& board, int speed) {

    int numRounds = 1;

    while (true) {

        switch (speed)
        {
        case 0:
            waitForEnter("Hit [enter] to continue....   ");
            break;
        case 1:
            //slow
            pause(1500);
            break;
        case 2:
            //medium
            pause(500);
            break;
        };


        GEvent me = getNextEvent();
        //cout << "event class" << me.getEventClass() << endl;
        //cout << "event type" << me.getEventType() << endl;

        if (me.getEventClass()==MOUSE_EVENT)
        {return;}
        else {
            // only advance board if there aren’t any outstanding mouse events
            advanceBoard(display, board);
            cout << "advanced round." << numRounds << endl;
            numRounds++;
        };
    };

}

static void advanceBoard(LifeDisplay& display, Grid<int>& board) {

    int curval = 0;
    Grid<int> nextBoard = board;
    int rows = board.numRows();
    int cols = board.numCols();

    cout << "advancing board..." << endl;
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
            if (isHotNeighbor(board, (x+xx), (y+yy))) {
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
