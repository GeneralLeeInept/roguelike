#define _CRT_SECURE_NO_WARNINGS

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <BearLibTerminal.h>

int main(int argc, char** argv)
{
    if (!terminal_open())
    {
        return -1;
    }

    terminal_set("window: title='Rogue Like One', size=132x50");
    terminal_set("font: ../Media/UbuntuMono-R.ttf, size=12");

    terminal_refresh();

    bool wantExit = false;

    while (!wantExit)
    {
        terminal_print(1, 1, "@");
        terminal_refresh();

        if (terminal_has_input() && terminal_read() == TK_CLOSE)
        {
            wantExit = true;
        }
    }

    terminal_close();

    return 0;
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    return main(__argc, __argv);
}
