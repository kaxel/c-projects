/******************************************************************************
 * File: Trailblazer.cpp
 *
 * Implementation of the graph algorithms that comprise the Trailblazer
 * assignment.
 */

#include "Trailblazer.h"
#include "TrailblazerGraphics.h"
#include "TrailblazerTypes.h"
#include "TrailblazerPQueue.h"
#include "random.h"
using namespace std;

struct Val {
    Color locColor;
    double cost;
    Loc parent;
};

// ******************* GETTER / SETTER FUNCTIONS for the Val struct
static void setColor(Grid<double>& world, Grid<Val>& vals, Loc& loc, Color newColor) {
    colorCell(world, loc, newColor);
    vals[loc.row][loc.col].locColor=newColor;
}

static Color getColor(Grid<Val>& vals, Loc& loc) {
    return vals[loc.row][loc.col].locColor;
}

static void setCost(Grid<Val>& vals, Loc& loc, double newCost) {
    vals[loc.row][loc.col].cost=newCost;
}

static double getCost(Grid<Val>& vals, Loc& loc) {
    return vals[loc.row][loc.col].cost;
}

static void setParent(Grid<Val>& vals, Loc& loc, Loc& newParent) {
    vals[loc.row][loc.col].parent=newParent;
}

static Loc getParent(Grid<Val>& vals, Loc& loc) {
    return vals[loc.row][loc.col].parent;
}
// ******************* END

static Vector<Loc> flipVector(const Vector<Loc>& unflipped) {
    Vector<Loc> returnvector;
    for (int i=unflipped.size()-1; i>=0; i--) returnvector.add(unflipped[i]);
    return returnvector;
}

static Vector<Loc> constructVector(Grid<Val>& vals, Loc& finalLoc, Loc& startLoc) {
    Vector<Loc> returnvector;
    Loc myloc = finalLoc;
    Loc parentloc = getParent(vals, myloc);

    returnvector.add(finalLoc);
    while (true) {
        myloc = parentloc;
        returnvector.add(myloc);
        if (myloc==startLoc) break;
        parentloc = getParent(vals, parentloc);
    }
    return flipVector(returnvector);
}

static Vector<Loc> constructVector(Grid<Val>& vals, Loc& finalLoc, Loc& start, Loc& end) {
    Vector<Loc> v1;
    Vector<Loc> v2;
    Loc myloc = finalLoc;
    Loc parentloc1 = getParent(vals, myloc);
    Loc parentloc2 = getParent(vals, myloc);

    //returnvector.add(finalLoc);

    while (true) {
        myloc = parentloc1;
        v1.add(myloc);
        if (myloc==start || myloc==end) break;
        parentloc1 = getParent(vals, parentloc1);
    }
    while (true) {
        myloc = parentloc2;
        v2.add(myloc);
        if (myloc==start || myloc==end) break;
        parentloc2 = getParent(vals, parentloc2);
    }
    //we have two vectors, only one needs to be flipped
    return flipVector(flipVector(v1) + v2);
}

static Vector<Loc> getAdjacent(const Loc& start, Grid<double>& world) {

    Vector<Loc> retvector;

    for (int x=start.row-1; x<=start.row+1; x++){
        for (int y=start.col-1; y<=start.col+1; y++) {
            if (x>=0 && y>=0 && x<world.numRows() && y<world.numCols()) {
                Loc l = makeLoc(x, y);
                retvector.add(l);
            }
        }
    }
    return retvector;
}

// KRUSKAL helpers
static void gatherClusters (Map<int, Vector<Loc>>& clusters, int& rows, int& cols) {
    int counter = 1;
    for (int x=0; x<rows; x++){
        for (int y=0; y<cols; y++) {
            Loc newloc = makeLoc(x, y);
            Vector<Loc> newvec;
            newvec.add(newloc);
            clusters.put(counter, newvec);
            counter++;
        }
    }
}

static void createEdges (TrailblazerPQueue<Edge>& pq, int& rows, int& cols) {
    for (int x=0; x<rows; x++){
        for (int y=0; y<cols; y++) {
            // down:
            if (x<rows-1) {
                double prob = randomReal(0,1);
                Loc first = makeLoc(x, y);
                Loc second = makeLoc(x+1, y);
                Edge newedge = makeEdge(first, second);
                pq.enqueue(newedge, prob);
            }
            // right:
            if (y<cols-1) {
                double prob = randomReal(0,1);
                Loc first = makeLoc(x, y);
                Loc second = makeLoc(x, y+1);
                Edge newedge = makeEdge(first, second);
                pq.enqueue(newedge, prob);
            }
        }
    }
}

static int findCluster(const Loc& loc, Map<int, Vector<Loc>>& clusters) {
    for (int index : clusters) {
        Vector<Loc> vl = clusters[index];
        for (Loc l : vl) {
            if (l==loc) return index;
        }
    }
    error("no cluster was found.");
    return 0;
}

