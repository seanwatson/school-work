#pragma once
#include "Vector1D.h"
#include "Table2D.h"
#include "math.h"

////////////////////////////////////////////////////////////////////////////////////////////
// THIS FILE DEFINES GLOBAL OPERATORS, FUNCTIONS, AND SOME EXTRA CLASSES (e.g. Kernel2D)  // 
// ALLOWING BASIC STANDARD MATHEMATICAL OPERATIONS ON Table2D and Vector1D OBJECTS. E.G.: //
// MATRIX ADDITION, MULTIPLICATION, CONVOLUTION, SCALAR AND VECTOR MULTIPLICATION, etc.   //
////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Global operators for Table2D and Kernel2D objects //
///////////////////////////////////////////////////////

template <class T> // scalar multiplication
Table2D<T> operator*(const Table2D<T>& a, const double& scalar); 

template <class T> // scalar multiplication
Table2D<T> operator*(const double& scalar, const Table2D<T>& a); 

template <class T> // adding a (scalar-valued) constant to all elements 
Table2D<T> operator+(const Table2D<T>& a, const double& val); 

template <class T> // adding a (scalar-valued) constant to all elements
Table2D<T> operator+(const double& val, const Table2D<T>& a); 

template <class T> // subtracting a (scalar-valued) constant from all elements
Table2D<T> operator-(const Table2D<T>& a, const double& val); 

template <class T> // subtracting from a (scalar-valued) constant
Table2D<T> operator-(const double& val, const Table2D<T>& a); 

template <class T> // matrix summation, PRECONDITION; arrays should have the same size
Table2D<T> operator+(const Table2D<T>& a, const Table2D<T>& b); 

template <class T> // matrix subtraction, PRECONDITION; arrays should have the same size
Table2D<T> operator-(const Table2D<T>& a, const Table2D<T>& b); 

template <class T> // matrix multiplication, PRECONDITION; width of "a" must match height of "b"
Table2D<T> operator*(const Table2D<T>& a, const Table2D<T>& b); 

template <class T> // POINTWISE scaling, PRECONDITION; arrays should have the same size
Table2D<T> operator%(const Table2D<T>& a, const Table2D<double>& s); 

template <class T, class K> // basic matrix convolution, PRECONDITION: kernel's size should be (preferably much) smaller than arr's size
Table2D<T> operator*(const Table2D<T>& a, const Kernel2D<K>& kernel); // NOTE: for correlation, use   a*(~kernel) 
                                                                       // where operator~ returns kernel's transpose
template <class T, class K> // basic normallized cross-correlation (NCC), PRECONDITION: kernel's size should be (preferably much) smaller than arr's size
Table2D<T> operator%(const Table2D<T>& a, const Kernel2D<K>& kernel); //faster code for bigger kernels (templates ) could be found (see J.P. Lewis)


///////////////////////////////////////////////////////
// Global operators for Vector1D and Table2D objects //
///////////////////////////////////////////////////////

template <class T> // scalar multiplication
Vector1D<T> operator*(const Vector1D<T>& v, const double& scalar); 

template <class T> // scalar multiplication
Vector1D<T> operator*(const double& scalar, const Vector1D<T>& v); 

template <class T> // adding a (scalar-valued) constant to all elements 
Vector1D<T> operator+(const Vector1D<T>& v, const double& val); 

template <class T> // adding a (scalar-valued) constant to all elements 
Vector1D<T> operator+(const double& val, const Vector1D<T>& v); 

template <class T> // subtracting a (scalar-valued) constant to all elements 
Vector1D<T> operator-(const Vector1D<T>& v, const double& val); 

template <class T> // subtracting from a (scalar-valued) constant
Vector1D<T> operator-(const double& val, const Vector1D<T>& v); 

template <class T> // vector summation (point-wise), PRECONDITION; vectors should have the same size
Vector1D<T> operator+(const Vector1D<T>& a, const Vector1D<T>& b); 

template <class T> // vector subtraction (point-wise), PRECONDITION; vectors should have the same size
Vector1D<T> operator-(const Vector1D<T>& a, const Vector1D<T>& b); 

template <class T> // "dot product" multiplication of vectors, PRECONDITION; vectors have the same size
T operator*(const Vector1D<T>& a, const Vector1D<T>& b); 

template <class T> // POINTWISE scaling, PRECONDITION; vectors should have the same size
Vector1D<T> operator%(const Vector1D<T>& a, const Vector1D<double>& s); 

