#include <BearLibTerminal.h>

#include <memory>
#include <vector>

#include "fov.h"
#include "geometry.h"
#include "map_def.h"
#include "map_generator.h"
#include "random.h"
#include "renderer.h"

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define STRINGIZE(s_) STRINGIZE2(s_)
#define STRINGIZE2(s_) #s_
#define SCREEN_SIZE STRINGIZE(SCREEN_WIDTH) "x" STRINGIZE(SCREEN_HEIGHT)

class Player
{
public:
    Player()
        : fov(4)
    {
    }

    Renderer::ActorHandle renderer_handle;
    Point position;
    Fov fov;
};

struct Actor
{
    Renderer::ActorHandle renderer_handle;
    ActorType type;
    Point position;
};

MapDef map_def;
Renderer renderer;
bool want_exit;
Player player;
std::vector<Actor> actorsparamLayout_onRemoveContainer;

bool can_walk(const Point& position)
{
    return map_def.tiles[position.x + position.y * map_def.size.x].type == TileType::Floor;
}

void process_input()
{
    if (terminal_has_input())
    {
        int key = terminal_read();

        Point new_position = player.position;

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

        if (new_position != player.position && can_walk(new_position))
        {
            player.position = new_position;
        }
    }
}

void init_map()
{
    MapGeneratorParameters generator_parameters = {};
    generator_parameters.map_size = Point(SCREEN_WIDTH, SCREEN_HEIGHT);
    generator_parameters.room_max = 100;
    generator_parameters.room_size_range = Point(7, 9);
    generator_parameters.room_min_spacing = 5;
    generator_parameters.monsters_max_per_room = 5;
    BasicMapGenerator generator;
    generator.generate_map(generator_parameters, map_def);
    renderer.map_create(map_def);
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

    init_map();

    for (auto& monster_def : map_def.actors)
    {
        Actor monster;
        monster.type = monster_def.type;
        monster.position = monster_def.spawn_pos;
        monster.renderer_handle = renderer.actor_create(monster.type, monster.position);
    }

    player.position = map_def.spawn_position;
    player.renderer_handle = renderer.actor_create(ActorType::Player, player.position);

    want_exit = false;

    while (!want_exit)
    {
        renderer.actor_set_position(player.renderer_handle, player.position);
        player.fov.update(player.position, map_def);
        renderer.draw_game(player.fov);
        terminal_refresh();
        process_input();
    }

    terminal_close();

    return 0;
}
