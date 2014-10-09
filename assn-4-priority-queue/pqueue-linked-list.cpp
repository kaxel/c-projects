#include "pqueue-linked-list.h"
#include <iostream>

// I am really sorry this doesn't work that well, I have the enqueue dequeue in decent shape
// but the delete is not working right and I am getting 'The program has unexpectedly finished.' errors

using namespace std;

LinkedListPQueue::LinkedListPQueue() {
    numOfItems = 0;
}

LinkedListPQueue::~LinkedListPQueue() {
    destroyList();
}

void LinkedListPQueue::destroyList() {

}

string LinkedListPQueue::peek() const {
    if (numOfItems == 0) error("Sorry, this is an empty list.");
    return topOfList->value;
}

string LinkedListPQueue::extractMin() {
    if (numOfItems == 0) error("Sorry, this is an empty list.");
    string s = topOfList->value;
    Node *next = topOfList->next;

    delete topOfList;
    topOfList = next;
    numOfItems--;

    return s;
}

void LinkedListPQueue::insertNode(Node* newNode) {

    Node *current = NULL;
    Node *successor = NULL;

    current = topOfList;

    //look for match
    while (true) {
        cout << "look for match " << newNode->value << " to " << current->value <<endl;
        if (newNode->value >= current->value) {
            //newNode comes after current
            // not sure why setting the previous pointer is crashing the system

            if (current->next != NULL) {
                cout << "set successor" <<endl;
                successor=current->next;
                newNode->next = successor;
                //current->next->previous=newNode;
            }
            //newNode->previous=current;
            break;
        } else {
            current=current->next;
        }

    }

}

void LinkedListPQueue::enqueue(const string& elem) {

    cout << "enqueue" <<endl;

    Node *newNode = new Node;
    newNode->value = elem;

    if (numOfItems==0) {
        topOfList = newNode;
    } else {
        // cycle through possible places to put new node
        insertNode(newNode);
    }

    numOfItems++;
}

LinkedListPQueue *LinkedListPQueue::merge(LinkedListPQueue *one, LinkedListPQueue *two) {
	// placeholder so method compiles..
	// replace with your own implementation
	return new LinkedListPQueue();
}

int LinkedListPQueue::size() {
    return numOfItems;
}

bool LinkedListPQueue::isEmpty() {
    return (numOfItems==0);
}

