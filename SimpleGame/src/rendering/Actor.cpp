#include "stdafx.h"
#include "Actor.h"

Actor::Actor(const WorldProp& description) : m_Description(description)
{
}

const WorldProp& Actor::Description() const
{
    return m_Description;
}

float Actor::Depth() const
{
    return m_Description.y;
}
