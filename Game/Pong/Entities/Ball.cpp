//
// Created by SyperOlao on 18.03.2026.
//

#include "Ball.h"

void Ball::SetSize(const float size) noexcept
{
    Collider.HalfSize = DirectX::SimpleMath::Vector2(size * 0.5f, size * 0.5f);
}

float Ball::Size() const noexcept
{
    return Collider.HalfSize.x * 2.0f;
}

AABB Ball::GetAABB() const noexcept
{
    return Collider.GetWorldAABB(Transform);
}