#include "actor.h"

#include "action.h"

Actor::Actor(const ActorDef& def)
    : _def(&def)
    , _energy(0)
{
    set_speed(def.speed);
}

void Actor::update()
{
    _energy += _speed;
    if (_energy >= 100)
    {
        Action* action = get_next_action();
        if (action)
        {
            _energy -= 100;
            action->execute(this);
            delete action;
        }
    }
}

const ActorDef& Actor::get_def() const
{
    return *_def;
}

Action* Actor::get_next_action()
{
    return nullptr;
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
