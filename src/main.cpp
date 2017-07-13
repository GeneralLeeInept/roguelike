#include <BearLibTerminal.h>

#define SCREEN_SIZE "80x25"

int player_x;
int player_y;
bool want_exit;

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
    }
}

int main(int argc, char** argv)
{
    if (!terminal_open())
    {
        return -1;
    }

    terminal_set("window: title='Rogue Like One', size=" SCREEN_SIZE);
    terminal_set("font: ../Media/UbuntuMono-R.ttf, size=12");

    player_x = 40;
    player_y = 12;
    want_exit = false;

    while (!want_exit)
    {
        terminal_put(player_x, player_y, '@');
        terminal_refresh();
        terminal_put(player_x, player_y, ' ');
        process_input();
    }

    terminal_close();

    return 0;
}
