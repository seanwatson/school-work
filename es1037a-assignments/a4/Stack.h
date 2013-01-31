#pragma once

#define DEFAULT_STACK  30

#include "List.h"

template <class Item>
class Stack {
 public:
      Stack(unsigned int capacity = DEFAULT_STACK); // construct a stack with specified initial capacity; 
                                                    // if the user does not provide a value, DEFAULT_STACK is used
     ~Stack( );  // destructor
     bool isEmpty( ) const; // returns true if stack is empty, and false otherwise   
     bool isFull( ) const;  // returns true if stack is full, and false otherwise 
     unsigned int getSize() const; // returns the number or items in the stack
     void push( Item k ); // adds new item on top of the stack
     Item pop( );  // Remove and return the top element from the stack; 
                   // PRECONDITION: the stack must not already be empty
     Item peek() const; // return the top element without modifying the stack; 
                        // PRECONDITION: the stack must not be empty
 
private:
     List<Item> m_container; // "unlimited" capacity storage
     
};
#include "Stack.template"
