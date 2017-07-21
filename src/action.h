#pragma once

#include "geometry.h"

class Actor;
class Dungeon;

class Action
{
public:
    Action() = delete;
    Action(Dungeon& dungeon);

    virtual ~Action() = default;

    virtual void execute(Actor* actor) = 0;

protected:
    Dungeon* _dungeon;
};

class MoveAction : public Action
{
public:
    MoveAction(class Dungeon& dungeon, Point new_position);

    void execute(Actor* actor) override;

private:
    Point _new_position;
};
