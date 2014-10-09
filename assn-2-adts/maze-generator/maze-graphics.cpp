/**
 * File: maze-graphics.cpp
 * -----------------------
 * Provides the implementation of the (very primitive)
 * graphics routines needed to help animate the construction
 * of a maze.
 */

#include "maze-graphics.h"

#include <string>
using namespace std;

/**
 * Global constants that help provide more
 * meaningful names for what will otherwise be
 * magic values.
 */
static const string kWindowTitle("Maze Generator");
static const double kWindowWidth = 14 * 72; // 14 inches at 72 pixel per inch
static const double kWindowHeight = 8 * 72; // 8 inches at 72 pixels per inch
static const string kMazeVisibleColor("Blue");
static const string kMazeInvisibleColor("White");
static const double kMazeSize = 7 * 72; // 7 inches at 72 pixels per inch

MazeGeneratorView::MazeGeneratorView() : GWindow(kWindowWidth, kWindowHeight) {
    ulx = (getWidth() - kMazeSize)/2;
    uly = (getHeight() - kMazeSize)/2;
    setWindowTitle(kWindowTitle);
    setVisible(true);
}

MazeGeneratorView::~MazeGeneratorView() {
    close();
}

void MazeGeneratorView::setDimension(int dimension) {
    if (dimension <= 0)
        error("Value passed to MazeGeneratorView::setDimension should be positive.");

    clear();
    this->dimension = dimension;
    verticalWalls.resize(dimension, dimension - 1);
    horizontalWalls.resize(dimension - 1, dimension);
    for(int i = 0; i < dimension; i++) {
        for(int j = 0; j < dimension - 1; j++) {
            verticalWalls[i][j] = false;
            horizontalWalls[j][i] = false;
        }
    }
    wallLength = kMazeSize / dimension;
}

void MazeGeneratorView::drawWall(const wall& w, const string& color, double inset) {
    double startx = ulx + w.one.col * this->wallLength;
    double starty = uly + w.one.row * this->wallLength;

    double wallLength = this->wallLength;
    if (color == kMazeInvisibleColor) {
        wallLength -= 2 * inset;
    }

    setColor(color);
    if (w.one.row == w.two.row) {
        startx += this->wallLength;
        starty += inset;
        drawLine(startx, starty, startx, starty + wallLength);
    } else {
        starty += this->wallLength;
        startx += inset;
        drawLine(startx, starty, startx + wallLength, starty);
    }
}

void MazeGeneratorView::drawColoredLine(double startx, double starty,
                                        double endx, double endy,
                                        const string& color) {
    setColor(color);
    drawLine(startx, starty, endx, endy);
}

void MazeGeneratorView::drawBorder() {
    drawColoredLine(ulx, uly, ulx, uly + kMazeSize, kMazeVisibleColor);
    drawColoredLine(ulx, uly + kMazeSize,
                    ulx + kMazeSize - wallLength, uly + kMazeSize, kMazeVisibleColor);
    drawColoredLine(ulx + kMazeSize - wallLength, uly + kMazeSize,
                    ulx + kMazeSize, uly + kMazeSize, kMazeInvisibleColor);
    drawColoredLine(ulx + kMazeSize, uly + kMazeSize,
                    ulx + kMazeSize, uly, kMazeVisibleColor);
    drawColoredLine(ulx + kMazeSize, uly,
                    ulx + wallLength, uly, kMazeVisibleColor);
    drawColoredLine(ulx + wallLength, uly,
                    ulx, uly, kMazeInvisibleColor);
}

void MazeGeneratorView::drawWall(const wall& w) {
    drawWall(w, kMazeVisibleColor, 0);
    if(w.one.row == w.two.row) {
        verticalWalls[w.one.row][min(w.one.col, w.two.col)] = true;
    } else {
        horizontalWalls[min(w.one.row, w.two.row)][w.one.col] = true;
    }
}

void MazeGeneratorView::removeWall(const wall& w) {
    drawWall(w, kMazeInvisibleColor, wallLength / 5);
    if(w.one.row == w.two.row) {
        verticalWalls[w.one.row][min(w.one.col, w.two.col)] = false;
    } else {
        horizontalWalls[min(w.one.row, w.two.row)][w.one.col] = false;
    }
}

string MazeGeneratorView::getMazeAsString() {
    string result;
    for(int row = 0; row < dimension; row++) {
        for(int col = 0; col < dimension; col++) {
            result += "C";
            if(col < dimension - 1) {
                if(!verticalWalls[row][col]) {
                    result += "*";
                } else {
                    result += " ";
                }
            }
        }
        result += "\n";
        if(row < dimension - 1) {
            for(int col = 0; col < dimension; col++) {
                if(!horizontalWalls[row][col]) result += "*";
                else result += " ";
                if(col < dimension - 1) result += " ";
            }
            result += "\n";
        }
    }
    return result;
}

void MazeGeneratorView::printMaze() {
    cout << getMazeAsString() << endl;
}
