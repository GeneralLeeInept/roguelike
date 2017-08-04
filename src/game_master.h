#pragma once

#include <memory>

class Dungeon;
class Player;

struct MapDef;

class GameMaster
{
public:
    void new_campaign(const char* save_file);
    void load_campaign(const char* save_file);
    void save_campaign();

    void update();

private:
    void go_to_town();
    void on_change_level();

    std::unique_ptr<MapDef> _map_def;
    std::unique_ptr<Dungeon> _dungeon;
    std::unique_ptr<Player> _player;
    size_t _next_udpate_actor;
};