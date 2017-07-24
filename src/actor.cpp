#include "actor.h"

#include "action.h"
#include "thinker.h"

Actor::Actor(const ActorDef& def, Dungeon& dungeon)
    : _def(&def)
    , _dungeon(&dungeon)
    , _energy(0)
    , _next_action(nullptr)
    , _thinker(nullptr)
{
    set_speed(def.speed);
}

void Actor::act()
{
    if (_thinker)
    {
        _thinker->think();
    }

    if (_next_action)
    {
        _next_action->execute(this);
        _next_action.reset(nullptr);
    }

    _energy -= 100;
}

bool Actor::can_act() const
{
    return _energy >= 100;
}

void Actor::gain_energy()
{
    _energy += _speed;
}

bool Actor::has_next_action() const
{
    return _next_action != nullptr;
}

bool Actor::needs_input() const
{
    return false;
}

const ActorDef& Actor::get_def() const
{
    return *_def;
}

Dungeon& Actor::get_dungeon() const
{
    return *_dungeon;
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
    _next_action.reset(action);
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

void Actor::set_thinker(Thinker* thinker)
{
    _thinker.reset(thinker);
}
