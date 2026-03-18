//
// Created by SyperOlao on 17.03.2026.
//

#include "Entity.h"

#include "../Render/Renderer.h"

Entity::Entity(std::string name)
    : m_name(std::move(name))
{
}

void Entity::Update(const float deltaTime)
{
    if (!m_isActive)
    {
        return;
    }

    if (m_movement.has_value())
    {
        m_movement->Update(m_transform, deltaTime);
    }

    OnUpdate(deltaTime);
}

void Entity::Render(const Renderer& renderer) const
{
    if (!m_isActive)
    {
        return;
    }

    if (m_renderComponent.has_value())
    {
        m_renderComponent->Render(m_transform, renderer);
    }

    OnRender(renderer);
}

const std::string& Entity::GetName() const noexcept
{
    return m_name;
}

bool Entity::IsActive() const noexcept
{
    return m_isActive;
}

void Entity::SetActive(const bool isActive) noexcept
{
    m_isActive = isActive;
}

Transform2D& Entity::GetTransform() noexcept
{
    return m_transform;
}

const Transform2D& Entity::GetTransform() const noexcept
{
    return m_transform;
}

std::optional<MovementComponent>& Entity::GetMovement() noexcept
{
    return m_movement;
}

const std::optional<MovementComponent>& Entity::GetMovement() const noexcept
{
    return m_movement;
}

std::optional<Collider2D>& Entity::GetCollider() noexcept
{
    return m_collider;
}

const std::optional<Collider2D>& Entity::GetCollider() const noexcept
{
    return m_collider;
}

std::optional<RenderComponent2D>& Entity::GetRenderComponent() noexcept
{
    return m_renderComponent;
}

const std::optional<RenderComponent2D>& Entity::GetRenderComponent() const noexcept
{
    return m_renderComponent;
}

MovementComponent& Entity::AddMovementComponent()
{
    m_movement.emplace();
    return *m_movement;
}

Collider2D& Entity::AddColliderComponent(
    const DirectX::SimpleMath::Vector2& halfSize,
    const DirectX::SimpleMath::Vector2& offset
)
{
    m_collider.emplace();
    m_collider->HalfSize = halfSize;
    m_collider->Offset = offset;
    return *m_collider;
}

RenderComponent2D& Entity::AddRenderComponent(
    const DirectX::SimpleMath::Vector2& size,
    const Color& tint,
    const int layer
)
{
    m_renderComponent.emplace();
    m_renderComponent->Size = size;
    m_renderComponent->Tint = tint;
    m_renderComponent->Layer = layer;
    return *m_renderComponent;
}

void Entity::OnUpdate(float /*deltaTime*/)
{
}

void Entity::OnRender(const Renderer& /*renderer*/) const
{
}