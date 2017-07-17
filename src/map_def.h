#pragma once
#include "geometry.h"

#include <cstdint>
#include <vector>

enum class TileType : int8_t
{
    Empty,
    Floor,
    Wall
};

struct MapDef
{
    struct Tile
    {
        TileType type;
    };

    Point size;
    Point spawn_position;
    std::vector<Tile> tiles;
};
