#ifndef PINGPONG_UNIFORMSPATIALGRID3D_H
#define PINGPONG_UNIFORMSPATIALGRID3D_H

#include "Core/Physics/Collision3D/AxisAlignedBox3D.h"

#include <SimpleMath.h>

#include <cstdint>
#include <unordered_map>
#include <vector>

struct UniformGridCellKey3D final
{
    std::int32_t CellX{0};
    std::int32_t CellY{0};
    std::int32_t CellZ{0};

    [[nodiscard]] bool operator==(const UniformGridCellKey3D &other) const noexcept
    {
        return CellX == other.CellX && CellY == other.CellY && CellZ == other.CellZ;
    }
};

struct UniformGridCellKey3DHash final
{
    [[nodiscard]] std::size_t operator()(const UniformGridCellKey3D &key) const noexcept
    {
        const std::uint64_t packedX = static_cast<std::uint64_t>(static_cast<std::uint32_t>(key.CellX));
        const std::uint64_t packedY = static_cast<std::uint64_t>(static_cast<std::uint32_t>(key.CellY));
        const std::uint64_t packedZ = static_cast<std::uint64_t>(static_cast<std::uint32_t>(key.CellZ));
        return static_cast<std::size_t>(packedX * 73856093ull ^ packedY * 19349663ull ^ packedZ * 83492791ull);
    }
};

class UniformSpatialGrid3D final
{
public:
    void Initialize(float cellSize, const AxisAlignedBox3D &worldBounds);

    void Clear();

    void InsertObject(std::uint32_t objectId, const AxisAlignedBox3D &worldBounds);

    void QueryOverlapping(const AxisAlignedBox3D &queryBounds, std::vector<std::uint32_t> &outObjectIds) const;

private:
    [[nodiscard]] UniformGridCellKey3D WorldPositionToCell(const DirectX::SimpleMath::Vector3 &worldPosition) const;

private:
    float m_cellSize{1.0f};
    AxisAlignedBox3D m_worldBounds{};
    bool m_hasBounds{false};
    std::unordered_map<UniformGridCellKey3D, std::vector<std::uint32_t>, UniformGridCellKey3DHash> m_cells{};
};

#endif
