#ifndef PINGPONG_SCENECOLLISIONTYPES_H
#define PINGPONG_SCENECOLLISIONTYPES_H

#include "Core/Gameplay/EntityId.h"
#include "Core/Physics/Collision3D/CollisionContact3D.h"

#include <cstdint>

enum class CollisionPairKind : std::uint8_t
{
    SphereSphere = 0,
    SphereBox = 1
};

struct CollisionPair final
{
    CollisionPairKind Kind{CollisionPairKind::SphereSphere};
    EntityId EntityA{0u};
    EntityId EntityB{0u};
    CollisionContact3D Contact{};
};

#endif
