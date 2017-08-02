#include "player.h"

#include "action.h"

ActorDef player_def = { ActorType::Player, Point(), 100, 20, 4, 0 };

Player::Player(Dungeon& dungeon)
    : Actor(player_def, dungeon)
    , _fov(16)
{
    set_fighter(new MeleeFighter(*this, player_def.hp, player_def.power, player_def.defence));
}

bool Player::needs_input() const
{
    return get_fighter()->get_hp() >= 0 && !has_next_action();
}

void Player::update()
{
    _fov.update(get_position(), get_dungeon());
}

const Fov& Player::get_fov() const
{
    return _fov;
}
