#pragma once
#include "Basics2D.h"

////////////////////////////////////////////////////////////////////////////////////////////
// THIS FILE DEFINES (TEMPLATED) CLASS "Vector1D" REPRESENTING 1D ARRAYS WITH ELEMENTS    //
// OF ANY TYPE "T".   BASIC MATHEMATICAL OPERATIONS FOR Vector1D OBJECTS ARE IN Math2D.h  //
// GLOBAL FUNCTIONS FOR DISPLAYING Vector1D (AS 2D CONTOURS OR 2D PLOTS) ARE IN Image2D.h //
////////////////////////////////////////////////////////////////////////////////////////////


template <class T> 
class Vector1D {
public:
        // constructing/copying of 1d arrays 
    Vector1D();    // creates an empty array of zero width 
    Vector1D(unsigned width); // initialized array of given width (values are not initialized)
    Vector1D(unsigned width, T val); // as above, but also initializes all values in the array
    Vector1D(const Vector1D<T>& src); // copy constructor
    Vector1D<T>& operator=(const Vector1D<T>& src); // copy operator
    ~Vector1D(); // destructor

    template <class type>              // conversion constructor that works
    Vector1D(const Vector1D<type>& src); // works if casting from "type" to "T" is defined
    template <class type>                            // conversion operator that works
    Vector1D<T>& operator=(const Vector1D<type>& src); // if casting "type->T" is defined
 
        // basic quiry functions
    bool isEmpty() const {return (m_container==NULL);} // checks if array is empty (zero width)
    unsigned getWidth() const {return m_width;} 
    T getMin() const; // uses operator "<" for type "T"  PRECONDITION: Vector should be non empty
    T getMax() const; // uses operator ">" for type "T"  PRECONDITION: Vector should be non empty
	                                       
        // functions for checking "in-range" array indexes
    bool pointIn(unsigned x) const {return (x<m_width);}
    bool pointIn(int x) const {return (0<=x && ((unsigned)x)<m_width);}

        // operators allowing access of items in "Vector1D<T> a" using syntax` "a[x]"
    T& operator[](unsigned x) const;  // PRECONDITION: index "x" must be in-range
    T& operator[](int x) const;       

        // functions for resizing/resetting arrays
    Vector1D<T>& resize(unsigned width); // creates new container (but values are not initialized)
    Vector1D<T>& reset(T val); // assigns new value to all items
    Vector1D<T>& reset(unsigned width, T val) {resize(width); return reset(val);}

        // basic arithmetic operators
    Vector1D<T>& operator+=(const Vector1D<T>& arg); // adding another Vector1D
    Vector1D<T>& operator-=(const Vector1D<T>& arg); // subtracting another Vector1D
    Vector1D<T>& operator+=(const T& val); // adding a constant to each element of the vector
    Vector1D<T>& operator-=(const T& val); // subtracting a constant from each element of the vector
    Vector1D<T>& operator*=(const double& s); // multiplication by a constant scalar 
    Vector1D<T>& operator%=(const Vector1D<double>& src); // point-wise scaling

        // member functions for transforming into new arrays via "point-processing" --- trg[x] = f( arr[x] )
    template <class type>     // via linear scaling to new range of values [min_val,max_val]  (works for scalar type)
    void convertTo(Vector1D<type>& trg, const double new_min, const double new_max) const;
    template <class type, class Conversion>    // f - "conversion functor" with "convert(type,T)" method
    void convertTo(Vector1D<type>& trg, const Conversion& f) const; 
    template <class type>     // f - "function pointer", e.g. Vector1D<double> b(10,4), a; b.convertTo(a,&sqrt);
    void convertTo(Vector1D<type>& trg, type (*f)(T item)) const; 

private:
    T* m_container;
    unsigned m_width;
};


//////////////////////////////////////////////////////////////////////////////////////

    // global functions for transforming Vector1D objects via "point-processing" --- trg[x] = f( arr[x] )
template <class type, class T>  // using linear scaling to new range [new_min,new_max]  (works for scalar type)
Vector1D<type> convert(const Vector1D<T>& src, const double new_min, const double new_max);

template <class type, class T>   // f - "function pointer", e.g.  Vector1D<double> b(10,4), a = convert<double>(b,&sqrt);
Vector1D<type> convert(const Vector1D<T>& src, type (*f) (T)); 

template <class type, class T, class Conversion>  // f - "conversion functor" with "convert(type,T)" method
Vector1D<type> convert(const Vector1D<T>& src, const Conversion& f);


#include "Vector1D.template"
