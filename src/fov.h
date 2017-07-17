#pragma once

#include <vector>

#include "geometry.h"
#include "map_def.h"

class Fov
{
public:
    Fov(int range);

    void update(const Point& view_position, const MapDef& map_def);

    bool can_see(const Point& position) const;

private:
    int _range;
    Rectangle _pvs;
    
    std::vector<bool> _tiles;
    std::vector<bool> _visibility;

    int tile_index(int x, int y) const;
    void cast_ray(const Point& from, const Point& to);
};
