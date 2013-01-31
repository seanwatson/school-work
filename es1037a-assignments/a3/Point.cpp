#include "Point.h"

Point :: Point( ) { x=0; y=0; }

Point :: Point( int x_init, int y_init ) {// construct the point specified by the parameters
    x = x_init;
    y = y_init;
}

void Point :: shift( int x_amount, int y_amount ) {
    x += x_amount;
    y += y_amount;

}
 void Point :: rotate90( ) {
    int new_x, new_y;
    new_x = y;   new_y = -x;
    x = new_x;   y = new_y;
}