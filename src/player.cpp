#include "player.h"

ActorDef player_def =
{
    ActorType::Player,
    Point(),
    10
};

Player::Player()
    : Actor(player_def)
    , fov(4)
{
}