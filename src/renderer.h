#pragma once
#include <BearLibTerminal.h>

#include "map_def.h"

#include <memory>
#include <vector>

class Fov;

struct RendererMap
{
    Point size;
    std::vector<TileType> tiles;
};

struct RendererActor
{
    Point position;
    int code;
    color_t colour;
};

class Renderer
{
public:
    Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void init();

    void map_create(const MapDef& map_def);

    typedef size_t ActorHandle;

    ActorHandle actor_create(int code, color_t colour);
    void actor_set_position(ActorHandle actor, const Point& postition);

    void draw_game(const Fov &fov);

private:
    const MapDef* _map_def;
    std::unique_ptr<RendererMap> _map;
    std::vector<RendererActor> _actors;
    color_t _lit_colour;
    color_t _unlit_colour;
};
