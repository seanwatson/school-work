#pragma once

#include "LinkedList.h"
 

template <class Item>
 class Queue {
 public:
     Queue( ); // builds an empty queue
     ~Queue( ); 
     bool isEmpty( ) const; // true if queue is empty, false otherwise
     bool isFull( ) const;  // true if queue is full, false otherwise
     void enqueue( Item k ); // add k to the end of the queue
     Item dequeue( );   // remove the front item from the queue, and return it; 
                        // precondition: queue must not be empty already
     Item peek( ) const; // retrieve front item from queue without removing it; 
                         // precondition: queue must not be empty already  
private:
     LinkedList<Item> m_container;
};

#include "Queue.template"
