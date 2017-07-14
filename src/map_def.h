#pragma once
#include <cstdint>
#include <vector>

enum class TileType : int8_t
{
    Floor,
    Wall,
};

struct MapDef
{
    struct Tile
    {
        TileType type;
    };

    int width;
    int height;
    std::vector<Tile> tiles;
};
