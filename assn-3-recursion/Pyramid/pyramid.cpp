/**
 * File: draw-boxy-fractal.cpp
 * ---------------------------
 * drawBoxyFractal draws a collection of recursively layered squares
 * to form a fractal in two-dimensional space.
 */

using namespace std;

#include "console.h"
#include "vector.h"
#include "random.h"
#include "simpio.h"
#include <iomanip>

static int weightOnKnees(int row, int col, Vector<Vector<int>>& weights);
static void assembleWeights(Vector<Vector<int>>& weights, const int & ncols);
static void printWeights(Vector<Vector<int>>& weights);
static void printWeightOnKnees(Vector<Vector<int>>& weights);

int main() {

    while (true) {
        Vector<Vector<int> > weights;
        int ncols = getInteger("How many people are on the bottom row? ");
        cout << endl;

        assembleWeights(weights, ncols);
        printWeights(weights);
        printWeightOnKnees(weights);

        string response = getLine("Make another pyramid? [y/n] ");
        if (response != "y")
            break;
        cout << endl;
    }

    return 0;
}

static int weightOnKnees(int row, int col, Vector<Vector<int>>& weights) {
    if (row==weights.size()) return 0;
    if (col>=weights.size()-row) return 0;
    if (col<0) return 0;

    return weights[row][col] + weightOnKnees(row+1, col, weights)/2 + weightOnKnees(row+1, col-1, weights)/2;
}

static void assembleWeights(Vector<Vector<int>>& weights, const int & ncols) {

    for (int row=0; row<ncols; row++) {
        Vector<int> linevector;
        //rows are ncols size
        for (int col=0; col<(ncols-row); col++) {
            linevector.add(randomInteger(50, 150));
        }
        weights.add(linevector);
    }
}

static void printWeights(Vector<Vector<int>>& weights) {

    for (int row=weights.size()-1; row>-1; row--) {
        for (int col=0; col<=(weights.size()-row-1); col++) {
            cout << setw(8) << weights[row][col];
        }
        cout << endl;
    }
    cout << endl;
    cout << setw(weights.size()*4) << "---" << endl;
}

static void printWeightOnKnees(Vector<Vector<int>>& weights) {

    for (int row=weights.size()-1; row>-1; row--) {
        for (int col=0; col<=(weights.size()-row-1); col++) {
            cout << setw(8) << weightOnKnees(row, col, weights);
        }
        cout << endl;
    }
    cout << "." << endl;
}

