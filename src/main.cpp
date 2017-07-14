#include <BearLibTerminal.h>

#include <memory>
#include <vector>

#include "map_def.h"
#include "random.h"
#include "renderer.h"

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define STRINGIZE(s_) STRINGIZE2(s_)
#define STRINGIZE2(s_) #s_
#define SCREEN_SIZE STRINGIZE(SCREEN_WIDTH) "x" STRINGIZE(SCREEN_HEIGHT)

char* map_data[] = {
    "################################################################################",
    "#                                                                              #",
    "#                                                                              #",
    "#                                                                              #",
    "#                                 #   #                                        #",
    "#                                  # #                                         #",
    "#                                   #                                          #",
    "#                                  # #                                         #",
    "#                                 #   #                                        #",
    "#                                                                              #",
    "#                     #                            #####                       #",
    "#                    ###                           #####                       #",
    "#                   #####                          #####                       #",
    "#                    ###                           #####                       #",
    "#                     #                            #####                       #",
    "#                                                                              #",
    "#                                                                              #",
    "#                               ######                                         #",
    "#                              #      #                                        #",
    "#                             #        #                                       #",
    "#                                                                              #",
    "#                                                                              #",
    "#                                                                              #",
    "#                                                                              #",
    "################################################################################",
};

MapDef map_def;
Renderer renderer;
int player_x;
int player_y;
bool want_exit;
int test_roller = 0;

void process_input()
{
    if (terminal_has_input())
    {
        int key = terminal_read();

        if (key == TK_CLOSE || key == TK_ESCAPE)
        {
            want_exit = true;
        }
        else if (key == TK_UP)
        {
            player_y -= 1;
        }
        else if (key == TK_DOWN)
        {
            player_y += 1;
        }
        else if (key == TK_LEFT)
        {
            player_x -= 1;
        }
        else if (key == TK_RIGHT)
        {
            player_x += 1;
        }
        else if (key >= TK_2 && key <= TK_6)
        {
            test_roller = (key - TK_2 + 2) * 2;
        }
        else if (key == TK_0)
        {
            test_roller = 20;
        }
        else if (key == TK_1)
        {
            test_roller = 100;
        }
    }
}

void init_map()
{
    map_def.width = static_cast<int>(strlen(map_data[0]));
    map_def.height = sizeof(map_data) / sizeof(map_data[0]);
    map_def.tiles.resize(map_def.width * map_def.height);

    for (int y = 0; y < map_def.height; ++y)
    {
        for (int x = 0; x < map_def.width; ++x)
        {
            map_def.tiles[x + y * map_def.width].type = map_data[y][x] == ' ' ? TileType::Floor : TileType::Wall;
        }
    }

    renderer.map_create(map_def);
}

int main(int argc, char** argv)
{
    if (!terminal_open())
    {
        return -1;
    }

    terminal_set("window: title='Rogue Like One', size=" SCREEN_SIZE);
    terminal_set("font: ../Media/UbuntuMono-R.ttf, size=12");

    init_map();

    player_x = map_def.width / 2;
    player_y = map_def.height / 2;
    want_exit = false;

    Renderer::ActorHandle player = renderer.actor_create('@', color_from_name("white"));

    while (!want_exit)
    {
        renderer.actor_set_position(player, player_x, player_y);
        renderer.draw_game();
        terminal_refresh();
        process_input();

        if (test_roller)
        {
            Roller die(test_roller);
            die.test();
            test_roller = 0;
        }
    }

    terminal_close();

    return 0;
}
