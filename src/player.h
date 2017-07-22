#pragma once

#include "actor.h"
#include "fov.h"

class Dungeon;

class Player : public Actor
{
public:
    Player(Dungeon& dungeon);

    void update() override;

    const Fov& get_fov();
    Action* get_next_action() override;

    void set_next_action(Action* action);

private:
    Fov _fov;
    Dungeon* _dungeon;
    Action* _next_action;
};

