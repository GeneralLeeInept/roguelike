#include "dungeon.h"

#include "actor.h"
#include "player.h"

Dungeon::~Dungeon()
{
    shutdown();
}

void Dungeon::init(const MapDef& map_def)
{
    _map_def = &map_def;

    _player = new Player(*this);
    _player->set_position(map_def.spawn_position);
    _actors.push_back(_player);

    for (auto& monster_def : map_def.actors)
    {
        Actor* monster = new Actor(monster_def);
        monster->set_position(monster_def.spawn_position);
        monster->set_speed(monster_def.speed);
        _actors.push_back(monster);
    }
}

void Dungeon::shutdown()
{
    for (auto& actor : _actors)
    {
        delete actor;
    }

    _actors.clear();
    _player = nullptr;
}

bool Dungeon::move_actor(Actor* actor, const Point& position)
{
    if (get_tile(position.x, position.y) == TileType::Wall)
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

Actor* Dungeon::get_actor(const Point& position) const
{
    for (auto& actor : _actors)
    {
        if (actor->get_position() == position)
        {
            return actor;
        }
    }

    return nullptr;
}

const std::vector<Actor*>& Dungeon::get_actors() const
{
    return _actors;
}

Player* Dungeon::get_player() const
{
    return _player;
}

Point Dungeon::get_size() const
{
    return _map_def->size;
}

TileType Dungeon::get_tile(int x, int y) const
{
    return _map_def->tiles[x + y * _map_def->size.x];
}
