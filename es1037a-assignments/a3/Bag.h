#pragma once
#define DEFAULT_BAG 20
#include "List.h" //Include list for m_container

// NOTE: your "main" should initialize a random number generator (via "srand") when using Bag<Item> objects

template <class Item>
class Bag {
public: 
    Bag(unsigned int max_capacity=DEFAULT_BAG); // construct an empty bag of specified "max_capacity"
	                                        // if no value is provided, DEFAULT_BAG is used
    ~Bag( );  // destructor (recycles memory allocated by constructor)

    void add(Item value); // adds a new item into the bag, 
                          // PRECONDITION: the bag must not be "full"
    Item getOne( ); // extracts an item from the bag, 
                    // PRECONDITION: the bag must not be "empty"
    bool isEmpty( ) const; 
    bool isFull( ) const;
    unsigned int currentSize( ) const;   // returns # of items now in bag
    unsigned int capacity( ) const;        // returns bag’s upper capacity

private: 
    unsigned int m_size; // number of elements currently stored in the bag
    unsigned int m_max_capacity; // the maximum capacity of the bag 
    List<Item> m_container;   // internal storage list of items
};  

#include "Bag.template" // header file for a templated class must include implementation

