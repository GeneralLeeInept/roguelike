#pragma once
#include "actor_def.h"
#include "geometry.h"
#include "map_def.h"

#include <map>
#include <memory>

class Fov;

typedef uint32_t color_t;

class Renderer
{
public:
    Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void init();

    void map_create(const MapDef& map_def);

    typedef size_t ActorHandle;

    ActorHandle actor_create(ActorType type, const Point& position);
    void actor_destroy(ActorHandle actor);
    void actor_set_position(ActorHandle actor, const Point& postition);

    void draw_game(const Fov& fov, const Point& reference_point);
    void draw_map(const Fov& fov, const Rectangle& viewport);

private:
    struct Map
    {
        Point size;
        std::vector<TileType> tiles;
    };

    struct Actor
    {
        Point position;
        int code;
        color_t colour;
    };

    const MapDef* _map_def;
    std::unique_ptr<Map> _map;
    std::map<size_t, Actor> _actors;
    size_t _next_actor_handle;
    color_t _lit_colour;
    color_t _unlit_colour;
    Point _map_scroll;
};
