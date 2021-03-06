#pragma once

#include "geometry.h"
#include "map_def.h"

#include <vector>

class Actor;
class Player;

class Dungeon
{
public:
    ~Dungeon();

    void init(const MapDef& map_def);
    void shutdown();

    bool move_actor(Actor& actor, const Point& position);
    void remove_actor(Actor& actor);

    Actor* get_actor(const Point& position) const;
    std::vector<Actor*>& get_actors();
    Player& get_player() const;
    Point get_size() const;
    TileType get_tile(int x, int y) const;

    bool los_check(const Point& from, const Point& to) const;

private:
    const MapDef* _map_def;
    Player* _player;
    std::vector<Actor*> _actors;
};