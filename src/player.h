#pragma once

#include "actor.h"
#include "fov.h"

class Dungeon;

class Player : public Actor
{
public:
    Player(Dungeon& dungeon);

    const Fov& get_fov() const;
    bool needs_input() const override;
    void update();

private:
    Fov _fov;
};

