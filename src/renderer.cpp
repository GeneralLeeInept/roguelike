#include "renderer.h"

#include <algorithm>

#include "fov.h"
#include "map_def.h"

void Renderer::init()
{
    _lit_colour = color_from_name("lighter orange");
    _unlit_colour = color_from_name("dark grey");
}

void Renderer::map_create(const MapDef& map_def)
{
    _map_def = &map_def;

    _map.reset(new RendererMap);
    _map->width = map_def.width;
    _map->height = map_def.height;

    size_t num_tiles = _map->width * _map->height;
    _map->tiles.resize(num_tiles, static_cast<int>(' '));
}

Renderer::ActorHandle Renderer::actor_create(int code, color_t colour)
{
    RendererActor actor;
    actor.code = code;
    actor.colour = colour;
    _actors.push_back(actor);
    return _actors.size() - 1;
}

void Renderer::actor_set_position(ActorHandle actor, int x, int y)
{
    _actors[actor].x = x;
    _actors[actor].y = y;
}

void Renderer::draw_game(const Fov& fov)
{
    // Update map according to fov
    int* map_tile = &_map->tiles[0];
    const MapDef::Tile* def_tile = &_map_def->tiles[0];

    for (int y = 0; y < _map->height; ++y)
    {
        for (int x = 0; x < _map->width; ++x)
        {
            if (fov.can_see(x, y))
            {
                switch (def_tile->type)
                {
                    case TileType::Floor:
                    {
                        *map_tile = static_cast<int>('.');
                        break;
                    }
                    case TileType::Wall:
                    {
                        *map_tile = static_cast<int>('#');
                        break;
                    }
                    default:
                    {
                        *map_tile = static_cast<int>('?');
                        break;
                    }
                }
            }

            map_tile++;
            def_tile++;
        }
    }

    int view_width = std::min(terminal_state(TK_WIDTH), _map ? _map->width : 0);
    int view_height = std::min(terminal_state(TK_HEIGHT), _map ? _map->height : 0);

    terminal_clear();

    // Draw map
    if (_map)
    {
        terminal_layer(0);
        int* tile = &_map->tiles[0];

        for (int y = 0; y < view_height; ++y)
        {
            for (int x = 0; x < view_width; ++x, ++tile)
            {
                if (fov.can_see(x, y))
                {
                    terminal_color(_lit_colour);
                }
                else
                {
                    terminal_color(_unlit_colour);
                }
                terminal_put(x, y, *tile);
            }
        }
    }

    // Draw actors
    terminal_layer(1);

    for (auto actor : _actors)
    {
        if (fov.can_see(actor.x, actor.y))
        {
            terminal_color(actor.colour);
            terminal_put(actor.x, actor.y, actor.code);
        }
    }
}
