#include "game_master.h"

#include "actor.h"
#include "dungeon.h"
#include "map_generator.h"

static const size_t _start_turn_sentinel = (size_t)-1;

// Start a new campaign
void GameMaster::new_campaign(const char* save_file)
{
    // Roll player character (todo)

    // Put the player in the town
    go_to_town();
}

// Continue a campaign from a saved state
void GameMaster::load_campaign(const char* save_file)
{
    // TODO
}

// Save the current campaign state
void GameMaster::save_campaign()
{
    // TODO
}

// Advance the campaign
void GameMaster::update()
{
    std::vector<Actor*>& actors = _dungeon->get_actors();

    if (_next_update_actor == _start_turn_sentinel)
    {
        actors.erase(
                std::remove_if(actors.begin(), actors.end(),
                               [](Actor* actor) -> bool { return actor->get_fighter() != nullptr && actor->get_fighter()->is_alive() == false; }),
                actors.end());

        // Start turn (TODO)
        // * Everyone accumulates energy
        // * Everyone thinks (await player input)
        // * Everyone with enough energy to act rolls for initiative (?)
        // * Sort everyone with enough energy to act according to their
        //   initiative
        // * Everyone _tries_ to act - if you take your turn late your
        //   move might get screwed up by an early mover

        _next_update_actor = 0;
    }

    while (_next_update_actor < _dungeon->get_actors().size())
    {
        Actor* actor = actors.at(_next_update_actor);

        if (actor->can_act())
        {
            if (actor->needs_input())
            {
                return;
            }

            actor->act();
        }
        else
        {
            actor->gain_energy();
        }

        _next_update_actor++;
    }

    if (_next_update_actor >= actors.size())
    {
        // End turn (TODO)
        // * Tick dungeon
    }

    _next_update_actor = _start_turn_sentinel;
}

Dungeon& GameMaster::get_dungeon()
{
    return *_dungeon;
}

MapDef& GameMaster::get_map_def()
{
    return *_map_def;
}

Player& GameMaster::get_player()
{
    return _dungeon->get_player();
}

void GameMaster::on_change_level()
{
    _dungeon.reset(new Dungeon);
    _dungeon->init(*_map_def);
    _next_update_actor = _start_turn_sentinel;
    save_campaign();
}

void GameMaster::go_to_town()
{
    TownMapGenerator generator;
    MapGeneratorParameters parameters;
    _map_def.reset(new MapDef);
    generator.generate_map(parameters, *_map_def);
    on_change_level();
}
