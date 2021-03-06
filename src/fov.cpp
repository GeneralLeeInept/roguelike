﻿#include "fov.h"

#include "dungeon.h"

#include <algorithm>

Fov::Fov(int range)
    : _range(range)
{
    int max_tiles = (range * 2) * (range * 2);
    _tiles.resize(max_tiles);
    _visibility.resize(max_tiles, true);
}

void Fov::update(const Point& view_position, const Dungeon& dungeon)
{
    _pvs = Rectangle::intersection(Rectangle(Point(0, 0), dungeon.get_size()), Rectangle(view_position, _range * 2, _range * 2));

    for (int tile_y = _pvs.mins.y; tile_y < _pvs.maxs.y; ++tile_y)
    {
        for (int tile_x = _pvs.mins.x; tile_x < _pvs.maxs.x; ++tile_x)
        {
            int index = tile_index(tile_x, tile_y);
            _tiles[index] = (dungeon.get_tile(tile_x, tile_y) == TileType::Wall);
        }
    }

    for (auto& tile : _visibility)
    {
        tile = false;
    }

    for (int y = _pvs.mins.y; y < _pvs.maxs.y; ++y)
    {
        for (int x = _pvs.mins.x; x < _pvs.maxs.x; ++x)
        {
            int index = tile_index(x, y);
            _visibility[index] = dungeon.los_check(view_position, Point(x, y));
        }
    }

    // Apply jice's post-process to fix artefacts
    for (int tile_y = _pvs.mins.y + 1; tile_y < _pvs.maxs.y - 1; ++tile_y)
    {
        for (int tile_x = _pvs.mins.x + 1; tile_x < _pvs.maxs.x - 1; ++tile_x)
        {
            int index = tile_index(tile_x, tile_y);

            if (!_tiles[index] && _visibility[index])
            {
                if (tile_x < view_position.x)
                {
                    if (tile_y < view_position.y)
                    {
                        index = tile_index(tile_x - 1, tile_y);
                        if (_tiles[index]) _visibility[index] = true;
                        index = tile_index(tile_x - 1, tile_y - 1);
                        if (_tiles[index]) _visibility[index] = true;
                        index = tile_index(tile_x, tile_y - 1);
                        if (_tiles[index]) _visibility[index] = true;
                    }
                    else if (tile_y == view_position.y)
                    {
                        index = tile_index(tile_x - 1, tile_y - 1);
                        if (_tiles[index]) _visibility[index] = true;
                        index = tile_index(tile_x - 1, tile_y);
                        if (_tiles[index]) _visibility[index] = true;
                        index = tile_index(tile_x - 1, tile_y + 1);
                        if (_tiles[index]) _visibility[index] = true;
                    }
                    else // tile_y > view_position.y
                    {
                        index = tile_index(tile_x - 1, tile_y);
                        if (_tiles[index]) _visibility[index] = true;
                        index = tile_index(tile_x - 1, tile_y + 1);
                        if (_tiles[index]) _visibility[index] = true;
                        index = tile_index(tile_x, tile_y + 1);
                        if (_tiles[index]) _visibility[index] = true;
                    }
                }
                else if (tile_x == view_position.x)
                {
                    if (tile_y < view_position.y)
                    {
                        index = tile_index(tile_x - 1, tile_y - 1);
                        if (_tiles[index]) _visibility[index] = true;
                        index = tile_index(tile_x, tile_y - 1);
                        if (_tiles[index]) _visibility[index] = true;
                        index = tile_index(tile_x + 1, tile_y - 1);
                        if (_tiles[index]) _visibility[index] = true;
                    }
                    else if (tile_y > view_position.y)
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
                    if (tile_y < view_position.y)
                    {
                        index = tile_index(tile_x + 1, tile_y);
                        if (_tiles[index]) _visibility[index] = true;
                        index = tile_index(tile_x + 1, tile_y - 1);
                        if (_tiles[index]) _visibility[index] = true;
                        index = tile_index(tile_x, tile_y - 1);
                        if (_tiles[index]) _visibility[index] = true;
                    }
                    else if (tile_y == view_position.y)
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

bool Fov::can_see(const Point& position) const
{
    int index = tile_index(position.x, position.y);

    if (index < 0)
    {
        return false;
    }

    return _visibility[index];
}

int Fov::tile_index(int x, int y) const
{
    if (x < _pvs.mins.x || x >= _pvs.maxs.x || y < _pvs.mins.y || y >= _pvs.maxs.y)
    {
        return -1;
    }

    x -= _pvs.mins.x;
    y -= _pvs.mins.y;

    return x + y * _range * 2;
}

// Use Bresenham's algorithm to march along the ray marking tiles visible until
// the end of the ray or a visibility barrier is reached
void Fov::cast_ray(const Point& from, const Point& to)
{
    int delta_x = to.x - from.x;
    int delta_y = to.y - from.y;
    int step_x = delta_x > 0 ? 1 : (delta_x < 0 ? -1 : 0);
    int step_y = delta_y > 0 ? 1 : (delta_y < 0 ? -1 : 0);

    if ((delta_x * step_x) > (delta_y * step_y))
    {
        // x-major
        int y = from.y;
        int error = 0;

        for (int x = from.x; x != to.x; x += step_x)
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
        // y-major
        int x = from.x;
        int error = 0;

        for (int y = from.y; y != to.y; y += step_y)
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
