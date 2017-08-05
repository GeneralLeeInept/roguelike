#include <BearLibTerminal.h>

#include <algorithm>
#include <memory>
#include <vector>

#include "action.h"
#include "actor.h"
#include "dungeon.h"
#include "game_master.h"
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

Renderer renderer;
GameMaster game_master;
bool want_exit = false;
Renderer::StatusBarHandle status_bar_hp;

void process_input()
{
    if (terminal_has_input())
    {
        int key = terminal_read();

        Player& player = game_master.get_player();
        Point player_position = player.get_position();
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
            player.set_next_action(new MoveAction(player.get_dungeon(), player_position));
        }

        if (new_position != player_position)
        {
            player.set_next_action(new MoveAction(player.get_dungeon(), new_position));
        }
    }
}

void init_renderer()
{
    status_bar_hp = renderer.status_bar_create(Rectangle(Point(2, 2), Point(24, 3)), 0, 20);
}

void level_change_handler()
{
    renderer.map_create(game_master.get_map_def());

    for (auto& actor : game_master.get_dungeon().get_actors())
    {
        Renderer::ActorHandle renderer_handle = renderer.actor_create(actor->get_def().type, actor->get_position());
        actor->set_renderer_handle(renderer_handle);
    }
}

void run_game()
{
    init_renderer();

    level_change_handler();

    // Game loop
    while (!want_exit)
    {
        process_input();

        game_master.update();

        Player& player = game_master.get_player();
        player.get_fov().update(player.get_position(), player.get_dungeon());
        renderer.status_bar_set_value(status_bar_hp, player.get_fighter()->get_hp());

        for (auto& actor : game_master.get_dungeon().get_actors())
        {
            Renderer::ActorHandle renderer_handle = actor->get_renderer_handle();

            if (actor->get_fighter() == nullptr || actor->get_fighter()->is_alive())
            {
                renderer.actor_set_position(renderer_handle, actor->get_position());
            }
            else
            {
                renderer.actor_destroy(renderer_handle);
                actor->set_renderer_handle(0);
            }
        }

        renderer.draw_game(game_master.get_dungeon().get_player().get_fov(), game_master.get_dungeon().get_player().get_position());
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

    game_master.new_campaign(nullptr);

    run_game();

    terminal_close();

    return 0;
}
