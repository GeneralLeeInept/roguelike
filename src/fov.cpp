#include "fov.h"

#include <algorithm>

Fov::Fov(int range)
    : _range(range)
{
    int max_tiles = (range * 2) * (range * 2);
    _tiles.resize(max_tiles);
    _visibility.resize(max_tiles, true);
}

void Fov::update(int x, int y, const MapDef& map_def)
{
    _pvs_min_x = std::max(x - _range, 0);
    _pvs_max_x = std::min(x + _range, map_def.width);
    _pvs_min_y = std::max(y - _range, 0);
    _pvs_max_y = std::min(y + _range, map_def.height);

    for (int y = _pvs_min_y; y < _pvs_max_y; ++y)
    {
        for (int x = _pvs_min_x; x < _pvs_max_x; ++x)
        {
            int index = tile_index(x, y);
            _tiles[index] = (map_def.tiles[x + y * map_def.width].type == TileType::Wall);
        }
    }

    for (auto& v : _visibility)
    {
        v = false;
    }

    for (int ex = _pvs_min_x; ex < _pvs_max_x; ++ex)
    {
        cast_ray(x, y, ex, _pvs_min_y);
        cast_ray(x, y, ex, _pvs_max_y - 1);
    }

    for (int ey = _pvs_min_y; ey < _pvs_max_y; ++ey)
    {
        cast_ray(x, y, _pvs_min_x, ey);
        cast_ray(x, y, _pvs_max_x - 1, ey);
    }
}

bool Fov::can_see(int x, int y) const
{
    int index = tile_index(x, y);

    if (index < 0)
    {
        return false;
    }

    return _visibility[index];
}

int Fov::tile_index(int x, int y) const
{
    if (x < _pvs_min_x || x >= _pvs_max_x || y < _pvs_min_y || y >= _pvs_max_y)
    {
        return -1;
    }

    x -= _pvs_min_x;
    y -= _pvs_min_y;

    return x + y * _range * 2;
}

/*
  Octants for Bresenham

        \3|2/
        4\|/1
       ---+----
        5/|\0
        /6|7\
*/

void to_octant0(int from_octant, int x, int y, int& ox, int& oy)
{
    switch (from_octant)
    {
        case 0:
        {
            ox = x;
            oy = y;
            break;
        }
        case 1:
        {
            ox = x;
            oy = -y;
            break;
        }
        case 2:
        {
            ox = -y;
            oy = x;
            break;
        }
        case 3:
        {
            ox = -y;
            oy = -x;
            break;
        }
        case 4:
        {
            ox = -x;
            oy = -y;
            break;
        }
        case 5:
        {
            ox = -x;
            oy = y;
            break;
        }
        case 6:
        {
            ox = y;
            oy = -x;
            break;
        }
        case 7:
        {
            ox = y;
            oy = x;
            break;
        }
    }
}

void from_octant0(int to_octant, int x, int y, int& ox, int& oy)
{
    switch (to_octant)
    {
        case 0:
        {
            ox = x;
            oy = y;
            break;
        }
        case 1:
        {
            ox = x;
            oy = -y;
            break;
        }
        case 2:
        {
            ox = y;
            oy = -x;
            break;
        }
        case 3:
        {
            ox = -y;
            oy = -x;
            break;
        }
        case 4:
        {
            ox = -x;
            oy = -y;
            break;
        }
        case 5:
        {
            ox = -x;
            oy = y;
            break;
        }
        case 6:
        {
            ox = -y;
            oy = x;
            break;
        }
        case 7:
        {
            ox = y;
            oy = x;
            break;
        }
    }
}

// Use Bresenham's algorithm to march along the ray marking tiles visible until
// the end of the ray or a visibility barrier is reached
void Fov::cast_ray(int sx, int sy, int ex, int ey)
{
    int octant = 0;

    if (ex >= sx)
    {
        if (ey >= sy)
        {
            if ((ex - sx) >= (ey - sy))
            {
                octant = 0;
            }
            else
            {
                octant = 7;
            }
        }
        else
        {
            if ((ex - sx) >= (sy - ey))
            {
                octant = 1;
            }
            else
            {
                octant = 2;
            }
        }
    }
    else
    {
        if (ey >= sy)
        {
            if ((sx - ex) > (ey - sy))
            {
                octant = 5;
            }
            else
            {
                octant = 6;
            }
        }
        else
        {
            if ((sx - ex) > (sy - ey))
            {
                octant = 4;
            }
            else
            {
                octant = 3;
            }
        }
    }

    int x0, y0;
    int x1, y1;

    to_octant0(octant, sx, sy, x0, y0);
    to_octant0(octant, ex, ey, x1, y1);

    int dx = x1 - x0, dy = y1 - y0, y = y0, eps = 0;

    for (int x = x0; x <= x1; x++)
    {
        int tx, ty;
        from_octant0(octant, x, y, tx, ty);
        int index = tile_index(tx, ty);
        _visibility[index] = true;

        if (_tiles[index])
        {
            return;
        }

        eps += dy;

        if ((eps * 2) >= dx)
        {
            y++;
            eps -= dx;
        }
    }
}
