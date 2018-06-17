#include "Tile.h"

Tile::Tile(int _x, int _y, int _sx, int _sy, bool _isCollidable) :
    x(_x),
    y(_y),
    sx(_sx),
    sy(_sy),
    isCollidable(_isCollidable)
    {}

Tile::Tile() :
    x(0),
    y(0),
    sx(0),
    sy(0),
    isCollidable(false)
    {}

ScrollingField::ScrollingField(int _x, int _y, int _w, int _h, int _offsetX, int _offsetY) :
    x(_x),
    y(_y),
    width(_w),
    height(_h),
    offsetX(_offsetX),
    offsetY(_offsetY)
    {}