static void mergeClusters(const int& idstart, const int& idend, Map<int, Vector<Loc>>& clusters) {

    Vector<Loc> v1 = clusters[idstart];
    Vector<Loc> v2 = clusters[idend];

    //compare size so we only traverse the smaller vector
    if (v1.size()>=v2.size()) {
        for (Loc l : v2) v1.add(l);
        clusters[idstart] = v1;
        clusters.remove(idend);
    } else {
        for (Loc l : v1) v2.add(l);
        clusters[idend] = v2;
        clusters.remove(idstart);
    }
}

Vector<Loc>
shortestPath(Loc start,
             Loc end,
             Grid<double>& world,
             double costFn(Loc from, Loc to, Grid<double>& world),
             double heuristic(Loc start, Loc end, Grid<double>& world)) {

    Vector<Loc> finalvec;
    TrailblazerPQueue<Loc> pq;
    Grid<Val> vals(world.numRows(), world.numCols());
    setColor(world, vals, start, YELLOW);
    setCost(vals, start, 0);
    pq.enqueue(start, 0);

    while (!pq.isEmpty()) {
        Loc curr = pq.dequeueMin();
        colorCell(world, curr, GREEN);

        if (curr.row==end.row && curr.col==end.col) {
            finalvec = constructVector(vals, curr, start);
            break;
        }
        for (Loc l : getAdjacent(curr, world)) {
            double associatedCost = costFn(curr, l, world) + getCost(vals, curr);
            double priorityH = heuristic(l, end, world);
            switch (getColor(vals, l)) {
                case GRAY:
                    setColor(world, vals, l, YELLOW);
                    setCost(vals, l, associatedCost);
                    setParent(vals, l, curr);
                    pq.enqueue(l, associatedCost+priorityH);
                    break;
                case YELLOW:
                if (associatedCost<getCost(vals, l)) {
                    setCost(vals, l, associatedCost);
                    setParent(vals, l, curr);
                    //update q priority
                    pq.decreaseKey(l, associatedCost+priorityH);
                    break;
                case GREEN:
                    // should never happen; added here to suppress warning
                    break;
                }
            }
        }
    }
    if (finalvec.size()==0) error("no path was found.");
    return finalvec;
}

Set<Edge> createMaze(int numRows, int numCols) {

    Set <Edge> finaltree;
    Map<int, Vector<Loc>> clusters;
    // I opted to use an id keyed map to point to vectors of Loc
    // the find is slow, but the equality comparison is trivial and the merge is easy.
    // The huge world maze builds in under 20 seconds which is not too bad.
    TrailblazerPQueue<Edge> pq;

    // place each node in it's own cluster;
    gatherClusters(clusters, numRows, numCols);

    // insert all edges in the graph into a priority queue
    createEdges(pq, numRows, numCols);

    while (clusters.size()>1) {
        // dequeue an edge from pq
        Edge curedge = pq.dequeueMin();
        //find start cluster
        int sc = findCluster(curedge.start, clusters);
        //find end cluster
        int ec = findCluster(curedge.end, clusters);
        // if endpoints are not in same cluster
        if (sc!=ec) {
            // merge clusters of each endpoint
            mergeClusters(sc, ec, clusters);
            // add edge to spanning tree
            finaltree.add(curedge);
        }
    }
    return finaltree;
}


Vector<Loc>
shortestPathBiDirectional(Loc start,
             Loc end,
             Grid<double>& world,
             double costFn(Loc from, Loc to, Grid<double>& world),
             double heuristic(Loc start, Loc end, Grid<double>& world)) {

    Vector<Loc> finalvec;
    TrailblazerPQueue<Loc> pq;
    Grid<Val> vals(world.numRows(), world.numCols());
    setColor(world, vals, start, YELLOW);
    setColor(world, vals, end, YELLOW);
    setCost(vals, start, 0);
    setCost(vals, end, 0);
    pq.enqueue(start, 0);
    pq.enqueue(end, 0);

    while (!pq.isEmpty()) {

        Loc current = pq1.dequeueMin();
        colorCell(world, currleft, GREEN);
        colorCell(world, currright, GREEN);

        if (currleft.row==currright.row && currleft.col==currright.col) {
            finalvec = constructVector(vals, currleft, start, end);
            break;
        }

        for (Loc l : getAdjacent(currleft, world)) {
            double associatedCost1 = costFn(currleft, l, world) + getCost(vals, currleft);
            switch (getColor(vals, l)) {
                case GRAY:
                    setColor(world, vals, l, YELLOW);
                    setCost(vals, l, associatedCost1);
                    setParent(vals, l, currleft);
                    pq.enqueue(l, associatedCost1);
                    break;
                case YELLOW:
                if (associatedCost1<(getCost(vals, l))) {
                    setCost(vals, l, associatedCost1);
                    setParent(vals, l, currleft);
                    //update q priority (if necessary)
                    pq.decreaseKey(l, associatedCost1);
                    break;
                case GREEN:
                    // should never happen; added here to suppress warning
                    break;
                }
            }
        }


    }

    if (finalvec.size()==0) error("no path was found.");
    return finalvec;
}

