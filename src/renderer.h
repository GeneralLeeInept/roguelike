#pragma once
#include <BearLibTerminal.h>

class MapDef;

class Renderer
{
public:
    typedef intptr_t MapHandle;
    typedef intptr_t ActorHandle;

    MapHandle map_create(const MapDef &map_def);

    ActorHandle actor_create(int code, color_t colour);
    void actor_set_visible(ActorHandle actor, bool visible);
    void actor_set_position(ActorHandle actor, int x, int y);

    void draw_game();
};
