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
        Actor* monster = new Actor(monster_def, *this);
        monster->set_position(monster_def.spawn_position);
        monster->set_speed(monster_def.speed);
        monster->set_thinker(new MeleeMonster(*monster));
        monster->set_fighter(new MeleeFighter(*monster, monster_def.hp, monster_def.power, monster_def.defence));
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

bool Dungeon::move_actor(Actor& actor, const Point& position)
{
    if (get_tile(position.x, position.y) == TileType::Wall)
    {
        return false;
    }

    const Actor* other_actor = get_actor(position);

    if (other_actor && other_actor != &actor)
    {
        return false;
    }

    actor.set_position(position);

    return true;
}

void Dungeon::remove_actor(Actor& actor)
{
    for (auto it = _actors.begin(); it != _actors.end(); ++it)
    {
        if (*it == &actor)
        {
            _actors.erase(it);
            break;
        }
    }

    if (&actor != _player)
    {
        delete &actor;
    }
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

std::vector<Actor*>& Dungeon::get_actors()
{
    return _actors;
}

Player& Dungeon::get_player() const
{
    return *_player;
}

Point Dungeon::get_size() const
{
    return _map_def->size;
}

TileType Dungeon::get_tile(int x, int y) const
{
    return _map_def->tiles[x + y * _map_def->size.x];
}

bool Dungeon::los_check(const Point& from, const Point& to) const
{
    int delta_x = to.x - from.x;
    int delta_y = to.y - from.y;
    int step_x = delta_x > 0 ? 1 : (delta_x < 0 ? -1 : 0);
    int step_y = delta_y > 0 ? 1 : (delta_y < 0 ? -1 : 0);

    if ((delta_x * step_x) > (delta_y * step_y))
    {
        // x-major
        int y = from.y;
        int error = 0;

        for (int x = from.x; x != to.x; x += step_x)
        {
            if (get_tile(x, y) == TileType::Wall)
            {
                return false;
            }

            error += step_y * delta_y;

            if ((error * 2) >= step_x * delta_x)
            {
                y += step_y;
                error -= step_x * delta_x;
            }
        }
    }
    else
    {
        // y-major
        int x = from.x;
        int error = 0;

        for (int y = from.y; y != to.y; y += step_y)
        {
            if (get_tile(x, y) == TileType::Wall)
            {
                return false;
            }

            error += step_x * delta_x;

            if ((error * 2) >= step_y * delta_y)
            {
                x += step_x;
                error -= step_y * delta_y;
            }
        }
    }

    return true;
}
