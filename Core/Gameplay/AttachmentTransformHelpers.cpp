#include "Core/Gameplay/AttachmentTransformHelpers.h"

#include "Core/Gameplay/GameplayComponents.h"
#include "Core/Gameplay/Scene.h"

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Quaternion;
using DirectX::SimpleMath::Vector3;

bool AttachmentTransformHelpers::AttachEntityPreserveWorldTransform(
    Scene &scene,
    EntityId const childEntityId,
    EntityId const parentEntityId,
    Vector3 const &attachmentLocalOffset
)
{
    if (childEntityId == 0u || parentEntityId == 0u || childEntityId == parentEntityId)
    {
        return false;
    }

    TransformComponent *const childTransform = scene.TryGetTransformComponent(childEntityId);
    TransformComponent *const parentTransform = scene.TryGetTransformComponent(parentEntityId);
    if (childTransform == nullptr || parentTransform == nullptr)
    {
        return false;
    }

    if (scene.TryGetAttachmentComponent(childEntityId) != nullptr)
    {
        return false;
    }

    Matrix const childWorldBefore = scene.ComputeWorldMatrixFromAttachmentHierarchy(childEntityId);
    Matrix const parentWorld = scene.ComputeWorldMatrixFromAttachmentHierarchy(parentEntityId);

    parentTransform->WorldMatrix = parentWorld;

    AttachmentComponent attachment{};
    attachment.ParentEntityId = parentEntityId;
    attachment.LocalOffset = attachmentLocalOffset;
    if (!scene.AddAttachmentComponent(childEntityId, attachment))
    {
        return false;
    }

    Matrix const offsetMatrix = Matrix::CreateTranslation(attachmentLocalOffset);
    Matrix const chain = offsetMatrix * parentWorld;
    Matrix localMatrix = childWorldBefore * chain.Invert();

    Vector3 decomposedScale;
    Quaternion decomposedRotation;
    Vector3 decomposedTranslation;
    if (localMatrix.Decompose(decomposedScale, decomposedRotation, decomposedTranslation))
    {
        childTransform->Local.Scale = decomposedScale;
        childTransform->Local.Position = decomposedTranslation;
        childTransform->Local.RotationEulerRad = Matrix::CreateFromQuaternion(decomposedRotation).ToEuler();
    }
    else
    {
        childTransform->Local.Position = Vector3::Zero;
    }

    childTransform->WorldMatrix = childTransform->Local.GetWorldMatrix() * offsetMatrix * parentWorld;
    return true;
}
