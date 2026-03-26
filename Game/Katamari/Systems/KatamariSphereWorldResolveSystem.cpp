#include "Game/Katamari/Systems/KatamariSphereWorldResolveSystem.h"

#include "Core/App/AppContext.h"
#include "Core/Gameplay/GameplayComponents.h"
#include "Core/Gameplay/Scene.h"
#include "Core/Gameplay/SceneCollisionTypes.h"
#include "Game/Katamari/Data/KatamariWorldContext.h"

KatamariSphereWorldResolveSystem::KatamariSphereWorldResolveSystem(KatamariWorldContext *const gameplayWorld) noexcept
    : GameplayWorld(gameplayWorld)
{
}

void KatamariSphereWorldResolveSystem::Initialize(Scene &, AppContext &)
{
}

void KatamariSphereWorldResolveSystem::Update(Scene &scene, AppContext &, float)
{
    if (GameplayWorld == nullptr || GameplayWorld->Config == nullptr)
    {
        return;
    }

    const EntityId ballId = GameplayWorld->BallEntityId;
    TransformComponent *const transform = scene.TryGetTransformComponent(ballId);
    if (transform == nullptr)
    {
        return;
    }

    VelocityComponent *const velocity = scene.TryGetVelocityComponent(ballId);
    const std::vector<CollisionPair> &pairs = scene.GetCollisionFrameResults();

    const CollisionPair *BestPair = nullptr;
    float BestPenetrationDepth = 0.0f;

    for (CollisionPair const &pair : pairs)
    {
        if (pair.Kind != CollisionPairKind::SphereBox)
        {
            continue;
        }

        if (pair.EntityA != ballId)
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

        if (pair.Contact.PenetrationDepth > BestPenetrationDepth)
        {
            BestPenetrationDepth = pair.Contact.PenetrationDepth;
            BestPair = &pair;
        }
    }

    if (BestPair != nullptr)
    {
        transform->Local.Position += BestPair->Contact.Normal * BestPair->Contact.PenetrationDepth;

        if (velocity != nullptr)
        {
            const float inwardDot = velocity->LinearVelocity.Dot(BestPair->Contact.Normal);
            if (inwardDot < 0.0f)
            {
                velocity->LinearVelocity -= BestPair->Contact.Normal * inwardDot;
            }
        }
    }

    if (transform->Local.Position.y < GameplayWorld->BallRadius)
    {
        transform->Local.Position.y = GameplayWorld->BallRadius;
    }
}
