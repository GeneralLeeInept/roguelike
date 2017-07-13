#include <BearLibTerminal.h>

#include <memory>

#define SCREEN_SIZE "80x25"

struct TileDef
{
    int type;
};

struct MapDef
{
    int width;
    int height;
    TileDef* tiles;
};

class DrawMap
{
public:
    DrawMap(int mapLayer)
        : mMapLayer(mapLayer)
    {
    }

    ~DrawMap() { delete[] mTiles; }

    bool initialise(const MapDef& def)
    {
        mWidth = def.width;
        mHeight = def.height;
        mTiles = new Tile[mWidth * mHeight];

        for (int y = 0; y < mHeight; ++y)
        {
            for (int x = 0; x < mWidth; ++x)
            {
                Tile& tile = tile_at(x, y);

                if (def.tiles[x + y * def.width].type == 0)
                {
                    tile.code = '.';
                    tile.colour = color_from_argb(0xff, 0x80, 0x80, 0x80);
                }
                else
                {
                    tile.code = '#';
                    tile.colour = color_from_argb(0xff, 0x80, 0x80, 0x80);
                }
            }
        }

        return true;
    }

    void render()
    {
        int restoreLayer = terminal_state(TK_LAYER);
        terminal_layer(mMapLayer);
        terminal_clear_area(0, 0, 80, 25);

        for (int y = 0; y < mHeight; ++y)
        {
            for (int x = 0; x < mWidth; ++x)
            {
                Tile& tile = tile_at(x, y);
                terminal_color(tile.colour);
                terminal_put(x, y, tile.code);
            }
        }

        terminal_layer(restoreLayer);
    }

private:
    struct Tile
    {
        color_t colour;
        int code;
    };

    int mMapLayer;
    int mWidth;
    int mHeight;
    Tile* mTiles;

    Tile& tile_at(int x, int y) { return mTiles[x + y * mWidth]; }
};

char map_data[] = {
    "####################################################"
    "#                                                  #"
    "#                                                  #"
    "#             #                 ###                #"
    "#            ###                ###                #"
    "#             #                 ###                #"
    "#                                                  #"
    "#                                                  #"
    "####################################################"
};

int player_x;
int player_y;
bool want_exit;
DrawMap draw_map(1);

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

void init_map()
{
    TileDef tiles[80 * 25];
    memset(tiles, 0, 80 * 25 * sizeof(TileDef));

    int source_width = sizeof(map_data) / 9;
    int source_height = 9;
    int y = (25 - source_height) / 2;

    for (int h = 0; h < source_height; ++h, ++y)
    {
        int x = (80 - source_width) / 2;

        for (int w = 0; w < source_width; ++w, ++x)
        {
            tiles[x + y * 80].type = map_data[w + h * source_width] == ' ' ? 0 : 1;
        }
    }

    MapDef map_def;
    map_def.width = 80;
    map_def.height = 25;
    map_def.tiles = tiles;
    draw_map.initialise(map_def);
}

int main(int argc, char** argv)
{
    if (!terminal_open())
    {
        return -1;
    }

    init_map();

    terminal_set("window: title='Rogue Like One', size=" SCREEN_SIZE);
    terminal_set("font: ../Media/UbuntuMono-R.ttf, size=12");

    player_x = 40;
    player_y = 12;
    want_exit = false;

    while (!want_exit)
    {
        draw_map.render();

        terminal_layer(0);
        terminal_clear_area(0, 0, 80, 25);
        terminal_color("white");
        terminal_put(player_x, player_y, '@');

        terminal_refresh();
        process_input();
    }

    terminal_close();

    return 0;
}
