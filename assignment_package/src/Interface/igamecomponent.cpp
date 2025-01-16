#include "IUpdateComponent.h"
#include "igamecomponent.h"

IGameComponent::IGameComponent(Entity* entity):
    m_gameObject(entity)
{
    awake();
}

Entity *IGameComponent::getGameObject()
{
    return this->m_gameObject;
}
