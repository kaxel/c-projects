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
#include <unistd.h> // for sleep

// prototypes add here
static bool getAnswerOne();
static void getDimensions(int & gridx, int & gridy);
static void waitForEnter(string message);
static void welcome(int & x, int & y, string & fn);
static string getInputFileName();
void runSimulation(LifeDisplay& display, Grid<int>& board);

const int X_FLOOR_VAL = 10;
const int Y_FLOOR_VAL = 10;
const int X_MAX_VAL = 100;
const int Y_MAX_VAL = 100;

int main() {
    int initx = 0;
    int inity = 0;
    LifeDisplay display;
    string fn = "";

    welcome(initx, inity, fn);

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
        runSimulation(display, board);

    } else {
        //load from file;
        cout << "load from file fn = " << fn << endl;
    };

    return 0;
}


void runSimulation(LifeDisplay& display, Grid<int>& board) {
    //int timerun = 99;
    while (true) {
        //I copied this little event-trapping snippet from a student on piazza
        GEvent me = getNextEvent();
        if (me.getEventType() == MOUSE_CLICKED) {
            cout << "end." << endl;
            return;
        }
        else {
            sleep(2);
            cout << "advancing board..." << endl;
            cout << "." << endl;
            //advanceBoard(display, board);
        };

    };
}



static void waitForEnter(string message) {

    cout << message;
    (void) getLine();
}

static void welcome(int & x, int & y, string & fn) {

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


}

void getDimensions(int & gridx, int & gridy) {

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

static string getInputFileName() {
    string filename;
    cout << "please enter a file name and press enter." << endl;
    cin >> filename;
    return filename;
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
    }

}
