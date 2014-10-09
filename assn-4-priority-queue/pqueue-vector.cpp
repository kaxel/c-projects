#include "pqueue-vector.h"
#include "error.h"
#include <iostream>

using namespace std;

VectorPQueue::VectorPQueue() {
    array = new string[NUMALLOCATED];
    numallocated=NUMALLOCATED;
    numOfItems = 0;
}

VectorPQueue::~VectorPQueue() {
    delete []array;
}

string VectorPQueue::peek() const {
    string s;

    if (numOfItems == 0) {
        error("This queue is empty.");
    } else {
        s = array[0];
        for (int i=1; i<numOfItems; i++) {
            if (array[i] < s) {
                s = array[i];
            }
        }
    }
    return s;

}

string VectorPQueue::extractMin() {
    string s;
    int priorityindex;

    if (numOfItems == 0) {
        error("This queue is empty.");
    }
     else {
        s = array[0];
        priorityindex=0;
        for (int i=1; i<numOfItems; i++) {
            if (array[i] < s) {
                s = array[i];
                priorityindex = i;
            }
        }
        rebuildTable(priorityindex);
    }
    return s;

}

void VectorPQueue::enqueue(const string& elem) {
    int newNum = numOfItems++;
    if (numOfItems == numallocated-1) {
        addCapacity();
    }
    array[newNum] = elem;
}

VectorPQueue *VectorPQueue::merge(VectorPQueue *one, VectorPQueue *two) {
    VectorPQueue* merged = new VectorPQueue;

    if (one->isEmpty()) merged = two;
    if (two->isEmpty()) merged = one;

    while (true) {
        if (one->isEmpty()) break;
        string val = one->extractMin();
        merged->enqueue(val);
    }

    while (true) {
        if (two->isEmpty()) break;
        string val = two->extractMin();
        merged->enqueue(val);
    }

    return merged;

}

int VectorPQueue::size() {
    return numOfItems;
}

void VectorPQueue::rebuildTable(int index) {
    for (int i=index; i<numOfItems; i++) {
        array[i] = array[i+1];
    }
    numOfItems--;
}

bool VectorPQueue::isEmpty() {
        return (numOfItems==0);
}

void VectorPQueue::addCapacity() {
    string *arrayCopy;
    int newsize = numallocated*2;

    arrayCopy = new string[newsize];

    for (int i=0; i < numOfItems; i++) {
        arrayCopy[i] = array[i];
    }
    delete[] array;
    array = arrayCopy;

    numallocated = newsize;
}
