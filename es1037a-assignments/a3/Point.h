#pragma once

class Point
{
public:
    Point( ); // default constructor that builds the origin, p=(0,0)
    Point( int x_init, int y_init ); 
        // construct the point specified by the parameters
        

    void shift( int x_amount, int y_amount );
        // function shifts a point by the amount specified in the parameters

    void rotate90( );  // rotate the point 90 degrees clockwise

public:
    int x;  // x coordinate of this point
    int y;  // y coordinate of this point
};

inline Point operator+(Point a, Point b) {return Point(a.x+b.x,a.y+b.y);}
inline bool operator==(Point a, Point b) {return (a.x==b.x && a.y==b.y);}