template <class T, class A> // vector-by-matrix multiplication, PRECONDITION; array height equals vectors size
Vector1D<T> operator*(const Vector1D<T>& vect, const Table2D<A>& a); 

template <class T, class A> // matrix-by-vector multiplication, PRECONDITION; array width equals vectors size
Vector1D<T> operator*(const Table2D<A>& a, const Vector1D<T>& vect); 


///////////////////////////////////////////////////////////////////////////
// useful global functions for Table2D or Vector1D using operators above //
///////////////////////////////////////////////////////////////////////////

  //  computes |intensity gradient|^2,  PRECONDITION: kernel's size should be smaller than arr's size
inline Table2D<double> grad2(const Table2D<double>& intensity); // uses default kernels dX() and dY() (defined below) 
inline Table2D<double> grad2(const Table2D<double>& intensity, const Kernel2D<double>& dX, const Kernel2D<double>& dY); 


/////////////////////////////////////////////////////////////////////////////
// Definition of class Kernel2D used for "matrix convolution" operaton.    //
// Kernel2D is a special case of Table2D allowing negative indexes as in   //
// "a[-2][3]" or "a[0][-1]". This is convenient for "convolution" above.   //   
//                                                                         //
// NOTE: Table2D is a "private" base class for Kernel2D. Yet, one can cast //
// Kernel2D object as Table2D (conversion-constructor in Table2D.h) as in  //
//   --- drawImage(((Table2D<double>) SobolX()).resize(16)*400+125 );      //
//   --- drawImage(((Table2D<double>) DoG(3)).resize(16),ShowRange(-5,5)); //
//   --- drawImage( autoScale(((Table2D<double>) GaussY(3)).resize(16)) ); //
// where SobolX, DoG, GaussY are Kerenl2D objects defined below, while     //
// drawImage/rescale/*/+ are Table2D methods (see Table2D.h and Image2D.h).//
/////////////////////////////////////////////////////////////////////////////

template <class T> 
class Kernel2D : private Table2D<T> { 
public:
        // constructing/copying: must specify "center point" (cell arr[0][0]) w.r.t. array's top-left corner 
    Kernel2D(); // default constructor creates Kernel of width=height=1 and value 1;
    Kernel2D(unsigned width, unsigned height, int centerX, int centerY, T val=0); 
    Kernel2D(unsigned width, unsigned height, Point center, T val=0); 
    Kernel2D(unsigned width, unsigned height, T val=0); // default center point in set in the middle of the "kernel"
    Kernel2D(const Kernel2D<T>& src);  // copy constructor
    Kernel2D<T>& operator=(const Kernel2D<T>& src); // copy operator
    Kernel2D(const Table2D<T>& src);   // constructor from table2D (centers kernel in the middle of the table)
    Kernel2D(const Table2D<T>& src, int centerX, int centerY);   // constructor from table2D
    Kernel2D(const Table2D<T>& src, Point center);   // constructor from table2D
    Kernel2D<T>& operator=(const Table2D<T>& src); // conversion operator
    ~Kernel2D();

        // basic quiry functions
    bool isEmpty() const {return (m_container==NULL);} // checks if array is empty (zero width and/or height)
    unsigned getWidth() const {return m_width;} 
    unsigned getHeight() const {return m_height;}	                                       
    int left() const;   // index of the left-most columm
    int right() const;  // index of the right-most column
    int top() const;    // index of the top-most row
    int bottom() const; // index of the bottom-most row

        // functions for checking "in-range" array indexes
    bool pointIn(int x, int y) const {return (left()<=x && x<=right() && top()<=y && y<=bottom());}
    bool pointIn(Point p) const {return pointIn(p.x,p,y);}

        // operators allowing access of items in "Kernel2D<T> a" using syntax` "a[x][y]" or "a[p]"
    T* operator[](int x) const;   // "a[x]" returns address of the 0-th row item in column "x"
    T& operator[](Point p) const; // "a[p]" returns item "a[p.x][p.y]"

        // basic arithmetic operators
    Kernel2D<T> operator+(const Kernel2D<T>& src) const; // sums 2 kernells
    Kernel2D<T> operator-(const Kernel2D<T>& src) const; // subtracts 2 kernells
    Kernel2D<T> operator+(const T& scalar) const; // adds a scalar to each Item
    Kernel2D<T> operator-(const T& scalar) const; // subtracts a scalar from each Item
    Kernel2D<T> operator*(const T& scalar) const; // scalar multiplication 
    Kernel2D<T> operator~() const; // returns new transposed kernel

private:
    int m_centerX; // position of the center-column w.r.t the left-most column of the array
    int m_centerY; // position of the center-row w.r.t the top-most row of the 2d array
    T * m_center; // address of the center cell

