#include "dungeon.h"

void Dungeon::init(const MapDef& map_def)
{
    _map_def = &map_def;

    for (auto& monster_def : map_def.actors)
    {
        Actor monster(monster_def);
        monster.set_position(monster_def.spawn_position);
        monster.set_speed(monster_def.speed);
        _actors.push_back(monster);
    }
}

Actor* Dungeon::get_actor(const Point& position)
{
    for (auto& actor : _actors)
    {
        if (actor.get_position() == position)
        {
            return &actor;
        }
    }

    return nullptr;
}
