#include <BearLibTerminal.h>

#include <algorithm>
#include <memory>
#include <vector>

#include "action.h"
#include "actor.h"
#include "dungeon.h"
#include "fov.h"
#include "geometry.h"
#include "map_def.h"
#include "map_generator.h"
#include "player.h"
#include "random.h"
#include "renderer.h"

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 50
#define STRINGIZE(s_) STRINGIZE2(s_)
#define STRINGIZE2(s_) #s_
#define SCREEN_SIZE STRINGIZE(SCREEN_WIDTH) "x" STRINGIZE(SCREEN_HEIGHT)

MapDef map_def;
Renderer renderer;
Dungeon dungeon;
bool want_exit = false;

void process_input()
{
    if (terminal_has_input())
    {
        int key = terminal_read();

        Point player_position = dungeon.get_player().get_position();
        Point new_position = player_position;

        if (key == TK_CLOSE || key == TK_ESCAPE)
        {
            want_exit = true;
        }
        else if (key == TK_UP || key == TK_KP_8)
        {
            new_position.y -= 1;
        }
        else if (key == TK_PAGEUP || key == TK_KP_9)
        {
            new_position.y -= 1;
            new_position.x += 1;
        }
        else if (key == TK_RIGHT || key == TK_KP_6)
        {
            new_position.x += 1;
        }
        else if (key == TK_PAGEDOWN || key == TK_KP_3)
        {
            new_position.y += 1;
            new_position.x += 1;
        }
        else if (key == TK_DOWN || key == TK_KP_2)
        {
            new_position.y += 1;
        }
        else if (key == TK_END || key == TK_KP_1)
        {
            new_position.y += 1;
            new_position.x -= 1;
        }
        else if (key == TK_LEFT || key == TK_KP_4)
        {
            new_position.x -= 1;
        }
        else if (key == TK_HOME || key == TK_KP_7)
        {
            new_position.y -= 1;
            new_position.x -= 1;
        }
        else if (key == TK_KP_5 || key == TK_COMMA)
        {
            dungeon.get_player().set_next_action(new MoveAction(dungeon, player_position));
        }

        if (new_position != player_position)
        {
            dungeon.get_player().set_next_action(new MoveAction(dungeon, new_position));
        }
    }
}

void generate_map()
{
    MapGeneratorParameters generator_parameters = {};
    generator_parameters.map_size = Point(100, 100);
    generator_parameters.room_max = 100;
    generator_parameters.room_size_range = Point(5, 14);
    generator_parameters.room_min_spacing = 7;
    generator_parameters.monsters_max_per_room = 5;
    //BasicMapGenerator generator;
    TestMapGenerator generator;
    generator.generate_map(generator_parameters, map_def);
}

void init_renderer()
{
    renderer.map_create(map_def);

    for (auto& actor : dungeon.get_actors())
    {
        Renderer::ActorHandle handle = renderer.actor_create(actor->get_def().type, actor->get_position());
        actor->set_renderer_handle(handle);
    }
}

void run_game()
{
    generate_map();
    dungeon.init(map_def);
    init_renderer();

    // Game loop
    size_t current_actor = 0;

    while (!want_exit)
    {
        process_input();

        // Start turn (TODO)
        // * Everyone accumulates energy
        // * Everyone thinks (await player input)
        // * Everyone with enough energy to act rolls for initiative (?)
        // * Sort everyone with enough energy to act according to their
        //   initiative
        // * Everyone _tries_ to act - if you take your turn late your
        //   move might get screwed up by an early mover

        for (; current_actor < dungeon.get_actors().size(); ++current_actor)
        {
            Actor* actor = dungeon.get_actors().at(current_actor);

            if (actor->can_act())
            {
                if (actor->needs_input())
                {
                    break;
                }

                actor->act();
                renderer.actor_set_position(actor->get_renderer_handle(), actor->get_position());
            }
            else
            {
                actor->gain_energy();
            }
        }

        if (current_actor >= dungeon.get_actors().size())
        {
            std::vector<Actor*> dead_actors;
            for (auto& actor : dungeon.get_actors())
            {
                if (actor->get_fighter() && actor->get_fighter()->get_hp() <= 0)
                {
                    dead_actors.push_back(actor);
                }
            }

            for (auto& actor : dead_actors)
            {
                renderer.actor_destroy(actor->get_renderer_handle());
                dungeon.remove_actor(*actor);
            }

            // End turn (TODO)
            // * Tick dungeon

            current_actor = 0;
        }

        dungeon.get_player().update();
        renderer.draw_game(dungeon.get_player().get_fov(), dungeon.get_player().get_position());
        terminal_refresh();
    }
}

int main(int argc, char** argv)
{
    if (!terminal_open())
    {
        return -1;
    }

    terminal_refresh();

    renderer.init();

    terminal_set("window: title='Rogue Like One', size=" SCREEN_SIZE);
    terminal_set("font: ../Media/UbuntuMono-R.ttf, size=12");

    run_game();

    terminal_close();

    return 0;
}
