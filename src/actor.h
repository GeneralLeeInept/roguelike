#pragma once

#include "actor_def.h"
#include "geometry.h"
#include "renderer.h"
#include "thinker.h"

class Action;
class Dungeon;
class Thinker;

class Actor
{
public:
    Actor() = default;
    Actor(const ActorDef& def, Dungeon& dungeon);

    virtual ~Actor() = default;

    void act();
    bool can_act() const;
    void gain_energy();
    bool has_next_action() const;
    virtual bool needs_input() const;

    const ActorDef& get_def() const;
    Dungeon& get_dungeon() const;
    Point get_position() const;
    Renderer::ActorHandle get_renderer_handle() const;
    int get_speed() const;

    void set_next_action(Action* next_action);
    void set_position(const Point& position);
    void set_renderer_handle(Renderer::ActorHandle renderer_handle);
    void set_speed(int speed);
    void set_thinker(Thinker* thinker);

private:
    const ActorDef* _def;
    Dungeon* _dungeon;
    Point _position;
    int _speed;
    int _energy;
    Renderer::ActorHandle _renderer_handle;
    std::unique_ptr<Action> _next_action;
    std::unique_ptr<Thinker> _thinker;
};
