#include "actor.h"

#include "action.h"
#include "thinker.h"

MeleeFighter::MeleeFighter(Actor& owner, int hp, int power, int defence)
    : _owner(&owner)
    , _hp(hp)
    , _power(power)
    , _defence(defence)
{
}

int MeleeFighter::get_defence() const
{
    return _defence;
}

int MeleeFighter::get_hp() const
{
    return _hp;
}

int MeleeFighter::get_power() const
{
    return _power;
}

bool MeleeFighter::is_alive() const
{
    return _hp >= 0;
}

void MeleeFighter::attack(MeleeFighter& opponent)
{
    int damage = _power - opponent._defence;

    if (damage > 0)
    {
        opponent.take_damage(damage);
    }
}

void MeleeFighter::take_damage(int damage)
{
    _hp -= damage;
}

Actor::Actor(const ActorDef& def, Dungeon& dungeon)
    : _def(&def)
    , _dungeon(&dungeon)
    , _energy(0)
    , _next_action(nullptr)
    , _thinker(nullptr)
    , _fighter(nullptr)
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
        _next_action->execute(*this);
        _next_action.reset(nullptr);
    }

    _energy -= 100;
}

bool Actor::can_act() const
{
    return _energy >= 100 && (!_fighter || _fighter->get_hp() >= 0);
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

const MeleeFighter* Actor::get_fighter() const
{
    return _fighter.get();
}

MeleeFighter* Actor::get_fighter()
{
    return _fighter.get();
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

void Actor::set_fighter(MeleeFighter* fighter)
{
    _fighter.reset(fighter);
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
