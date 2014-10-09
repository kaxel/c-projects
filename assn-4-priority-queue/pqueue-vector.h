#ifndef _vector_pqueue_
#define _vector_pqueue_

#include "pqueue.h"
#include <string>

using namespace std;

class VectorPQueue : public PQueue {
public:
	VectorPQueue();
	~VectorPQueue();

    int size();
    bool isEmpty();

	static VectorPQueue *merge(VectorPQueue *one, VectorPQueue *two);
	
	void enqueue(const std::string& elem);
    std::string extractMin();
    std::string peek() const;
	
private:
    string *array;
    int numOfItems;
    int numallocated;
    const int NUMALLOCATED = 12;

    void rebuildTable(int index);
    void addCapacity();
};

#endif
