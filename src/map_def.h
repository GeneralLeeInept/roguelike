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

enum class MonsterType : int8_t
{
    Player,
    Weak,
    Strong
};

struct MapDef
{
    struct Tile
    {
        TileType type;
    };

    struct Monster
    {
        MonsterType type;
        Point spawn_pos;
    };

    Point size;
    Point spawn_position;
    std::vector<Tile> tiles;
    std::vector<Monster> monsters;
};
