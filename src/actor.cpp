#include "actor.h"

Actor::Actor(const ActorDef& def)
    : _def(&def)
    , _speed(0)
    , _next_action(nullptr)
{
}

void Actor::update()
{
    _energy += _speed;
    if (_energy >= 100)
    {
        _energy -= 100;
    }
}

const ActorDef& Actor::get_def() const
{
    return *_def;
}

Action* Actor::get_next_action()
{
    Action* next = _next_action;
    _next_action = nullptr;
    return next;
}

Point Actor::get_position() const
{
    return _position;
}

Renderer::ActorHandle Actor::get_renderer_handle() const
{
    return _renderer_handle;
}

int Actor::get_speed() const
{
    return _speed;
}

void Actor::set_next_action(Action* action)
{
    delete _next_action;
    _next_action = action;
}

void Actor::set_position(const Point& position)
{
    _position = position;
}

void Actor::set_renderer_handle(Renderer::ActorHandle renderer_handle)
{
    _renderer_handle = renderer_handle;
}

void Actor::set_speed(int speed)
{
    _speed = speed;
}
