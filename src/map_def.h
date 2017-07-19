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

enum class ActorType : int8_t
{
    Player,
    WeakMonster,
    StrongMonster
};

struct MapDef
{
    struct Tile
    {
        TileType type;
    };

    struct Actor
    {
        ActorType type;
        Point spawn_pos;
    };

    Point size;
    Point spawn_position;
    std::vector<Tile> tiles;
    std::vector<Actor> actors;
};
