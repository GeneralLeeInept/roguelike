#include <BearLibTerminal.h>

int main(int argc, char** argv)
{
    if (!terminal_open())
    {
        return -1;
    }

    terminal_set("window: title='Rogue Like One', size=80x25");
    terminal_set("font: ../Media/UbuntuMono-R.ttf, size=12");

    terminal_refresh();

    bool want_exit = false;

    while (!want_exit)
    {
        terminal_put(0, 0, '@');
        terminal_refresh();

        if (terminal_has_input() && terminal_read() == TK_CLOSE)
        {
            want_exit = true;
        }
    }

    terminal_close();

    return 0;
}
