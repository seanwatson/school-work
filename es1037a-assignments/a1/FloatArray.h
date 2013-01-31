#pragma once
#include <iostream>
using namespace std;



class FloatArray
{
public:

	FloatArray();                                 //Default constructor declaration
	FloatArray(int w, int h);                     //Constructor with parameters declaration
	FloatArray (const FloatArray& a);             //Copy constructor declaration
	~FloatArray();                                //Destructor declaration
	void operator=(const FloatArray& a);          //Overload copy operator
	float * operator[](int x);                    //Overload operator[]
	FloatArray operator+(const FloatArray& a);    //Overload operator+
	
private:

	int width, height;                            //Declare variables to store width and height of array
	float *data;                                  //Declare pointer to point to dynamic array

	
};