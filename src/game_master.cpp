#include "game_master.h"

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
    if (_next_udpate_actor == _start_turn_sentinel)
    {
        std::vector<Actor*>& actors = _dungeon->get_actors();
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

    while (_next_update_actor < dungeon.get_actors().size())
    {
        Actor* actor = dungeon.get_actors().at(current_actor);

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

        current_actor++;
    }

    if (current_actor >= dungeon.get_actors().size())
    {
        std::vector<Actor*> dead_actors;
        for (auto& actor : dungeon.get_actors())
        {
            if (actor->get_fighter() && actor->get_fighter()->get_hp() < 0)
            {
                dead_actors.push_back(actor);
            }
        }

        for (auto& actor : dead_actors)
        {
            dungeon.remove_actor(*actor);
        }
    }

    // End turn (TODO)
    // * Tick dungeon

    _next_update_actor = _start_turn_sentinel;
}

void GameMaster::on_change_level()
{
    _dungeon.reset(new Dungeon);
    _dungeon->init(*_map_def);
    _next_udpate_actor = _start_turn_sentinel;
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
