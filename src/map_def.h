#pragma once
#include "actor.h"
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
    Point size;
    Point spawn_position;
    std::vector<TileType> tiles;
    std::vector<ActorDef> actors;
};
