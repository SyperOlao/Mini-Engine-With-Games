#include "Core/Gameplay/SphereStaticWorldCollisionResolver.h"

#include "Core/Gameplay/GameplayComponents.h"
#include "Core/Gameplay/Scene.h"
#include "Core/Gameplay/SceneCollisionTypes.h"

void SphereStaticWorldCollisionResolver::ApplySphereVersusStaticBoxResponseForEntity(
    Scene &scene,
    const EntityId sphereEntityId
)
{
    TransformComponent *const transform = scene.TryGetTransformComponent(sphereEntityId);
    if (transform == nullptr)
    {
        return;
    }

    const AttachmentComponent *const attachment = scene.TryGetAttachmentComponent(sphereEntityId);
    if (attachment != nullptr && attachment->ParentEntityId != 0u)
    {
        return;
    }

    VelocityComponent *const velocity = scene.TryGetVelocityComponent(sphereEntityId);

    const std::vector<CollisionPair> &pairs = scene.GetCollisionFrameResults();
    for (const CollisionPair &pair : pairs)
    {
        if (pair.Kind != CollisionPairKind::SphereBox)
        {
            continue;
        }

        if (pair.EntityA != sphereEntityId)
        {
            continue;
        }

        const BoxColliderComponent *const box = scene.TryGetBoxColliderComponent(pair.EntityB);
        if (box == nullptr || box->IsTrigger || !box->IsStatic)
        {
            continue;
        }

        if (!pair.Contact.HasOverlap)
        {
            continue;
        }

        transform->Local.Position += pair.Contact.Normal * pair.Contact.PenetrationDepth;

        if (velocity != nullptr)
        {
            const float inwardDot = velocity->LinearVelocity.Dot(pair.Contact.Normal);
            if (inwardDot < 0.0f)
            {
                velocity->LinearVelocity -= pair.Contact.Normal * inwardDot;
            }
        }
    }
}
