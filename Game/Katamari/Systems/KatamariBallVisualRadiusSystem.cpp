#include "Game/Katamari/Systems/KatamariBallVisualRadiusSystem.h"

#include "Core/App/AppContext.h"
#include "Core/Gameplay/Scene.h"
#include "Game/Katamari/Data/KatamariGameConfig.h"
#include "Game/Katamari/Data/KatamariWorldContext.h"

#include <cmath>

using DirectX::SimpleMath::Vector3;

KatamariBallVisualRadiusSystem::KatamariBallVisualRadiusSystem(KatamariWorldContext *const gameplayWorld) noexcept
    : GameplayWorld(gameplayWorld)
{
}

void KatamariBallVisualRadiusSystem::Initialize(Scene &, AppContext &)
{
}

void KatamariBallVisualRadiusSystem::Update(Scene &scene, AppContext &, const float deltaTime)
{
    if (GameplayWorld == nullptr || GameplayWorld->Config == nullptr)
    {
        return;
    }

    KatamariGameConfig const &config = *GameplayWorld->Config;
    const float timeConstantSeconds = config.BallVisualRadiusSmoothingTimeConstantSeconds;
    const float TargetPhysicalRadius = GameplayWorld->BallRadius;
    float CurrentVisualRadius = GameplayWorld->BallVisualRadius;

    if (timeConstantSeconds <= 1.0e-8f)
    {
        CurrentVisualRadius = TargetPhysicalRadius;
        GameplayWorld->BallVisualRadius = CurrentVisualRadius;
        return;
    }

    const float SmoothingAlpha = 1.0f - std::exp(-deltaTime / timeConstantSeconds);
    CurrentVisualRadius += (TargetPhysicalRadius - CurrentVisualRadius) * SmoothingAlpha;

    if (std::abs(TargetPhysicalRadius - CurrentVisualRadius) < 1.0e-5f)
    {
        CurrentVisualRadius = TargetPhysicalRadius;
    }

    GameplayWorld->BallVisualRadius = CurrentVisualRadius;

    TransformComponent *const ballTransform = scene.TryGetTransformComponent(GameplayWorld->BallEntityId);
    if (ballTransform != nullptr && GameplayWorld->BallMeshReferenceRadius > 1.0e-5f)
    {
        const float uniformScale = CurrentVisualRadius / GameplayWorld->BallMeshReferenceRadius;
        ballTransform->Local.Scale = Vector3(uniformScale, uniformScale, uniformScale);
    }
}
