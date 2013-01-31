#pragma once

const int DEFAULT_LIST = 200;

template <class Item>
class List{
public: 
    List(unsigned int capacity=DEFAULT_LIST); // constructor for an empty list of specified capacity; 
	                                          // if capacity is not supplied, the default size is used 
    ~List( ); // destructor

    bool isEmpty() const; // returns true if the list is empty, and false otherwise  
    bool isFull() const; // returns true if the list is full, and false otherwise 
    unsigned int getLength() const; // returns the number of items currently in the list
    
    void insert (unsigned int pos, Item item); // inserts item into the list at location pos; 
                                               	// PRECONDITION: pos is between 1 and getLength()+1
    Item remove (unsigned int pos); // removes the element at location pos from the list, and returns it;
                                    // PRECONDITION: the list is not empty, and pos is between 1 and getLength();
    Item retrieve (unsigned int pos) const; // find the element stored at location pos in the list and return it; 
                                            	// PRECONDITION: the list is not empty, and pos is between 1 and getLength();
    Item replace (unsigned int pos, Item item); // replace the element at location pos in the list with item,
                                                // and return the element that was displaced; 
                                                // PRECONDITION: the list is not empty, and pos is between 1 and getLength();
    void append(Item item) {return insert(getLength()+1,item);} // adds an item at the back of the list
    Item back() const {return retrieve(getLength());} // retrieves an item from the back of the list
    Item popBack() {return remove(getLength());} // removes an item from the back of the list
    void swap(unsigned int i, unsigned int j); // swaps two Items located in positions i and j

    template <class Equality> // "find" uses linear search to return position of the first item identical to "key",
    int find(Item key) const; // returns -1 if no identical Item is found. Extra template argument <class Equality> 
                              // must be a functor like "IsEqual" or "IsEqualDeref" defined in StandardFunctors.h. 
                              // EXAMPLE: call lst.find<IsEqual>(4) returns location of integer 4 inside List<int> lst;  
    template <class Order> // these two functions sort (via either "selection" or "insertion" sorting algorithms)
    void selectionSort();  // content of the list based on specified "Order". Additional template argument <class Order> 
                           // should be a comparison functor like "IsLess" or "IsGreaterDeref" from StandardFunctors.h             
    template <class Order> // EXAMPLE: can be called in main() as     a.selectionSort<IsLess>();
    void insertionSort();  // to sort integers in List<int> a; in increasing order
 

private:
    Item * m_container; // pointer to array to hold the list
    unsigned int m_max_capacity; // maximum capacity of the list
    unsigned int m_size;    // number of items currently in the list
    unsigned int translate(unsigned int pos) const; // converts a list position to the corresponding array index
    void replaceContainer(); // private function replaces the underlying array with one that holds
                             // the same values as the original, but has double the capacity
};

#include "List.template" // header file for a templated class must include implementation

