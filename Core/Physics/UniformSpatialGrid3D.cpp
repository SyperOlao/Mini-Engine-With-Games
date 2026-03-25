#include "Core/Physics/UniformSpatialGrid3D.h"

#include <cmath>
#include <unordered_set>

using DirectX::SimpleMath::Vector3;

void UniformSpatialGrid3D::Initialize(const float cellSize, const AxisAlignedBox3D &worldBounds)
{
    m_cellSize = cellSize;
    m_worldBounds = worldBounds;
    m_hasBounds = true;
    m_cells.clear();
}

void UniformSpatialGrid3D::Clear()
{
    m_cells.clear();
}

UniformGridCellKey3D UniformSpatialGrid3D::WorldPositionToCell(const Vector3 &worldPosition) const
{
    const float inverseCell = 1.0f / m_cellSize;
    UniformGridCellKey3D key{};
    key.CellX = static_cast<std::int32_t>(std::floor((worldPosition.x - m_worldBounds.Min.x) * inverseCell));
    key.CellY = static_cast<std::int32_t>(std::floor((worldPosition.y - m_worldBounds.Min.y) * inverseCell));
    key.CellZ = static_cast<std::int32_t>(std::floor((worldPosition.z - m_worldBounds.Min.z) * inverseCell));
    return key;
}

void UniformSpatialGrid3D::InsertObject(const std::uint32_t objectId, const AxisAlignedBox3D &worldBounds)
{
    if (!m_hasBounds || m_cellSize <= 0.0f)
    {
        return;
    }

    const UniformGridCellKey3D minCell = WorldPositionToCell(worldBounds.Min);
    const UniformGridCellKey3D maxCell = WorldPositionToCell(worldBounds.Max);

    const std::int32_t startX = (std::min)(minCell.CellX, maxCell.CellX);
    const std::int32_t endX = (std::max)(minCell.CellX, maxCell.CellX);
    const std::int32_t startY = (std::min)(minCell.CellY, maxCell.CellY);
    const std::int32_t endY = (std::max)(minCell.CellY, maxCell.CellY);
    const std::int32_t startZ = (std::min)(minCell.CellZ, maxCell.CellZ);
    const std::int32_t endZ = (std::max)(minCell.CellZ, maxCell.CellZ);

    for (std::int32_t cellZ = startZ; cellZ <= endZ; ++cellZ)
    {
        for (std::int32_t cellY = startY; cellY <= endY; ++cellY)
        {
            for (std::int32_t cellX = startX; cellX <= endX; ++cellX)
            {
                UniformGridCellKey3D key{};
                key.CellX = cellX;
                key.CellY = cellY;
                key.CellZ = cellZ;
                m_cells[key].push_back(objectId);
            }
        }
    }
}

void UniformSpatialGrid3D::QueryOverlapping(
    const AxisAlignedBox3D &queryBounds,
    std::vector<std::uint32_t> &outObjectIds
) const
{
    outObjectIds.clear();
    if (!m_hasBounds || m_cellSize <= 0.0f)
    {
        return;
    }

    const UniformGridCellKey3D minCell = WorldPositionToCell(queryBounds.Min);
    const UniformGridCellKey3D maxCell = WorldPositionToCell(queryBounds.Max);

    const std::int32_t startX = (std::min)(minCell.CellX, maxCell.CellX);
    const std::int32_t endX = (std::max)(minCell.CellX, maxCell.CellX);
    const std::int32_t startY = (std::min)(minCell.CellY, maxCell.CellY);
    const std::int32_t endY = (std::max)(minCell.CellY, maxCell.CellY);
    const std::int32_t startZ = (std::min)(minCell.CellZ, maxCell.CellZ);
    const std::int32_t endZ = (std::max)(minCell.CellZ, maxCell.CellZ);

    std::unordered_set<std::uint32_t> uniqueIds{};
    for (std::int32_t cellZ = startZ; cellZ <= endZ; ++cellZ)
    {
        for (std::int32_t cellY = startY; cellY <= endY; ++cellY)
        {
            for (std::int32_t cellX = startX; cellX <= endX; ++cellX)
            {
                UniformGridCellKey3D key{};
                key.CellX = cellX;
                key.CellY = cellY;
                key.CellZ = cellZ;
                const auto iterator = m_cells.find(key);
                if (iterator == m_cells.end())
                {
                    continue;
                }
                for (const std::uint32_t objectId : iterator->second)
                {
                    uniqueIds.insert(objectId);
                }
            }
        }
    }

    outObjectIds.assign(uniqueIds.begin(), uniqueIds.end());
}
