#include "thinker.h"

#include "action.h"
#include "actor.h"
#include "dungeon.h"
#include "player.h"

MeleeMonster::MeleeMonster(Actor& owner)
    : _owner(&owner)
    , _state(State::Seeking)
{
}

void MeleeMonster::think()
{
    Point owner_position = _owner->get_position();
    const Player& player = _owner->get_dungeon().get_player();
    Point player_position = player.get_position();

    if (_owner->get_dungeon().los_check(owner_position, player_position))
    {
        _state = State::Tracking;
        _target_location = player_position;
    }

    if (_state == State::Tracking)
    {
        if (_target_location == owner_position)
        {
            _state = State::Seeking;
        }
        else
        {
            Point move = _target_location - owner_position;
            move.x = move.x > 0 ? 1 : (move.x < 0 ? -1 : 0);
            move.y = move.y > 0 ? 1 : (move.y < 0 ? -1 : 0);
            _owner->set_next_action(new MoveAction(_owner->get_dungeon(), owner_position + move));
        }
    }
}
