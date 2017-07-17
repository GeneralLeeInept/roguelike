#pragma once
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

    int width;
    int height;
    int spawn_x;
    int spawn_y;
    std::vector<Tile> tiles;
};
