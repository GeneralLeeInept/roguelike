#include <BearLibTerminal.h>

#include "renderer.h"

#include <algorithm>
#include <map>

#include "fov.h"
#include "map_def.h"

class TileRenderInfo
{
public:
    TileRenderInfo(int code, color_t lit_colour, color_t unlit_colour)
        : _code(code)
        , _lit_colour(lit_colour)
        , _unlit_colour(unlit_colour)
    {
    }

    TileRenderInfo()
        : TileRenderInfo('?', color_from_argb(0xff, 0xff, 0, 0xff), color_from_argb(0xff, 0xff, 0, 0xff))
    {
    }

    int _code;
    color_t _lit_colour;
    color_t _unlit_colour;
};

class ActorRenderInfo
{
public:
    ActorRenderInfo(int code, color_t colour)
        : _code(code)
        , _colour(colour)
    {
    }

    ActorRenderInfo()
        : ActorRenderInfo('?', color_from_argb(0xff, 0xff, 0, 0))
    {
    }

    int _code;
    color_t _colour;
};

static std::map<TileType, TileRenderInfo> _tile_render_info;
static std::map<ActorType, ActorRenderInfo> _actor_render_info;

void Renderer::init()
{
    // Can put this in a config file.
    static const unsigned lit_colour = 0xffd7ac6a;
    static const unsigned unlit_colour = 0xff6b6050;

    _tile_render_info[TileType::Empty] = { ' ', 0xff000000, 0xff000000 };
    _tile_render_info[TileType::Floor] = { '.', lit_colour, unlit_colour };
    _tile_render_info[TileType::Wall] = { '#', lit_colour, unlit_colour };

    _actor_render_info[ActorType::Player] = { '@', 0xffc8c8c8 };
    _actor_render_info[ActorType::WeakMonster] = { 'm', 0xffa85219 };
    _actor_render_info[ActorType::StrongMonster] = { 'M', 0xffff2f42 };
}

void Renderer::map_create(const MapDef& map_def)
{
    _map_def = &map_def;
    _map.reset(new Map);
    _map->size = map_def.size;
    size_t num_tiles = map_def.size.x * map_def.size.y;
    _map->tiles.resize(num_tiles, TileType::Empty);
}

Renderer::ActorHandle Renderer::actor_create(ActorType type, const Point& position)
{
    Actor actor;
    actor.code = _actor_render_info[type]._code;
    actor.colour = _actor_render_info[type]._colour;
    actor.position = position;
    _actors[_next_actor_handle] = actor;
    return _next_actor_handle++;
}

void Renderer::actor_destroy(ActorHandle actor)
{
    _actors.erase(actor);
}

void Renderer::actor_set_position(ActorHandle actor, const Point& position)
{
    _actors[actor].position = position;
}

void Renderer::draw_game(const Fov& fov, const Point& reference_point)
{
    Point screen_size(terminal_state(TK_WIDTH), terminal_state(TK_HEIGHT));

    // Scroll to keep reference_point in the middle 1/3 of the screen
    int bw = (screen_size.x - 26) / 3;
    int bh = screen_size.y / 3;

    Point screen_reference_point = reference_point - _map_scroll;

    if (screen_reference_point.x < bw)
    {
        _map_scroll.x = reference_point.x - bw;
    }

    if (screen_reference_point.x > 2 * bw)
    {
        _map_scroll.x = reference_point.x - 2 * bw;
    }

    if (screen_reference_point.y < bh)
    {
        _map_scroll.y = reference_point.y - bh;
    }

    if (screen_reference_point.y > 2 * bh)
    {
        _map_scroll.y = reference_point.y - 2 * bh;
    }

    terminal_bkcolor(0xff252526);
    terminal_clear_area(26, 0, screen_size.x, screen_size.y);

    draw_map(fov, Rectangle(Point(26, 0), screen_size));

    terminal_bkcolor(0xff333337);
    terminal_clear_area(0, 0, 26, screen_size.y);
}

void Renderer::draw_map(const Fov& fov, const Rectangle& viewport)
{
    // Update map according to fov
    TileType* map_tile = &_map->tiles[0];
    const TileType* def_tile = &_map_def->tiles[0];

    for (int y = 0; y < _map->size.y; ++y)
    {
        for (int x = 0; x < _map->size.x; ++x)
        {
            if (fov.can_see(Point(x, y)))
            {
                *map_tile = *def_tile;
            }

            map_tile++;
            def_tile++;
        }
    }

    // Draw map
    int tile_y = _map_scroll.y;

    for (int screen_y = viewport.mins.y; screen_y < viewport.maxs.y && tile_y < _map->size.y; ++screen_y, ++tile_y)
    {
        int tile_x = _map_scroll.x;

        if (tile_y < 0)
        {
            continue;
        }

        for (int screen_x = viewport.mins.x; screen_x < viewport.maxs.x && tile_x < _map->size.x; ++screen_x, ++tile_x)
        {
            if (tile_x < 0)
            {
                continue;
            }
            TileRenderInfo& info = _tile_render_info[_map->tiles[tile_x + tile_y * _map->size.x]];

            if (fov.can_see(Point(tile_x, tile_y)))
            {
                terminal_color(info._lit_colour);
            }
            else
            {
                terminal_color(info._unlit_colour);
            }

            terminal_put(screen_x, screen_y, info._code);
        }
    }

    // Draw actors
    for (auto actor : _actors)
    {
        if (fov.can_see(actor.second.position))
        {
            terminal_color(actor.second.colour);
            terminal_put(viewport.mins.x + actor.second.position.x - _map_scroll.x, viewport.mins.y + actor.second.position.y - _map_scroll.y, actor.second.code);
        }
    }
}
