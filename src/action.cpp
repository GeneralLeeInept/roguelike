#include "action.h"

#include "dungeon.h"
#include "player.h"

Action::Action(Dungeon& dungeon)
    : _dungeon(&dungeon)
{
}

Player& Action::get_player()
{
    return _dungeon->get_player();
}

bool Action::move_actor(Actor& actor, const Point& new_position)
{
    if (!_dungeon->move_actor(actor, new_position))
    {
        Actor* opponent = _dungeon->get_actor(new_position);
        if (opponent && opponent != &actor && actor.get_fighter() && opponent->get_fighter())
        {
            actor.get_fighter()->attack(*opponent->get_fighter());
        }
    }
    return true;
}

MoveAction::MoveAction(class Dungeon& dungeon, Point new_position)
    : Action(dungeon)
    , _new_position(new_position)
{
}

void MoveAction::execute(Actor& actor)
{
    move_actor(actor, _new_position);
}

SeekPlayer::SeekPlayer(Dungeon& dungeon)
    : Action(dungeon)
{
}

void SeekPlayer::execute(Actor& actor)
{
    Point target_position = get_player().get_position();
    Point current_position = actor.get_position();
    Point move = target_position - current_position;
    move.x = move.x > 0 ? 1 : (move.x < 0 ? -1 : 0);
    move.y = move.y > 0 ? 1 : (move.y < 0 ? -1 : 0);
    move_actor(actor, current_position + move);
}
