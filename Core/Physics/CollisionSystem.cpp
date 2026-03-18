//
// Created by SyperOlao on 17.03.2026.
//

#include "CollisionSystem.h"
#include <cmath>

#include "../Math/MathHelpers.h"

bool CollisionSystem::CheckCollision(const AABB &a, const AABB &b) noexcept {
    return a.Intersects(b);
}

CollisionManifold CollisionSystem::FindCollision(const AABB &a, const AABB &b) noexcept {
    CollisionManifold manifold{};
    if (!CheckCollision(a, b)) {
        return manifold;
    }

    const auto aCenter = a.GetCenter();
    const auto bCenter = b.GetCenter();
    const auto aExtents = a.GetExtents();
    const auto bExtents = b.GetExtents();

    const float deltaX = bCenter.x - aCenter.x;
    const float deltaY = bCenter.y - aCenter.y;

    const float overlapX = (aExtents.x + bExtents.x) - std::abs(deltaX);
    const float overlapY = (aExtents.y + bExtents.y) - std::abs(deltaY);

    manifold.HasCollision = true;

    if (overlapX < overlapY) {
        manifold.Penetration = overlapX;
        manifold.Normal = Vector2(
            deltaX < 0.0f ? -1.0f : 1.0f,
            0.0f
        );
    } else {
        manifold.Penetration = overlapY;
        manifold.Normal = Vector2(
            0.0f,
            deltaY < 0.0f ? -1.0f : 1.0f
        );
    }

    return manifold;
}

void CollisionSystem::ReflectAgainstNormal(
    Vector2 &velocity,
    const Vector2 &normal
) noexcept {
    velocity = MathHelpers::Reflect(velocity, normal);
}

Vector2 CollisionSystem::BuildPaddleBounceVelocity(
    const AABB &ball,
    const AABB &paddle,
    const float speed,
    const float maxBounceAngleRadians,
    const bool bounceToRight
) noexcept {
    const float paddleCenterY = paddle.GetCenter().y;
    const float ballCenterY = ball.GetCenter().y;
    const float halfPaddleHeight = std::max(paddle.GetExtents().y, 0.0001f);

    const float normalizedOffset = MathHelpers::Clamp(
        (ballCenterY - paddleCenterY) / halfPaddleHeight,
        -1.0f,
        1.0f
    );

    const float bounceAngle = normalizedOffset * maxBounceAngleRadians;
    const float horizontalDirection = bounceToRight ? 1.0f : -1.0f;

    Vector2 direction
    {
        std::cos(bounceAngle) * horizontalDirection,
        std::sin(bounceAngle)
    };

    direction = MathHelpers::SafeNormalize(direction, {horizontalDirection, 0.0f});
    return direction * speed;
}

CourtSide CollisionSystem::CheckOutOfBounds(
    const AABB &ball,
    const float leftBoundary,
    const float rightBoundary
) noexcept {
    if (ball.Max.x < leftBoundary) {
        return CourtSide::Left;
    }

    if (ball.Min.x > rightBoundary) {
        return CourtSide::Right;
    }

    return CourtSide::None;
}