    inline  unsigned shift() const {return (m_centerY + m_centerX*m_height);} // shift to center-pixel in m_container 
};


/////////////////////////////////////////////////////////////////////////////////////
// Functions initializing standard "kernels" frequently used in matrix convolution //
/////////////////////////////////////////////////////////////////////////////////////

// basic kernels for computing "derivatives" in 'X' and 'Y' directions (horizontally and vertically)
inline const Kernel2D<double> dX() {Kernel2D<double> t(3,1); t[-1][0]=0.5; t[1][0]=-0.5; return t; }
inline const Kernel2D<double> dY() {return ~dX();  } // transpose operator

// "Sobol" kernels for computing "derivatives" in 'X' and 'Y' directions (horizontally and vertically)
inline const Kernel2D<double> SobolX() {Kernel2D<double> t(3,3); t[-1][-1]=t[-1][ 1]=0.125; t[ 1][-1]=t[1][1]=-0.125; t[-1][ 0]=0.25; t[1][0]=-0.25; return t; }
inline const Kernel2D<double> SobolY() {return ~SobolX(); } // transpose operator

// "Gauss" kernels for smoothing in 'X' and 'Y' directions (horizontally and vertically)
// For faster 2D smoothing use   "arr*GaussX*GaussY",  which is equivalent, but faster than  "arr*Gauss"
inline const Kernel2D<double> GaussX(double sigma=0.65)  {unsigned w=(sigma>0.5)?((unsigned)(2*sigma)):1; Kernel2D<double> t(1+2*w,1); double s2=2*sigma*sigma, norm = t[0][0] = 1; for (int i=1; i<=((int)w); i++) norm+=2*(t[-i][0]=t[i][0]=exp(-i*i/s2)); return t*(1/norm);}
inline const Kernel2D<double> GaussY(double sigma=0.65)  {return ~GaussX(sigma);} // transpose operator
inline const Kernel2D<double> Gauss(double  sigma=0.65)  {unsigned w=(sigma>0.5)?((unsigned)(2*sigma)):1; Kernel2D<double> t(1+2*w,1+2*w); double s2=2*sigma*sigma, norm = 0; for (int x=-((int)w); x<=((int)w); x++) for (int y=-((int)w); y<=((int)w); y++) norm+=t[x][y]=exp(-(x*x+y*y)/s2); return t*(1/norm);}

// "Box" kernels for smoothing in 'X' and 'Y' directions (horizontally and vertically)
// For 2D box-smoothing use   "arr*BoxX*BoxY", which is equivalent, but faster than  "arr*Box"
inline const Kernel2D<double> BoxX(unsigned half_width=1)  {Kernel2D<double> t(1+2*half_width,1,1.0/(1.0+2*half_width)); return t;}
inline const Kernel2D<double> BoxY(unsigned half_height=1) {return ~BoxX(half_height);} // transpose operator
inline const Kernel2D<double> Box(unsigned half_side=1)  {Kernel2D<double> t(1+2*half_side,1+2*half_side,1.0/((1.0+2*half_side)*(1.0+2*half_side))); return t;}

// "DoG" kernels (sharpening filters). 
// NOTE: in this case "arr*DoG" differes from  "arr*DoGX*DoGY"  
inline const Kernel2D<double> DoGX(double sigma=1.25)  {return GaussX() - GaussX(sigma);}
inline const Kernel2D<double> DoGY(double sigma=1.25)  {return ~DoGX(sigma);} // transpose operator
inline const Kernel2D<double> DoG(double  sigma=1.25)  {return Gauss() - Gauss(sigma);}

// Basic "Laplace" and "LoG" (Laplacian of Gaussian) kernels         (second-order "derivatives")
inline const Kernel2D<double> Laplace() {Kernel2D<double> t(3,3); t[0][0]=4; t[-1][0]=t[0][-1]=t[1][0]=t[0][1]=-1.0; return t; }
inline const Kernel2D<double> LoG(double  sigma=1.25) {double s2=2*sigma*sigma, norm=0; Kernel2D<double> t=Gauss(sigma)*(4/s2); for (int x=t.left(); x<=t.right(); x++) for (int y=t.top(); y<=t.bottom(); y++) norm+=(t[x][y]*=((s2-x*x-y*y)/s2)); return t-(norm/(t.getWidth()*t.getHeight()));}


#include "Math2D.template"