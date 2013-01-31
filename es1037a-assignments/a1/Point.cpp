#include "Point.h"

Point :: Point( int x_init, int y_init ) {
             // construct the point specified by the parameters
    m_x = x_init;
    m_y = y_init;
}

void Point :: shift( int x_amount, int y_amount ) {
    m_x += x_amount;
    m_y += y_amount;
    }
 void Point :: rotate90( ) {
    int new_x, new_y;
    new_x = m_y;   new_y = -m_x;
    m_x = new_x;   m_y = new_y;
    }