#pragma once

/////////////////////////////////////////////////////////////////////////////////
// THIS FILE DEFINES SOME GENERAL BASIC TYPES USEFUL FOR ARRAYS (e.g. Table2D) //
//  AND STANDARD CONVERSION FUNCTORS THAT CAN BE USED FOR TRANSFORMING ARRAYS  //  
/////////////////////////////////////////////////////////////////////////////////


////////////////////////
// BASIC USEFUL TYPES //
////////////////////////
class Point {
public:
    int x, y;  // x and y coordinates of this point
    Point( ); // default constructor that builds the origin, p=(0,0)
    Point(int x_init, int y_init); 
    Point operator*(const double& s) const {return Point((int)(s*x),(int)(s*y));}
    Point operator+(const Point& a) const {return Point(x+a.x,y+a.y);}
    Point operator-(const Point& a) const {return Point(x-a.x,y-a.y);}
    bool operator==(const Point& a) const {return (x==a.x && y==a.y);}
};

class Vect {
public:
    double x, y;  // x and y coordinates of this vector
    Vect( ); // default constructor that builds null vector, v=(0,0)
    Vect(double x_init, double y_init); 
    Vect operator*(const double& s) const {return Vect(s*x,s*y);}
    Vect operator+(const Vect& a) const {return Vect(x+a.x,y+a.y);}
    Vect operator-(const Vect& a) const {return Vect(x-a.x,y-a.y);}
};



//////////////////////////////////
// STANDARD CONVERSION FUNCTORS //
//////////////////////////////////

    // conversion functor using type casting   (also used in Table2D constructors, see Table2D.h)
class Casting {   // Example:  Table2D<unsigned char> a; ...  Table2D<double> b;
public:           //           b = a;    <=>    b = convert<double>(a,Casting());   
    template <class T, class S>      // PRECONDITION: casting S->T should be defined
    inline static void  convert(T& t, const S& s) { t = (T) s; } 
};

    // conversion functor for scalar-types (int, double, unsigned,...) using gain-&-bias linear scaling
class Scaling {   // Examples: Table2D<double>   a = image;
    double g,b;   //           Table2D<short>    b = convert<short>(a*LoG(),Scaling(20));
public:           //           Table2D<unsigned> c = convert<unsigned>(a*DoG(),Scaling(10,1000));
    Scaling(const double& gain, const double& bias=0.0) { g = gain;  b = bias; }
    template <class type, class T>
	inline void  convert(type& t, const T& s) const  {t = (type)( ((double)s)*g + b );}
};

    // conversion functor for integer types (int, char, unsigned,...) using look-up tables ("palettes")
class Palette {    // PRECONDITION: look_up table "palette[i]" must be valid for all possible values of argument "i"
public:            // Example:   double alpha[3] = {0.2, 1, 0};  Table2D<int> mask; ... // assume mask's values 0,1,2  
    void* look_up; //            Table2D<double> alpha_mat = convert<double>(mask,Palette(alpha));
    Palette(void* palette) {look_up = palette;}
    template <class T>
    inline void  convert(T& t, const int& i) const { t = ((T*)look_up)[i];
    }
};

