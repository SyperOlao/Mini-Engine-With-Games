#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "Core/Physics/Collision3D/CollisionQueries3D.h"

#include <algorithm>
#include <cmath>

#include "Core/Math/SpatialMath.h"

bool CollisionQueries3D::OverlapTestSphereSphere(
    const BoundingSphere3D &firstSphere,
    const BoundingSphere3D &secondSphere
) noexcept
{
    const float distanceSquared = (firstSphere.Center - secondSphere.Center).LengthSquared();
    const float radiusSum = firstSphere.Radius + secondSphere.Radius;
    return distanceSquared <= radiusSum * radiusSum;
}

CollisionContact3D CollisionQueries3D::FindContactSphereSphere(
    const BoundingSphere3D &firstSphere,
    const BoundingSphere3D &secondSphere
) noexcept
{
    CollisionContact3D contact{};
    const DirectX::SimpleMath::Vector3 delta = secondSphere.Center - firstSphere.Center;
    const float distance = delta.Length();
    const float radiusSum = firstSphere.Radius + secondSphere.Radius;

    if (distance > radiusSum)
    {
        return contact;
    }

    contact.HasOverlap = true;

    constexpr float kMinimumSeparation = 1.0e-6f;
    if (distance <= kMinimumSeparation)
    {
        contact.Normal = DirectX::SimpleMath::Vector3::UnitY;
        contact.PenetrationDepth = radiusSum;
        return contact;
    }

    contact.Normal = SpatialMath::SafeNormalizeVector3(delta, DirectX::SimpleMath::Vector3::UnitY);
    contact.PenetrationDepth = radiusSum - distance;
    return contact;
}

bool CollisionQueries3D::OverlapTestSphereAxisAlignedBox(
    const BoundingSphere3D &sphere,
    const AxisAlignedBox3D &axisAlignedBox
) noexcept
{
    const DirectX::SimpleMath::Vector3 closestPoint = axisAlignedBox.ClosestPointTo(sphere.Center);
    const DirectX::SimpleMath::Vector3 delta = closestPoint - sphere.Center;
    return delta.LengthSquared() <= sphere.Radius * sphere.Radius;
}

CollisionContact3D CollisionQueries3D::FindContactSphereAxisAlignedBox(
    const BoundingSphere3D &sphere,
    const AxisAlignedBox3D &axisAlignedBox
) noexcept
{
    CollisionContact3D contact{};
    const DirectX::SimpleMath::Vector3 closestPoint = axisAlignedBox.ClosestPointTo(sphere.Center);
    DirectX::SimpleMath::Vector3 delta = sphere.Center - closestPoint;
    const float distanceSquared = delta.LengthSquared();
    constexpr float kMinimumSeparation = 1.0e-6f;

    if (distanceSquared > kMinimumSeparation * kMinimumSeparation)
    {
        const float distance = std::sqrt(distanceSquared);
        if (distance > sphere.Radius)
        {
            return contact;
        }

        contact.HasOverlap = true;
        contact.Normal = SpatialMath::SafeNormalizeVector3(delta, DirectX::SimpleMath::Vector3::UnitY);
        contact.PenetrationDepth = sphere.Radius - distance;
        contact.HasContactPoint = true;
        contact.ContactPoint = sphere.Center - contact.Normal * sphere.Radius;
        return contact;
    }

    const float distanceToMinX = sphere.Center.x - axisAlignedBox.Min.x;
    const float distanceToMaxX = axisAlignedBox.Max.x - sphere.Center.x;
    const float distanceToMinY = sphere.Center.y - axisAlignedBox.Min.y;
    const float distanceToMaxY = axisAlignedBox.Max.y - sphere.Center.y;
    const float distanceToMinZ = sphere.Center.z - axisAlignedBox.Min.z;
    const float distanceToMaxZ = axisAlignedBox.Max.z - sphere.Center.z;

    float minimumFaceDistance = distanceToMinX;
    DirectX::SimpleMath::Vector3 normal = DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 0.0f);

    if (distanceToMaxX < minimumFaceDistance)
    {
        minimumFaceDistance = distanceToMaxX;
        normal = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
    }
    if (distanceToMinY < minimumFaceDistance)
    {
        minimumFaceDistance = distanceToMinY;
        normal = DirectX::SimpleMath::Vector3(0.0f, -1.0f, 0.0f);
    }
    if (distanceToMaxY < minimumFaceDistance)
    {
        minimumFaceDistance = distanceToMaxY;
        normal = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
    }
    if (distanceToMinZ < minimumFaceDistance)
    {
        minimumFaceDistance = distanceToMinZ;
        normal = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f);
    }
    if (distanceToMaxZ < minimumFaceDistance)
    {
        minimumFaceDistance = distanceToMaxZ;
        normal = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f);
    }

    contact.HasOverlap = true;

    if (minimumFaceDistance >= sphere.Radius)
    {
        contact.PenetrationDepth = 0.0f;
        contact.Normal = DirectX::SimpleMath::Vector3::UnitY;
        contact.HasContactPoint = true;
        contact.ContactPoint = sphere.Center - contact.Normal * sphere.Radius;
        return contact;
    }

    contact.Normal = normal;
    contact.PenetrationDepth = sphere.Radius - minimumFaceDistance;
    contact.HasContactPoint = true;
    contact.ContactPoint = sphere.Center - contact.Normal * sphere.Radius;
    return contact;
}

