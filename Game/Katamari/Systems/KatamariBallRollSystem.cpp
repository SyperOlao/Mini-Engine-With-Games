#include "Game/Katamari/Systems/KatamariBallRollSystem.h"

#include "Core/App/AppContext.h"
#include "Core/Gameplay/GameplayComponents.h"
#include "Core/Gameplay/Scene.h"
#include "Core/Math/SpatialMath.h"
#include "Game/Katamari/Data/KatamariWorldContext.h"

#include <algorithm>

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Quaternion;
using DirectX::SimpleMath::Vector3;

namespace
{
constexpr float MinimumHorizontalDistanceForRolling = 1.0e-4f;
}

KatamariBallRollSystem::KatamariBallRollSystem(KatamariWorldContext *const gameplayWorld) noexcept
    : GameplayWorld(gameplayWorld)
{
}

void KatamariBallRollSystem::Initialize(Scene &, AppContext &)
{
    RollOrientation = Quaternion::Identity;
    PreviousBallPosition = Vector3::Zero;
    HasPreviousBallPosition = false;
}

void KatamariBallRollSystem::Update(Scene &scene, AppContext &, const float)
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

    transform->Local.UseQuaternionRotation = true;
    transform->Local.RotationQuaternion = RollOrientation;

    const Vector3 currentBallPosition = transform->Local.Position;
    if (!HasPreviousBallPosition)
    {
        PreviousBallPosition = currentBallPosition;
        HasPreviousBallPosition = true;
        transform->WorldMatrix = transform->Local.GetWorldMatrix();
        return;
    }

    Vector3 horizontalDisplacement = currentBallPosition - PreviousBallPosition;
    PreviousBallPosition = currentBallPosition;
    horizontalDisplacement.y = 0.0f;

    const bool isGrounded = transform->Local.Position.y <= GameplayWorld->BallRadius + 0.02f;
    const float horizontalDistance = horizontalDisplacement.Length();
    if (!isGrounded || horizontalDistance < MinimumHorizontalDistanceForRolling)
    {
        transform->WorldMatrix = transform->Local.GetWorldMatrix();
        return;
    }

    const Vector3 movementDirection = horizontalDisplacement / horizontalDistance;

    const Vector3 rollAxis = SpatialMath::SafeNormalizeVector3(
        Vector3::UnitY.Cross(movementDirection),
        Vector3::Zero
    );
    if (rollAxis.LengthSquared() <= 1.0e-8f)
    {
        transform->WorldMatrix = transform->Local.GetWorldMatrix();
        return;
    }

    const float radius = (std::max)(GameplayWorld->BallRadius, 0.01f);
    const float visualScale = GameplayWorld->Config->BallVisualRollSpeedMultiplier;
    const float angularDisplacement = (horizontalDistance / radius) * visualScale;
    if (angularDisplacement <= 1.0e-6f)
    {
        transform->WorldMatrix = transform->Local.GetWorldMatrix();
        return;
    }

    // Roll from the resolved center displacement so the mesh follows the actual post-physics travel.
    const Matrix currentRotation = Matrix::CreateFromQuaternion(RollOrientation);
    const Matrix deltaRotation = Matrix::CreateFromAxisAngle(rollAxis, angularDisplacement);
    RollOrientation = Quaternion::CreateFromRotationMatrix(currentRotation * deltaRotation);
    RollOrientation.Normalize();

    transform->Local.RotationQuaternion = RollOrientation;
    transform->WorldMatrix = transform->Local.GetWorldMatrix();
}
