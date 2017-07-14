#include <BearLibTerminal.h>

#include <memory>
#include <vector>

#include "map_def.h"

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define STRINGIZE(x__) #x__
#define SCREEN_SIZE STRINGIZE(SCREEN_WIDTH) "x" STRINGIZE(SCREEN_HEIGHT)

class DrawMap
{
public:
    DrawMap(int mapLayer)
        : mMapLayer(mapLayer)
    {
    }

    bool initialise(const MapDef& def)
    {
        mWidth = def.width;
        mHeight = def.height;
        mTiles.reset(new Tile[mWidth * mHeight]);

        for (int y = 0; y < mHeight; ++y)
        {
            for (int x = 0; x < mWidth; ++x)
            {
                Tile& tile = tile_at(x, y);

                switch (def.tiles[x + y * def.width].type)
                {
                    case 0:
                    {
                        tile.code = ' ';
                        break;
                    }
                    case 1:
                    {
                        tile.code = '.';
                        tile.colour = color_from_argb(0xff, 0xA0, 0xA0, 0xA0);
                        break;
                    }
                    case 2:
                    {
                        tile.code = '#';
                        tile.colour = color_from_argb(0xff, 0xff, 0xff, 0xff);
                        break;
                    }
                    default:
                    {
                        tile.code = '?';
                        tile.colour = color_from_argb(0xff, 0xff, 0x00, 0x00);
                        break;
                    }
                }
            }
        }

        return true;
    }
    
    typedef size_t ActorHandle;

    ActorHandle create_actor(int code, color_t colour, int x, int y)
    {
        Actor *actor = nullptr;
        size_t index;

        for (index = 0; index < mActors.size(); ++index)
        {
            if (!mActors[index].in_use)
            {
                actor = &mActors[index];
                break;
            }
        }

        if (actor == nullptr)
        {
            mActors.push_back(Actor());
            actor = &mActors.back();
        }

        actor->code = code;
        actor->colour = colour;
        actor->x = x;
        actor->y = y;
        actor->in_use = true;

        return index;
    }

    void destroy_actor(ActorHandle handle)
    {
        mActors[handle].in_use = false;
    }

    void set_actor_position(ActorHandle handle, int x, int y)
    {
        mActors[handle].x = x;
        mActors[handle].y = y;
    }

    void set_actor_colour(ActorHandle handle, color_t colour)
    {
    }

    void set_actor_code(ActorHandle handle, int code)
    {

    }

    void render()
    {
        int restoreLayer = terminal_state(TK_LAYER);

        // Draw map
        terminal_layer(mMapLayer);
        terminal_clear_area(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

        for (int y = 0; y < mHeight; ++y)
        {
            for (int x = 0; x < mWidth; ++x)
            {
                Tile& tile = tile_at(x, y);
                terminal_color(tile.colour);
                terminal_put(x, y, tile.code);
            }
        }

        // Draw actors
        terminal_layer(mMapLayer + 1);
        terminal_clear_area(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

        for (auto actor : mActors)
        {
            if (actor.in_use)
            {
                terminal_color(actor.colour);
                terminal_put(actor.x, actor.y, actor.code);
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

    struct Actor
    {
        int x, y;
        color_t colour;
        int code;
        bool in_use;
    };

    int mMapLayer;
    int mWidth;
    int mHeight;
    std::unique_ptr<Tile> mTiles;
    std::vector<Actor> mActors;

    Tile& tile_at(int x, int y) { return mTiles.get()[x + y * mWidth]; }
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
DrawMap draw_map(0);

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
    TileDef tiles[SCREEN_WIDTH * SCREEN_HEIGHT];
    memset(tiles, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(TileDef));

    int source_width = sizeof(map_data) / 9;
    int source_height = 9;
    int y = (25 - source_height) / 2;

    for (int h = 0; h < source_height; ++h, ++y)
    {
        int x = (80 - source_width) / 2;

        for (int w = 0; w < source_width; ++w, ++x)
        {
            tiles[x + y * 80].type = map_data[w + h * source_width] == ' ' ? 1 : 2;
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

    DrawMap::ActorHandle player = draw_map.create_actor('@', color_from_name("white"), player_x, player_y);

    while (!want_exit)
    {
        draw_map.set_actor_position(player, player_x, player_y);
        draw_map.render();
        terminal_refresh();
        process_input();
    }

    terminal_close();

    return 0;
}
