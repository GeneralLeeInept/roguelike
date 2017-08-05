#pragma once

#include "actor.h"
#include "fov.h"

class Dungeon;

class Player : public Actor
{
public:
    Player(Dungeon& dungeon);

    Fov& get_fov();
    bool needs_input() const override;

private:
    Fov _fov;
};

