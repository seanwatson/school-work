#pragma once
#include "Point.h"

class Rect
{
public:
    Rect(Point top_left=Point(), Point bottom_right=Point());
    Rect(Point top_left, int width, int height);
    ~Rect(void);

    bool ptInRect(Point p) const;
    int get_top() const {return m_top_left.y;}
    int get_left() const {return m_top_left.x;}
    int get_bottom() const {return m_bottom_right.y;}
    int get_right() const {return m_bottom_right.x;}

    int get_length() const {return get_right()-get_left();}
    int get_height() const {return get_bottom()-get_top();}
    int get_area() const {return get_length()*get_height();}
    int get_perimeter() const {return 2*(get_length()+get_height());}

    void expand(int dLength, int dHeight);
    void shift(int dx, int dy) {m_top_left.shift(dx,dy); m_bottom_right.shift(dx,dy);}

private:
    Point m_top_left;
    Point m_bottom_right;
};
