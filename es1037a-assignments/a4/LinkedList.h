#pragma once

template <class Item>
class LinkedList   //singly-linked version as in class notes 13
{
private:
    class Node { // singly-linked nodes (basic containers of a linked list)  
    public:
        Node( Item val, Node * nx=NULL ) : value(val), next(nx)  { }
        Item  value;
        Node * next;
    };

public:
    LinkedList( );      // construct an empty linked list
    ~LinkedList( );     // destroy a linked list
    unsigned int size( ) const;  // return number of items in the list
    bool isEmpty( ) const; // true if list is empty, false otherwise
    void addHead( Item item );  // adds item to front of list
    Item removeHead( );  // remove front node, and return the data Item found in it;
                         // PRECONDITION: list must not be empty already
    Item retrieveHead( ) const; // returns the data Item in front node; 
                                // PRECONDITION: list must not be empty already
    void addTail( Item item ); // add item to end of list
    Item removeTail( ); // remove last node, and return the data Item found in it;
                        // PRECONDITION: list must not be empty already
    Item retrieveTail( ) const; // return data Item in last node; 
                                // PRECONDITION: list must not be empty already
    void add( unsigned int n, Item item ); // Add a new node containing an item in position n on the list
                                           // PRECONDITION: n must be a valid list position (1<=n<=(size+1))
    Item replace(unsigned int n, Item item); // Replace the data stored in position n of the list with
                                             // item, and return the replaced item; 
                                             // PRECONDITION: n must be a valid list position (1<=n<=size)  
    Item remove( unsigned int n ); // Remove the node at position n from the list, and return the
                                   // item it contained; PRECONDITION: n must be a valid list position (1<=n<=size)
    Item retrieve( unsigned int n ) const; // Return the item value from the node in position n; 
                                           // PRECONDITION: n must be a valid list position (1<=n<=size)

private:
    Node * m_head;
    Node * m_tail;
    unsigned int m_size;

public:
    class Iterator  {// iterators are used for efficient traversal of linked lists
    private: 
        Node* m_ptr;    // an iterator hides a pointer to node
    public:  
        Iterator(Node* ptr)  {m_ptr=ptr;}
        void operator++ ()   {m_ptr=m_ptr->next;} // for forward traversing, e,g, Iterator i=begin(); ... ++i;
        bool operator != (const Iterator& b)   {return m_ptr!=b.m_ptr;}
        Item getValue()   {return m_ptr->value;} 
        void setValue(Item val)   {m_ptr->value=val;} 
    };
	
    // linked list objects create forward-traversal iterators using two functions below
    Iterator begin() {return Iterator(m_head);}
    Iterator end()   {return Iterator(NULL);}
};

#include "LinkedList.template"