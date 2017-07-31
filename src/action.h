#pragma once

#include "geometry.h"

class Actor;
class Dungeon;
class Player;

class Action
{
public:
    Action() = delete;
    Action(Dungeon& dungeon);

    virtual ~Action() = default;

    virtual void execute(Actor& actor) = 0;

protected:
    Player& get_player();
    bool move_actor(Actor& actor, const Point& new_position);

private:
    Dungeon* _dungeon;
};

class MoveAction : public Action
{
public:
    MoveAction(Dungeon& dungeon, Point new_position);

    void execute(Actor& actor) override;

private:
    Point _new_position;
};

class SeekPlayer : public Action
{
public:
    SeekPlayer(Dungeon& dungeon);

    void execute(Actor& actor) override;
};