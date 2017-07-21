#pragma once

#include "actor.h"
#include "geometry.h"
#include "map_def.h"

#include <vector>

class Dungeon
{
public:
    void init(const MapDef& map_def);

    bool move_actor(Actor* actor, const Point& position);
    Actor* get_actor(const Point& position);

    const MapDef* _map_def;
    std::vector<Actor> _actors;
};