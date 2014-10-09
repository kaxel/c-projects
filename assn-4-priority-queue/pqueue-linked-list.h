#ifndef _linked_list_pqueue_
#define _linked_list_pqueue_

#include "pqueue.h"
#include <string>
#include "error.h"

class LinkedListPQueue : public PQueue {
public:
	LinkedListPQueue();
	~LinkedListPQueue();
	
	static LinkedListPQueue *merge(LinkedListPQueue *one, LinkedListPQueue *two);
	
	void enqueue(const std::string& elem);
    std::string extractMin();
    std::string peek() const;

    int size();
    bool isEmpty();
	
private:
    void insertNode(Node* newNode);
    void destroyList();

    int numOfItems;
    struct Node {
        std::string value;
        Node *next;
        Node *previous;
    };
    Node *topOfList;

};

#endif
