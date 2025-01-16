#ifndef IGAMECOMPONENT_H
#define IGAMECOMPONENT_H
#include "IUpdateComponent.h"

class Entity;
class IGameComponent : public IUpdateComponent
{
protected:
    Entity* m_gameObject;

public:
    IGameComponent(Entity*);
    Entity* getGameObject();
};

#endif // IGAMECOMPONENT_H
