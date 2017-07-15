#pragma once
#include <BearLibTerminal.h>

#include <memory>
#include <vector>

class Fov;
struct MapDef;

struct RendererMap
{
    struct Tile
    {
        int code;
        color_t colour;
    };

    int width;
    int height;
    std::vector<Tile> tiles;
};

struct RendererActor
{
    int code;
    color_t colour;
    int x;
    int y;
};

class Renderer
{
public:
    Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void map_create(const MapDef& map_def);

    typedef size_t ActorHandle;

    ActorHandle actor_create(int code, color_t colour);
    void actor_set_position(ActorHandle actor, int x, int y);

    void draw_game(const Fov &fov);

private:
    std::unique_ptr<RendererMap> _map;
    std::vector<RendererActor> _actors;
};
