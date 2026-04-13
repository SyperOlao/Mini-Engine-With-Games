#ifndef PINGPONG_ATTACHMENTTRANSFORMHELPERS_H
#define PINGPONG_ATTACHMENTTRANSFORMHELPERS_H

#include "Core/Gameplay/EntityId.h"

#include <SimpleMath.h>

class Scene;

namespace AttachmentTransformHelpers
{
[[nodiscard]] bool AttachEntityPreserveWorldTransform(
    Scene &scene,
    EntityId childEntityId,
    EntityId parentEntityId,
    DirectX::SimpleMath::Vector3 const &attachmentLocalOffset
);
}

#endif