CollisionContact3D CollisionQueries3D::FindContactSphereOrientedBox(
    const BoundingSphere3D &worldSphere,
    const DirectX::SimpleMath::Vector3 &localBoxCenterInEntitySpace,
    const DirectX::SimpleMath::Vector3 &halfExtents,
    const DirectX::SimpleMath::Matrix &entityWorldMatrix
) noexcept
{
    const DirectX::SimpleMath::Matrix worldToLocal = entityWorldMatrix.Invert();
    const DirectX::SimpleMath::Vector3 localSphereCenter = DirectX::SimpleMath::Vector3::Transform(
        worldSphere.Center,
        worldToLocal
    );

    const DirectX::SimpleMath::Vector3 columnX = DirectX::SimpleMath::Vector3(
        entityWorldMatrix._11,
        entityWorldMatrix._21,
        entityWorldMatrix._31
    );
    const DirectX::SimpleMath::Vector3 columnY = DirectX::SimpleMath::Vector3(
        entityWorldMatrix._12,
        entityWorldMatrix._22,
        entityWorldMatrix._32
    );
    const DirectX::SimpleMath::Vector3 columnZ = DirectX::SimpleMath::Vector3(
        entityWorldMatrix._13,
        entityWorldMatrix._23,
        entityWorldMatrix._33
    );
    const float scaleX = columnX.Length();
    const float scaleY = columnY.Length();
    const float scaleZ = columnZ.Length();
    const float maxScale = std::max(std::max(scaleX, scaleY), scaleZ);
    const float localRadius = maxScale > 1.0e-8f ? worldSphere.Radius / maxScale : worldSphere.Radius;

    const AxisAlignedBox3D localBox = AxisAlignedBox3D::FromCenterExtents(localBoxCenterInEntitySpace, halfExtents);
    BoundingSphere3D localSphere{};
    localSphere.Center = localSphereCenter;
    localSphere.Radius = localRadius;

    CollisionContact3D contact = FindContactSphereAxisAlignedBox(localSphere, localBox);
    if (!contact.HasOverlap)
    {
        return contact;
    }

    DirectX::SimpleMath::Vector3 worldNormal = DirectX::SimpleMath::Vector3::TransformNormal(contact.Normal, entityWorldMatrix);
    if (worldNormal.LengthSquared() > 1.0e-12f)
    {
        worldNormal.Normalize();
    }
    else
    {
        worldNormal = DirectX::SimpleMath::Vector3::UnitY;
    }

    contact.Normal = worldNormal;
    contact.PenetrationDepth = contact.PenetrationDepth * maxScale;
    contact.HasContactPoint = true;
    contact.ContactPoint = worldSphere.Center - worldNormal * worldSphere.Radius;
    return contact;
}

bool CollisionQueries3D::OverlapTestPointAxisAlignedBox(
    const DirectX::SimpleMath::Vector3 &point,
    const AxisAlignedBox3D &axisAlignedBox
) noexcept
{
    return axisAlignedBox.Contains(point);
}

bool CollisionQueries3D::OverlapTestPointSphere(
    const DirectX::SimpleMath::Vector3 &point,
    const BoundingSphere3D &sphere
) noexcept
{
    return (point - sphere.Center).LengthSquared() <= sphere.Radius * sphere.Radius;
}
