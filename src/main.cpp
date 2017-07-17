#include <BearLibTerminal.h>

#include <memory>
#include <vector>

#include "fov.h"
#include "map_def.h"
#include "map_generator.h"
#include "random.h"
#include "renderer.h"

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define STRINGIZE(s_) STRINGIZE2(s_)
#define STRINGIZE2(s_) #s_
#define SCREEN_SIZE STRINGIZE(SCREEN_WIDTH) "x" STRINGIZE(SCREEN_HEIGHT)

MapDef map_def;
Renderer renderer;
Fov fov(SCREEN_WIDTH);
int player_x;
int player_y;
bool want_exit;

bool can_walk(int x, int y)
{
    return map_def.tiles[x + y * map_def.width].type == TileType::Floor;
}

void process_input()
{
    if (terminal_has_input())
    {
        int key = terminal_read();

        int new_x = player_x;
        int new_y = player_y;

        if (key == TK_CLOSE || key == TK_ESCAPE)
        {
            want_exit = true;
        }
        else if (key == TK_UP || key == TK_KP_8)
        {
            new_y -= 1;
        }
        else if (key == TK_PAGEUP || key == TK_KP_9)
        {
            new_y -= 1;
            new_x += 1;
        }
        else if (key == TK_RIGHT || key == TK_KP_6)
        {
            new_x += 1;
        }
        else if (key == TK_PAGEDOWN || key == TK_KP_3)
        {
            new_y += 1;
            new_x += 1;
        }
        else if (key == TK_DOWN || key == TK_KP_2)
        {
            new_y += 1;
        }
        else if (key == TK_END || key == TK_KP_1)
        {
            new_y += 1;
            new_x -= 1;
        }
        else if (key == TK_LEFT || key == TK_KP_4)
        {
            new_x -= 1;
        }
        else if (key == TK_HOME || key == TK_KP_7)
        {
            new_y -= 1;
            new_x -= 1;
        }

        if ((new_x != player_x || new_y != player_y) && can_walk(new_x, new_y))
        {
            player_x = new_x;
            player_y = new_y;
        }
    }
}

void init_map()
{
    BasicMapGenerator generator;
    generator.configure(100, 5, 7);
    generator.generate_map(SCREEN_WIDTH, SCREEN_HEIGHT, map_def);
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

    player_x = map_def.spawn_x;
    player_y = map_def.spawn_y;
    want_exit = false;

    Renderer::ActorHandle player = renderer.actor_create('@', color_from_name("white"));

    while (!want_exit)
    {
        renderer.actor_set_position(player, player_x, player_y);
        fov.update(player_x, player_y, map_def);
        renderer.draw_game(fov);
        terminal_refresh();
        process_input();
    }

    terminal_close();

    return 0;
}
