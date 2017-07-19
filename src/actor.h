#pragma once

#include "actor_def.h"
#include "geometry.h"
#include "renderer.h"

class Actor
{
public:
    Actor() = default;
    Actor(const ActorDef& def);

    virtual ~Actor() = default;

    virtual void update();

    const ActorDef* _def;
    Point position;
    int speed;
    int energy;
    Renderer::ActorHandle renderer_handle;
};
