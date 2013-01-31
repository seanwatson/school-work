#pragma once
#define DEFAULT_BAG_SIZE 20

// NOTE: your "main" should initialize a random number generator (via "srand") when using Bag<Item> objects

template <class Item>
class Bag {
public: 
    Bag(unsigned int max_capacity=DEFAULT_BAG_SIZE); // construct a bag to hold max_capacity elements
    ~Bag( );                // destructor

    void add(Item value);   // adds a new item into the bag, PRECONDITION: the bag must not be "full"
    Item getOne( );         // extracts an item from the bag, PRECONDITION: the bag must not be "empty"

    bool isEmpty( ) const; 
    bool isFull( ) const;
    unsigned int currentSize( ) const;   // returns # of items now in bag
    unsigned int capacity( ) const;        // returns bag’s upper capacity

private: 
    unsigned int m_size; // number of elements currently stored in the bag
    unsigned int m_max_capacity; // the maximum capacity of the bag 
    Item * m_container;   // internal storage array of pointers to items 
};  

#include "Bag.template" // header file for a templated class must include implementation

