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
#include "stack.h"
#include "Queue.h"

static void waitForEnter(string message) {

    cout << message;
    (void) getLine();
}

static bool compareCells(cell thiscell, cell thatcell) {

    if (thiscell.row==thatcell.row && thiscell.col==thatcell.col) {
        return true;
    } else {
        return false;
    }

}

static bool compareWalls(wall thiswall, wall thatwall) {

    if (
            (compareCells(thiswall.one, thatwall.one)) &&
            (compareCells(thiswall.two, thatwall.two))
       ) {
        return true;
    } else {
        return false;
    }
}

static cell otherCell(wall w, cell c)
  {
    //return other cell of wall
    if (w.one.row==c.row && w.one.col==c.col) {
        return w.two;
    } else {
        return w.one;
    }
}

static void getGreaterCell(cell x, cell y, cell& returncell) {
    if (x.row>y.row || x.col>y.col) {
        returncell = x;
    } else {
        returncell = y;
    }
}

static void getLesserCell(cell x, cell y, cell& returncell) {
    if (x.row<y.row || x.col<y.col) {
        returncell = x;
    } else {
        returncell = y;
    }
}

static Set<cell> getSurroundingCells (cell c, int dimension){
    Set<cell> surrounding;
    cell newcell;
    int max = dimension-1;

    if (c.row == 0) {
        // top row
        if (c.col==0) {
            //left side
            newcell.row=0;
            newcell.col=1;
            surrounding.add(newcell);
            newcell.row=1;
            newcell.col=0;
            surrounding.add(newcell);
        } else if (c.col==(max)) {
            //right side
            newcell.row=1;
            newcell.col=max;
            surrounding.add(newcell);
            newcell.row=0;
            newcell.col=(max-1);
            surrounding.add(newcell);
        } else {
            //middle
            newcell.row=c.row;
            newcell.col=(c.col-1);
            surrounding.add(newcell);
            newcell.row=(c.row+1);
            newcell.col=c.col;
            surrounding.add(newcell);
            newcell.row=c.row;
            newcell.col=(c.col+1);
            surrounding.add(newcell);
        }
    } else if (c.row == (max)) {
        //bottom row
        if (c.col==0) {
            //left side
            newcell.row=(max-1);
            newcell.col=c.col;
            surrounding.add(newcell);
            newcell.row=max;
            newcell.col=(c.col+1);
            surrounding.add(newcell);
        } else if (c.col==(max)) {
            //right side
            newcell.row=(max-1);
            newcell.col=c.col;
            surrounding.add(newcell);
            newcell.row=max;
            newcell.col=(c.col-1);
            surrounding.add(newcell);
        } else {
            //middle
            newcell.row=max;
            newcell.col=(c.col-1);
            surrounding.add(newcell);
            newcell.row=(max-1);
            newcell.col=c.col;
            surrounding.add(newcell);
            newcell.row=max;
            newcell.col=(c.col+1);
            surrounding.add(newcell);
        }
    }

    return surrounding;
}

static Set<cell> getJoinedCells (cell c, Vector<wall> walls) {
    Set<cell> returnset;
    wall comparewall;
    cell newcell;

    while (true) {
        //cycle walls
        comparewall = walls.get(0);

        if (compareCells(comparewall.one, c)) {
            newcell.row=comparewall.two.row;
            newcell.col=comparewall.two.col;
            returnset.add(newcell);

        } else if (compareCells(comparewall.two, c)) {
            newcell.row=comparewall.one.row;
            newcell.col=comparewall.one.col;
            returnset.add(newcell);
        }
        walls.remove(0);
        if (walls.isEmpty()) break;
    }
    return returnset;
}


static void getAdjacentUnwalled(cell finalcell, cell callingcell, Vector<wall> & walls, Set<cell> & returncells, int dimension) {
    // this is where we decide which of 3 possible cells belong in returncells

    cell greaterthan;
    cell lesserthan;
    //cell targetCell;
    Set<cell> potentials;
    Set<cell> cellswithwallsjoined;

    getGreaterCell(finalcell, callingcell, greaterthan);
    getLesserCell(finalcell, callingcell, lesserthan);

    cout << "greaterthan " << greaterthan.row << ", " << greaterthan.col << endl;
    cout << "lesserthan " << lesserthan.row << ", " << lesserthan.col << endl;



    potentials = getSurroundingCells(finalcell, dimension);

    cout << "target cell " << finalcell.row << ", " << finalcell.col << " has " << potentials.size() << " matches. " << endl;

    //no need to remove because it comes in with the joined cells set
    potentials.remove(callingcell);

    //get set of cells with walls adjoining this cell and subtract sets
    cellswithwallsjoined = getJoinedCells(finalcell, walls);

    //whatever is left over
    returncells=(potentials-cellswithwallsjoined);
}

