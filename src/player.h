#pragma once

#include "actor.h"
#include "fov.h"

class Player : public Actor
{
public:
    Player();

    Fov fov;
};

