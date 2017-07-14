#include "renderer.h"

#include <algorithm>

#include "map_def.h"

void Renderer::map_create(const MapDef& map_def)
{
    _map.reset(new RendererMap);
    _map->width = map_def.width;
    _map->height = map_def.height;

    size_t num_tiles = _map->width * _map->height;
    _map->tiles.reserve(num_tiles);

    for (auto def_tile : map_def.tiles)
    {
        RendererMap::Tile map_tile;

        switch (def_tile.type)
        {
            case TileType::Floor:
            {
                map_tile.code = '.';
                map_tile.colour = color_from_name("lighter grey");
                break;
            }
            case TileType::Wall:
            {
                map_tile.code = '#';
                map_tile.colour = color_from_name("lightest blue");
                break;
            }
            default:
            {
                map_tile.code = '?';
                map_tile.colour = color_from_name("red");
                break;
            }
        }

        _map->tiles.push_back(map_tile);
    }
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

void Renderer::draw_game()
{
    int view_width = std::min(terminal_state(TK_WIDTH), _map ? _map->width : 0);
    int view_height = std::min(terminal_state(TK_HEIGHT), _map ? _map->height : 0);

    terminal_clear();

    // Draw map
    if (_map)
    {
        terminal_layer(0);
        RendererMap::Tile* tile = &_map->tiles[0];

        for (int y = 0; y < view_height; ++y)
        {
            for (int x = 0; x < view_width; ++x, ++tile)
            {
                terminal_color(tile->colour);
                terminal_put(x, y, tile->code);
            }
        }
    }

    // Draw actors
    terminal_layer(1);

    for (auto actor : _actors)
    {
        terminal_color(actor.colour);
        terminal_put(actor.x, actor.y, actor.code);
    }
}
