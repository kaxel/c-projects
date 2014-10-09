/**
 * File: maze-generator.cpp
 * ------------------------
 * Presents an adaptation of Kruskal's algorithm to generate mazes.
 */

#include <iostream>
using namespace std;

#include "console.h"
#include "simpio.h"
#include "maze-graphics.h"
#include "maze-types.h"
#include "set.h"
#include "random.h"
#include "vector.h"

static int getMazeDimension(string prompt,
                            int minDimension = 7, int maxDimension = 50) {
	while (true) {
		int response = getInteger(prompt);
		if (response == 0) return response;
        if (response >= minDimension && response <= maxDimension) return response;
		cout << "Please enter a number between "
			 << minDimension << " and " 
		     << maxDimension << ", inclusive." << endl;
	}
}

static void attemptRemoval(cell c1, cell c2, Vector<Set<cell> >& chambers) {
    // if c1 and c2 are contained in the same chamber do nothing.
    // if they are in different chambers COMBINE those two chambers
    // by removing the two matches from chambers and adding the single combined set
    Set<cell> comparechamber1;
    Set<cell> comparechamber2;
    Set<cell> combined;

    for (int i=0; i<chambers.size(); i++) {
        if (chambers.get(i).contains(c1)) {
            comparechamber1 = chambers.get(i);
        }
        if (chambers.get(i).contains(c2)) {
            comparechamber2 = chambers.get(i);
        }
    }
    if (comparechamber1.size()>0 && comparechamber2.size()) {
        Set<cell> combined;
        combined=(comparechamber1 + comparechamber2);

        for (int i=0; i<chambers.size(); i++) {
            if (chambers.get(i)==comparechamber1) chambers.remove(i);
        }
        for (int i=0; i<chambers.size(); i++) {
            if (chambers.get(i)==comparechamber2) chambers.remove(i);
        }
        chambers.add(combined);
    }
}

static void drawInitial (Vector<Set<cell> >& chambers, Vector<wall>& walls, MazeGeneratorView& maze, int dimension) {
    cell cellone;
    cell celltwo;
    int maxx = dimension - 1;
    int maxy = dimension - 1;
    wall newwall;

    //all chambers are one cell right now
    for (int i=0; i<chambers.size(); i++) {
        Set<cell> chamber;
        chamber = chambers.get(i);
        cellone = chamber.first();
        //cout << "built this cell " << cellone.row << ", " << cellone.col << " for i= " << i << endl;

        //make sure we are not all the way right - drawing right side wall
        if (cellone.col != maxy) {
            //draw right
            celltwo.row = cellone.row;
            celltwo.col = (cellone.col+1);
            newwall.one = cellone;
            newwall.two = celltwo;
            walls.add(newwall);
            maze.drawWall(newwall);
        }
        //make sure we are not all the way bottom - drawing bottom wall
        if (cellone.row != maxx) {
            //draw bottom
            celltwo.row = cellone.row+1;
            celltwo.col = (cellone.col);
            newwall.one = cellone;
            newwall.two = celltwo;
            walls.add(newwall);
            maze.drawWall(newwall);
        }

    }
}


static void breakWalls(Vector<Set<cell> >& chambers, Vector<wall>& walls, MazeGeneratorView& maze) {
    int count = 0;
    int r = 0;
    int whichwall;
    wall thiswall;
    int prevchambersize;
    int origwallsize;

    origwallsize = walls.size();
    for (int i=0; i<origwallsize; i++) {

        cell c1;
        cell c2;

        r = walls.size()-1;
        whichwall = randomInteger(0,r);
        thiswall = walls.get(whichwall);
        count+=1;
        c1 = thiswall.one;
        c2 = thiswall.two;
        prevchambersize = chambers.size();
        attemptRemoval(c1, c2, chambers);

        if (chambers.size()<prevchambersize) maze.removeWall(thiswall);
        if (walls.isEmpty()) break;
        if (chambers.size()==1) break;
        walls.remove(whichwall);
    }
    cout << count << " walls were considered of " << origwallsize << endl;
}

int main()  {
    MazeGeneratorView maze;

        while (true) {
            int dimension = getMazeDimension("What should the dimension of your maze be [0 to exit]? ");
            if (dimension == 0) break;

            maze.setDimension(dimension);
            maze.drawBorder();
            cell cellone;
            Vector<Set<cell> > chambers;
            Vector<wall> walls;

            //collect chambers
            for (int x = 0; x<(dimension);x++) {
                for (int y = 0; y<(dimension);y++) {
                    Set<cell> chamber;
                    cellone.row=x;
                    cellone.col=y;
                    chamber.add(cellone);
                    chambers.add(chamber);
                }
            }
            drawInitial(chambers, walls, maze, dimension);
            breakWalls(chambers, walls, maze);
        }
        return 0;
}
