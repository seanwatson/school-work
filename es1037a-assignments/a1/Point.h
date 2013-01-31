#pragma once

class Point
{
public:

	Point( int x_init=0, int y_init=0 ); 
		// construct the point specified by the parameters
		// default constructor builds the origin

    void shift( int x_amount, int y_amount );
                  // function shifts a point by the amount specified in
                  // the parameters

    void rotate90( );   // rotate the point 90 degrees clockwise

	int get_x( ) const { return m_x; } // inline function returns the x coordinate of this point
    int get_y( ) const { return m_y; } // inline function returns the y coordinate of this point

private:
    int m_x;  // x coordinate of this point
    int m_y;  // y coordinate of this point
};
