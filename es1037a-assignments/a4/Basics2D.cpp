#include "Basics2D.h"

Point :: Point( ) { x=0; y=0; }

Point :: Point( int x_init, int y_init ) {// construct the point specified by the parameters
    x = x_init;
    y = y_init;
}

Vect :: Vect( ) { x=0; y=0; }

Vect :: Vect( double x_init, double y_init ) {// construct the vector specified by the parameters
    x = x_init;
    y = y_init;
}
