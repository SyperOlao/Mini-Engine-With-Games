//
// Created by SyperOlao on 18.03.2026.
//

#include "Paddle.h"
#include "Game/Pong/Entities/Paddle.h"

auto Paddle::SetDimensions(const float width, const float height) noexcept -> void {
    Collider.HalfSize = DirectX::SimpleMath::Vector2(width * 0.5f, height * 0.5f);
}

float Paddle::Width() const noexcept
{
    return Collider.HalfSize.x * 2.0f;
}

float Paddle::Height() const noexcept
{
    return Collider.HalfSize.y * 2.0f;
}

AABB Paddle::GetAABB() const noexcept
{
    return Collider.GetWorldAABB(Transform);
}