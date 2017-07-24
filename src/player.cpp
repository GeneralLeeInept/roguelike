#include "player.h"

#include "action.h"

ActorDef player_def = { ActorType::Player, Point(), 100 };

Player::Player(Dungeon& dungeon)
    : Actor(player_def, dungeon)
    , _fov(16)
{
}

bool Player::needs_input() const
{
    return !has_next_action();
}

void Player::update()
{
    _fov.update(get_position(), get_dungeon());
}

const Fov& Player::get_fov() const
{
    return _fov;
}
