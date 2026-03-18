//
// Created by SyperOlao on 17.03.2026.
//

#ifndef PINGPONG_AABB_H
#define PINGPONG_AABB_H
#include <DirectXCollision.h>
#include <SimpleMath.h>

struct AABB final
{
    DirectX::SimpleMath::Vector2 Min{0.0f, 0.0f};
    DirectX::SimpleMath::Vector2 Max{0.0f, 0.0f};

    AABB() = default;

    AABB(const DirectX::SimpleMath::Vector2& minPoint, const DirectX::SimpleMath::Vector2& maxPoint) noexcept
        : Min(minPoint),
          Max(maxPoint)
    {
    }

    [[nodiscard]] static AABB FromCenterExtents(
        const DirectX::SimpleMath::Vector2& center,
        const DirectX::SimpleMath::Vector2& halfExtents
    ) noexcept
    {
        return AABB(center - halfExtents, center + halfExtents);
    }

    [[nodiscard]] static AABB FromPositionSize(
        const DirectX::SimpleMath::Vector2& position,
        const DirectX::SimpleMath::Vector2& size
    ) noexcept
    {
        return AABB(position, position + size);
    }

    [[nodiscard]] DirectX::SimpleMath::Vector2 GetCenter() const noexcept
    {
        return (Min + Max) * 0.5f;
    }

    [[nodiscard]] DirectX::SimpleMath::Vector2 GetExtents() const noexcept
    {
        return (Max - Min) * 0.5f;
    }

    [[nodiscard]] DirectX::SimpleMath::Vector2 GetSize() const noexcept
    {
        return Max - Min;
    }

    [[nodiscard]] bool Contains(const DirectX::SimpleMath::Vector2& point) const noexcept
    {
        return point.x >= Min.x && point.x <= Max.x && point.y >= Min.y && point.y <= Max.y;
    }

    [[nodiscard]] DirectX::BoundingBox ToBoundingBox() const noexcept
    {
        const auto center = GetCenter();
        const auto extents = GetExtents();
        return DirectX::BoundingBox({center.x, center.y, 0.0f}, {extents.x, extents.y, 0.0f});
    }

    [[nodiscard]] bool Intersects(const AABB& other) const noexcept
    {
        return ToBoundingBox().Intersects(other.ToBoundingBox());
    }
};

#endif //PINGPONG_AABB_H