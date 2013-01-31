#include "Rect.h"

Rect::Rect(Point top_left, Point bottom_right)
{
    m_top_left=top_left;
    m_bottom_right=bottom_right;
}

Rect::Rect(Point top_left, int width, int height)
{
    m_top_left=top_left;
    m_bottom_right=top_left;
    m_bottom_right.shift(width,height);
}

Rect::~Rect(void)
{
}

bool Rect::ptInRect(Point p) const
{
    if (p.x >= m_top_left.x && 
        p.x < m_bottom_right.x &&
        p.y >= m_top_left.y &&
        p.y < m_bottom_right.y) return true;
    return false;
}

void Rect::expand(int dLength, int dHeight)
{
    m_top_left.shift(-dLength/2,-dHeight/2);
    m_bottom_right.shift(dLength-dLength/2,dHeight-dHeight/2);
}
