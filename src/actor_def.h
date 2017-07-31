#pragma once

#include "geometry.h"

enum class ActorType : int8_t
{
    Player,
    WeakMonster,
    StrongMonster
};

struct ActorDef
{
    ActorType type;
    Point spawn_position;
    int speed;
    int hp;
    int power;
    int defence;
};

