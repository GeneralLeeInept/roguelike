#pragma once

#include "geometry.h"

class Thinker
{
public:
    virtual ~Thinker() = default;

    virtual void think() = 0;
};

class Actor;
class Dungeon;

class MeleeMonster : public Thinker
{
public:
    MeleeMonster() = delete;
    MeleeMonster(Actor& owner);

    void think() override;

protected:
    enum class State : uint8_t
    {
        Seeking, // Looking for player
        Tracking // Tracking player
    };

    Actor* _owner;
    Point _target_location;
    State _state;
};
