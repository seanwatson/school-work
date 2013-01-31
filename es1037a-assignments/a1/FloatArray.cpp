#include "FloatArray.h"



FloatArray::FloatArray(){             //Default constructor sets width and height to 0
	width = height = 0;
	data = NULL;
}

FloatArray::FloatArray(int w, int h){ //Constructor with parameters sets width and height to inputs and initializes array
	width = w; height = h;
	data = new float[width*height];
	for (int i=0; i<w*h; i++) data[i]=0;
}

FloatArray::FloatArray(const FloatArray& a){         //Copy constructor makes array with same values as input arrary
	width = a.width; height = a.height;
	data = new float[width*height];
	for (int i=0; i<width*height; i++) data[i]=a.data[i];
}

FloatArray::~FloatArray(){ //Destructor deletes dynamic variables
	delete [] data;
}

void FloatArray::operator =(const FloatArray& a){ //Overloaded copy operator copys input to array
	delete [] data; //Delete previous data
	width = a.width;
	height = a.height;
	data = new float[width*height]; //Create new dynamic array
	for (int i=0; i<width*height; i++) data[i]=a.data[i];
}

float * FloatArray::operator [](int x){ //Overloaded operator returns address of xth column
	if (x<width)                        //Check if the input is valid
		return data+x*height;
	else{
		cout << "Error. Not part of array. Quitting program."; //If not valid print error and exit program
		cin.ignore();
		exit(0);
	}
}
FloatArray FloatArray::operator +(const FloatArray& a){ //Overload operator adds two arrays together and returns the result
	if (width==a.width&&height==a.height){ //Confirm arrays are the same size
		FloatArray sum(width, height);
		for (int i=0; i<width*height; i++)sum.data[i]=data[i]+a.data[i];
		return sum;
	}
	else{
		cout << "Error. Arrays not the same size. Quitting program."; //Print error and exit if not the same size
		cin.ignore();
		exit(0);
	}
	
}
