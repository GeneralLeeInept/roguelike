#include "actor.h"

Actor(const ActorDef& def)
    : _def(&def)
{
}

virtual ~Actor() = default;

virtual void update();