static int MatchingWalls(wall thiswall, cell thiscell, Vector<wall> & walls, Queue<cell> & opencells, int dimension) {
    Vector<wall> wallscopy = walls;
    int wcount = 0;
    wall comparewall;
    cell finalcell;
    Set<cell> adjunwalled;

    //edge cases
    if (thiscell.row==0) wcount+=1;
    if (thiscell.col==0) wcount+=1;
    if (thiscell.row==(dimension-1)) wcount+=1;
    if (thiscell.col==(dimension-1)) wcount+=1;

    cout << "start MatchingWalls:" << endl;

    while (true) {
        //cycle wallscopy
        comparewall = wallscopy.get(0);
        if (!compareWalls(comparewall, thiswall)) {
            //not matching thiswall
            if ((compareCells(comparewall.one, thiscell)) || (compareCells(comparewall.two, thiscell))) wcount+=1;
        }
        wallscopy.remove(0);
        if (wallscopy.isEmpty()) break;
    }

    cout << " MatchingWalls." << endl;

    if (wcount < 3) {
        int finalx;
        int finaly;
        //add chamber to opencells collection
        if (compareCells(thiswall.one, thiscell)) {
            //opencells.push(thiswall.one);
            finalx = thiswall.one.row;
            finaly = thiswall.one.col;
        } else {
            finalx = thiswall.two.row;
            finaly = thiswall.two.col;
        }
        cout << "find adjacentcells with no walls for " << finalx << ", " << finaly << " calling from " << otherCell(thiswall, thiscell).row << ", " << otherCell(thiswall, thiscell).col << endl;
        cout << " count of opencells " << opencells.size() << endl;

        finalcell.row=finalx;
        finalcell.col=finaly;
        getAdjacentUnwalled(finalcell, (otherCell(thiswall, thiscell)), walls, adjunwalled, dimension);
        if (adjunwalled.isEmpty()) return wcount;

        int counter = 0;

        while (true) {
            if (adjunwalled.isEmpty()) break;
            if (counter>20) break;
            opencells.enqueue(adjunwalled.first());
            adjunwalled.remove(opencells.peek());
            counter+=1;
        }


        //opencells.pop();

    }


    return wcount;
}

static bool isEnclosedChamber(wall thiswall, cell thiscell, Vector<wall> & walls, Queue<cell> & opencells, int dimension) {
    //find 3 walls that don't match thiswall in walls collection that contain thiscell
    //if not add cells to opencells that are not bound by wall
    int wallcount = 0;

    cout << " isEnclosedChamber:" << endl;

    wallcount = MatchingWalls(thiswall, thiscell, walls, opencells, dimension);
    cout << " isEnclosedChamber finishing:" << endl;

    if (wallcount == 3) {
        return true;
    } else {
        return false;
    }

}

static bool removeWall(wall thiswall, Vector<wall> walls, int dimension) {
    Queue<cell> opencells;
    cell addcell;
    cell thiscell;
    bool answer = true;

    //start with two 'parent' cells for thiswall
    addcell = thiswall.one;
    opencells.enqueue(addcell);
    addcell = thiswall.two;
    opencells.enqueue(addcell);
    cout << " opencells populated." << endl;
    cout << "opencells count: " << opencells.size() << endl;

    while (true) {
        //return true if both cells are valid enclosed chambers
        thiscell = opencells.dequeue();
        cout << " got thiscell." << endl;
        if (opencells.isEmpty()) break;
        answer = isEnclosedChamber(thiswall, thiscell, walls, opencells, dimension);
        cout << "opencells count: " << opencells.size() << endl;
    }
    return answer;
}


static int getMazeDimension(string prompt, int minDimension = 2, int maxDimension = 50) {
	while (true) {
		int response = getInteger(prompt);
		if (response == 0) return response;
        if (response >= minDimension && response <= maxDimension) return response;
		cout << "Please enter a number between "
			 << minDimension << " and " 
		     << maxDimension << ", inclusive." << endl;
	}
}

static void breakWalls(Vector<wall> walls, MazeGeneratorView& maze, int dimension) {
    int count = 0;
    int r = 0;
    int whichi;
    wall thiswall;

    while (true) {
        cout << "size " << endl;
        r = walls.size()-1;
        whichi = randomInteger(0,r);
        cout << "get whichi " << whichi << endl;
        thiswall = walls.get(whichi);
        cout << "got it. " << endl;
        if (removeWall(thiswall, walls, dimension)) {
            cout << "removing thiswall " << thiswall.one.row << ", " << thiswall.one.col << " and " << thiswall.two.row << ", " << thiswall.two.col << " " << endl;
            maze.removeWall(thiswall);
            cout << "wall removed " << endl;
        }


        cout << whichi << " whichi in " << walls.size() << " for " << thiswall.one.row << ", " << thiswall.one.col << " and " << thiswall.two.row << ", " << thiswall.two.col << " " << endl;
        walls.remove(whichi);
        if (walls.isEmpty()) break;
        count+=1;

        cout << endl;
        waitForEnter("xx");
    }
    cout << count << " walls were considered." << endl;
}

int main()  {
    MazeGeneratorView maze;

	while (true) {
		int dimension = getMazeDimension("What should the dimension of your maze be [0 to exit]? ");
		if (dimension == 0) break;

        maze.setDimension(dimension);
        maze.drawBorder();
        cell cellone;
        cell celltwo;
        Set<cell> chambers;
        Set<cell> ccopy;
        Vector<wall> walls;
        wall newwall;
        //int count = 0;

        int maxx = dimension - 1;
        int maxy = dimension - 1;

        //collect chambers
        for (int x = 0; x<(dimension);x++) {
            for (int y = 0; y<(dimension);y++) {
                cellone.row=x;
                cellone.col=y;
                chambers.add(cellone);
            }
        }

        //assemble all walls - copy chambers
        ccopy=chambers;
        while (true) {
            cellone = ccopy.first();
            //make sure we are not all the way right
            if (cellone.col != maxy) {
                //draw right
                celltwo.row = cellone.row;
                celltwo.col = (cellone.col+1);
                newwall.one = cellone;
                newwall.two = celltwo;
                walls.add(newwall);
                maze.drawWall(newwall);
            }

            if (cellone.row != maxx) {
                //draw bottom
                celltwo.row = cellone.row+1;
                celltwo.col = (cellone.col);
                newwall.one = cellone;
                newwall.two = celltwo;
                walls.add(newwall);
                maze.drawWall(newwall);
            }

            ccopy.remove(cellone);
            if (ccopy.isEmpty()) break;
        }

        breakWalls(walls, maze, dimension);

	}

	return 0;
}


