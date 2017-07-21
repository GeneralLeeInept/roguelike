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

bool Dungeon::move_actor(Actor* actor, const Point& position)
{
    if (_map_def->tiles[position.x + position.y * _map_def->size.x] == TileType::Wall)
    {
        return false;
    }

    const Actor* other_actor = get_actor(position);
    
    if (other_actor && other_actor != actor)
    {
        return false;
    }

    actor->set_position(position);

    return true;
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
