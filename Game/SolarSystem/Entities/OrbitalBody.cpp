//
// Created by SyperOlao on 19.03.2026.
//

#include "OrbitalBody.h"

using DirectX::SimpleMath::Matrix;

void OrbitalBody::Update(const float deltaTime)
{
    if (HasOrbit)
    {
        m_currentMeanAnomaly += Orbit.AngularSpeed * deltaTime;
    }

    SelfRotationAngle += SelfRotationSpeed * deltaTime;

    for (const auto& child : m_children)
    {
        child->Update(deltaTime);
    }
}

void OrbitalBody::UpdateWorldMatrix(const Matrix& parentWorld)
{
    const Matrix scaleMatrix = Matrix::CreateScale(Scale);
    const Matrix selfSpinMatrix = Matrix::CreateRotationY(SelfRotationAngle);

    Matrix orbitMatrix = Matrix::Identity;
    if (HasOrbit)
    {
        orbitMatrix = OrbitMath::CalculateOrbitMatrix(Orbit, m_currentMeanAnomaly + Orbit.Phase);
    }

    const Matrix localMatrix = scaleMatrix * selfSpinMatrix * orbitMatrix;
    m_world = localMatrix * parentWorld;

    for (const auto& child : m_children)
    {
        child->UpdateWorldMatrix(m_world);
    }
}

void OrbitalBody::AddChild(std::shared_ptr<OrbitalBody> child)
{
    m_children.push_back(std::move(child));
}

const Matrix& OrbitalBody::GetWorldMatrix() const noexcept
{
    return m_world;
}

const std::vector<std::shared_ptr<OrbitalBody>>& OrbitalBody::GetChildren() const noexcept
{
    return m_children;
}