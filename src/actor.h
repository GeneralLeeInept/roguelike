#pragma once

#include "actor_def.h"
#include "geometry.h"
#include "renderer.h"

class Action;

class Actor
{
public:
    Actor() = default;
    Actor(const ActorDef& def);

    virtual ~Actor() = default;

    virtual void update();

    const ActorDef& get_def() const;
    virtual Action* get_next_action();
    Point get_position() const;
    Renderer::ActorHandle get_renderer_handle() const;
    int get_speed() const;

    void set_position(const Point& position);
    void set_renderer_handle(Renderer::ActorHandle renderer_handle);
    void set_speed(int speed);

private:
    const ActorDef* _def;
    Point _position;
    int _speed;
    int _energy;
    Renderer::ActorHandle _renderer_handle;
};
