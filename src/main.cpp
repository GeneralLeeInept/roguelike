#include <BearLibTerminal.h>

#include <algorithm>
#include <memory>
#include <vector>

#include "actor.h"
#include "dungeon.h"
#include "fov.h"
#include "geometry.h"
#include "map_def.h"
#include "map_generator.h"
#include "player.h"
#include "random.h"
#include "renderer.h"

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define STRINGIZE(s_) STRINGIZE2(s_)
#define STRINGIZE2(s_) #s_
#define SCREEN_SIZE STRINGIZE(SCREEN_WIDTH) "x" STRINGIZE(SCREEN_HEIGHT)

MapDef map_def;
Renderer renderer;
Dungeon dungeon;
bool want_exit = false;
Player player;

bool can_walk(const Point& position)
{
    if (map_def.tiles[position.x + position.y * map_def.size.x] == TileType::Wall)
    {
        return false;
    }

    if (position == player.get_position())
    {
        return false;
    }

    if (dungeon.get_actor(position) != nullptr)
    {
        return false;
    }

    return true;
}

void process_input()
{
    int key = terminal_read();

    Point new_position = player.get_position();

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

    if (new_position != player.get_position() && can_walk(new_position))
    {
        player.set_position(new_position);
    }
}

void generate_map()
{
    MapGeneratorParameters generator_parameters = {};
    generator_parameters.map_size = Point(SCREEN_WIDTH, SCREEN_HEIGHT);
    generator_parameters.room_max = 100;
    generator_parameters.room_size_range = Point(7, 9);
    generator_parameters.room_min_spacing = 5;
    generator_parameters.monsters_max_per_room = 5;
    BasicMapGenerator generator;
    generator.generate_map(generator_parameters, map_def);
}

void init_renderer()
{
    renderer.map_create(map_def);

    for (auto& actor : dungeon._actors)
    {
        Renderer::ActorHandle handle = renderer.actor_create(actor.get_def().type, actor.get_position());
        actor.set_renderer_handle(handle);
    }    
}

void run_game()
{
    generate_map();
    dungeon.init(map_def);
    init_renderer();

    player.set_position(map_def.spawn_position);
    player.set_speed(10);
    player.fov.update(player.get_position(), map_def);
    Renderer::ActorHandle handle = renderer.actor_create(ActorType::Player, player.get_position());
    player.set_renderer_handle(handle);

    // Game loop - angband style energy accumulation
    while (!want_exit)
    {
        process_input();

        player.update();

        player.fov.update(player.get_position(), map_def);
        renderer.actor_set_position(player.get_renderer_handle(), player.get_position());

        for (auto& actor : dungeon._actors)
        {
            actor.update();
            renderer.actor_set_position(actor.get_renderer_handle(), actor.get_position());
        }

        renderer.draw_game(player.fov);
        terminal_refresh();
    }
}

int main(int argc, char** argv)
{
    if (!terminal_open())
    {
        return -1;
    }

    renderer.init();

    terminal_set("window: title='Rogue Like One', size=" SCREEN_SIZE);
    terminal_set("font: ../Media/UbuntuMono-R.ttf, size=12");

    run_game();

    terminal_close();

    return 0;
}
