#ifndef PINGPONG_SPHERESTATICWORLDCOLLISIONRESOLVER_H
#define PINGPONG_SPHERESTATICWORLDCOLLISIONRESOLVER_H

#include "Core/Gameplay/EntityId.h"

class Scene;

class SphereStaticWorldCollisionResolver final
{
public:
    static void ApplySphereVersusStaticBoxResponseForEntity(Scene &scene, EntityId sphereEntityId);
};

#endif
