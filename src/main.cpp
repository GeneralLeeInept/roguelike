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
Renderer::StatusBarHandle status_bar_hp;

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
    status_bar_hp = renderer.status_bar_create(Rectangle(Point(2, 2), Point(24, 3)), 0, 20);
}

void run_game()
{
    init_renderer();

    generate_map();
    dungeon.init(map_def);

    // Game loop
    while (!want_exit)
    {
        process_input();

        // game master update..

        dungeon.get_player().update();
        renderer.status_bar_set_value(status_bar_hp, dungeon.get_player().get_fighter()->get_hp());

        for (auto& actor : dungeon.get_actors())
        {
            Renderer::ActorHandle renderer_handle = actor->get_renderer_handle();

            if (actor->get_fighter() == nullptr || actor->get_fighter()->is_alive())
            {
                if (renderer_handle == 0)
                {
                    Renderer::ActorHandle handle = renderer.actor_create(actor->get_def().type, actor->get_position());
                    actor->set_renderer_handle(renderer_handle);
                }

                renderer.actor_set_position(renderer_handle, actor->get_position());
            }
            else
            {
                renderer.actor_destroy(renderer_handle);
                actor->set_renderer_handle(0);
            }
        }

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
