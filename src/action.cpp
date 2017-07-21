#include "action.h"

#include "dungeon.h"

Action::Action(Dungeon& dungeon)
    : _dungeon(&dungeon)
{
}

MoveAction::MoveAction(class Dungeon& dungeon, Point new_position)
    : Action(dungeon)
    , _new_position(new_position)
{
}

void MoveAction::execute(Actor* actor)
{
    _dungeon->move_actor(actor, _new_position);
}

