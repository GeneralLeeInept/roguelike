#include "player.h"

#include "action.h"
#include "dungeon.h"

ActorDef player_def =
{
    ActorType::Player,
    Point(),
    100
};

Player::Player(Dungeon& dungeon)
    : Actor(player_def)
    , _fov(4)
    , _dungeon(&dungeon)
    , _next_action(nullptr)
{
}

void Player::update()
{
    Actor::update();
    _fov.update(get_position(), *_dungeon);
}

const Fov& Player::get_fov()
{
    return _fov;
}

Action* Player::get_next_action()
{
    Action* action = _next_action;
    _next_action = nullptr;
    return action;
}

void Player::set_next_action(Action* action)
{
    delete _next_action;
    _next_action = action;
}
