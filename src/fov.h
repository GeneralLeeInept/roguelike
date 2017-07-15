#pragma once

#include <vector>

#include "map_def.h"

class Fov
{
public:
    Fov(int range);

    void update(int x, int y, const MapDef& map_def);

    bool can_see(int x, int y) const;

private:
    int _range;
    int _pvs_min_x;
    int _pvs_max_x;
    int _pvs_min_y;
    int _pvs_max_y;

    std::vector<bool> _tiles;
    std::vector<bool> _visibility;

    int tile_index(int x, int y) const;
    void cast_ray(int sx, int sy, int ex, int ey);
};
