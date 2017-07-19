#pragma once

#include "geometry.h"
#include "renderer.h"

enum class ActorType : int8_t
{
    Player,
    WeakMonster,
    StrongMonster
};

struct ActorDef
{
    ActorType type;
    Point spawn_position;
    int speed;
};

class Actor
{
public:
    Actor() = default;
    Actor(const ActorDef& def);

    virtual ~Actor() = default;

    virtual void update();

    ActorDef* _def;
    Point position;
    int speed;
    int energy;
    Renderer::ActorHandle renderer_handle;
};
