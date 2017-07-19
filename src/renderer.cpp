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
    _tile_render_info[TileType::Empty] = { ' ', color_from_name("white"), color_from_name("white") };
    _tile_render_info[TileType::Floor] = { '.', color_from_name("lighter orange"), color_from_name("dark grey") };
    _tile_render_info[TileType::Wall] = { '#', color_from_name("lighter orange"), color_from_name("dark grey") };

    _actor_render_info[ActorType::Player] = { '@', color_from_name("white") };
    _actor_render_info[ActorType::WeakMonster] = { 'm', color_from_name("light brown") };
    _actor_render_info[ActorType::StrongMonster] = { 'M', color_from_name("dark red") };
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
    _actors.push_back(actor);
    return _actors.size() - 1;
}

void Renderer::actor_set_position(ActorHandle actor, const Point& position)
{
    _actors[actor].position = position;
}

void Renderer::draw_game(const Fov& fov)
{
    terminal_clear();

    // Update map according to fov
    TileType* map_tile = &_map->tiles[0];
    const MapDef::Tile* def_tile = &_map_def->tiles[0];

    for (int y = 0; y < _map->size.y; ++y)
    {
        for (int x = 0; x < _map->size.x; ++x)
        {
            if (fov.can_see(Point(x, y)))
            {
                *map_tile = def_tile->type;
            }

            map_tile++;
            def_tile++;
        }
    }

    Point view_size(std::min(terminal_state(TK_WIDTH), _map->size.x), std::min(terminal_state(TK_HEIGHT), _map->size.y));

    // Draw map
    terminal_layer(0);
    TileType* tile = &_map->tiles[0];

    for (int y = 0; y < view_size.y; ++y)
    {
        for (int x = 0; x < view_size.x; ++x, ++tile)
        {
            TileRenderInfo& info = _tile_render_info[*tile];
            if (fov.can_see(Point(x, y)))
            {
                terminal_color(info._lit_colour);
            }
            else
            {
                terminal_color(info._unlit_colour);
            }
            terminal_put(x, y, info._code);
        }
    }

    // Draw actors
    terminal_layer(0);

    for (auto actor : _actors)
    {
        if (fov.can_see(actor.position))
        {
            terminal_color(actor.colour);
            terminal_put(actor.position.x, actor.position.y, actor.code);
        }
    }
}
