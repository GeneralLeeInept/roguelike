#include "fov.h"

#include <algorithm>

Fov::Fov(int range)
    : _range(range)
{
    int max_tiles = (range * 2) * (range * 2);
    _tiles.resize(max_tiles);
    _visibility.resize(max_tiles, true);
}

void Fov::update(int view_x, int view_y, const MapDef& map_def)
{
    _pvs_min_x = std::max(view_x - _range, 0);
    _pvs_max_x = std::min(view_x + _range, map_def.width);
    _pvs_min_y = std::max(view_y - _range, 0);
    _pvs_max_y = std::min(view_y + _range, map_def.height);

    for (int tile_y = _pvs_min_y; tile_y < _pvs_max_y; ++tile_y)
    {
        for (int tile_x = _pvs_min_x; tile_x < _pvs_max_x; ++tile_x)
        {
            int index = tile_index(tile_x, tile_y);
            _tiles[index] = (map_def.tiles[tile_x + tile_y * map_def.width].type == TileType::Wall);
        }
    }

    for (auto& tile : _visibility)
    {
        tile = false;
    }

    // Cast rays to determine visible tiles
    for (int end_x = _pvs_min_x; end_x < _pvs_max_x; ++end_x)
    {
        cast_ray(view_x, view_y, end_x, _pvs_min_y);
        cast_ray(view_x, view_y, end_x, _pvs_max_y);
    }

    for (int end_y = _pvs_min_y + 1; end_y < _pvs_max_y - 1; ++end_y)
    {
        cast_ray(view_x, view_y, _pvs_min_x, end_y);
        cast_ray(view_x, view_y, _pvs_max_x, end_y);
    }

    // Apply jice's post-process to fix artifacts
    for (int tile_y = _pvs_min_y + 1; tile_y < _pvs_max_y - 1; ++tile_y)
    {
        for (int tile_x = _pvs_min_x + 1; tile_x < _pvs_max_x - 1; ++tile_x)
        {
            int index = tile_index(tile_x, tile_y);

            if (!_tiles[index] && _visibility[index])
            {
                if (tile_x < view_x)
                {
                    if (tile_y < view_y)
                    {
                        index = tile_index(tile_x - 1, tile_y);
                        if (_tiles[index]) _visibility[index] = true;
                        index = tile_index(tile_x - 1, tile_y - 1);
                        if (_tiles[index]) _visibility[index] = true;
                        index = tile_index(tile_x, tile_y - 1);
                        if (_tiles[index]) _visibility[index] = true;
                    }
                    else if (tile_y == view_y)
                    {
                        index = tile_index(tile_x - 1, tile_y - 1);
                        if (_tiles[index]) _visibility[index] = true;
                        index = tile_index(tile_x - 1, tile_y);
                        if (_tiles[index]) _visibility[index] = true;
                        index = tile_index(tile_x - 1, tile_y + 1);
                        if (_tiles[index]) _visibility[index] = true;
                    }
                    else // tile_y > view_y
                    {
                        index = tile_index(tile_x - 1, tile_y);
                        if (_tiles[index]) _visibility[index] = true;
                        index = tile_index(tile_x - 1, tile_y + 1);
                        if (_tiles[index]) _visibility[index] = true;
                        index = tile_index(tile_x, tile_y + 1);
                        if (_tiles[index]) _visibility[index] = true;
                    }
                }
                else if (tile_x == view_x)
                {
                    if (tile_y < view_y)
                    {
                        index = tile_index(tile_x - 1, tile_y - 1);
                        if (_tiles[index]) _visibility[index] = true;
                        index = tile_index(tile_x, tile_y - 1);
                        if (_tiles[index]) _visibility[index] = true;
                        index = tile_index(tile_x + 1, tile_y - 1);
                        if (_tiles[index]) _visibility[index] = true;
                    }
                    else if (tile_y > view_y)
                    {
                        index = tile_index(tile_x - 1, tile_y + 1);
                        if (_tiles[index]) _visibility[index] = true;
                        index = tile_index(tile_x, tile_y + 1);
                        if (_tiles[index]) _visibility[index] = true;
                        index = tile_index(tile_x + 1, tile_y + 1);
                        if (_tiles[index]) _visibility[index] = true;
                    }
                }
                else // tile_x > view_x
                {
                    if (tile_y < view_y)
                    {
                        index = tile_index(tile_x + 1, tile_y);
                        if (_tiles[index]) _visibility[index] = true;
                        index = tile_index(tile_x + 1, tile_y - 1);
                        if (_tiles[index]) _visibility[index] = true;
                        index = tile_index(tile_x, tile_y - 1);
                        if (_tiles[index]) _visibility[index] = true;
                    }
                    else if (tile_y == view_y)
                    {
                        index = tile_index(tile_x + 1, tile_y - 1);
                        if (_tiles[index]) _visibility[index] = true;
                        index = tile_index(tile_x + 1, tile_y);
                        if (_tiles[index]) _visibility[index] = true;
                        index = tile_index(tile_x + 1, tile_y + 1);
                        if (_tiles[index]) _visibility[index] = true;
                    }
                    else // tile_y > view_y
                    {
                        index = tile_index(tile_x + 1, tile_y);
                        if (_tiles[index]) _visibility[index] = true;
                        index = tile_index(tile_x + 1, tile_y + 1);
                        if (_tiles[index]) _visibility[index] = true;
                        index = tile_index(tile_x, tile_y + 1);
                        if (_tiles[index]) _visibility[index] = true;
                    }
                }
            }
        }
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

// Use Bresenham's algorithm to march along the ray marking tiles visible until
// the end of the ray or a visibility barrier is reached
void Fov::cast_ray(int start_x, int start_y, int end_x, int end_y)
{
    int delta_x = end_x - start_x;
    int delta_y = end_y - start_y;
    int step_x = delta_x > 0 ? 1 : (delta_x < 0 ? -1 : 0);
    int step_y = delta_y > 0 ? 1 : (delta_y < 0 ? -1 : 0);

    if ((delta_x * step_x) > (delta_y * step_y))
    {
        // tile_x-major
        int y = start_y;
        int error = 0;

        for (int x = start_x; x != end_x; x += step_x)
        {
            int index = tile_index(x, y);

            _visibility[index] = true;

            if (_tiles[index])
            {
                return;
            }

            error += step_y * delta_y;

            if ((error * 2) >= step_x * delta_x)
            {
                y += step_y;
                error -= step_x * delta_x;
            }
        }
    }
    else
    {
        // tile_y-major
        int x = start_x;
        int error = 0;

        for (int y = start_y; y != end_y; y += step_y)
        {
            int index = tile_index(x, y);

            _visibility[index] = true;

            if (_tiles[index])
            {
                return;
            }

            error += step_x * delta_x;

            if ((error * 2) >= step_y * delta_y)
            {
                x += step_x;
                error -= step_y * delta_y;
            }
        }
    }
}
