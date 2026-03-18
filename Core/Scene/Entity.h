//
// Created by SyperOlao on 17.03.2026.
//

#ifndef PINGPONG_ENTITY_H
#define PINGPONG_ENTITY_H
#include <optional>
#include <string>

#include "Transform2D.h"
#include "Components/Collider2D.h"
#include "Components/MovementComponent.h"
#include "Components/RenderComponent2D.h"

class Renderer;

class Entity
{
public:
    explicit Entity(std::string name = "Entity");
    virtual ~Entity() = default;

    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;
    Entity(Entity&&) = default;
    Entity& operator=(Entity&&) = default;

    virtual void Update(float deltaTime);
    virtual void Render(const Renderer& renderer) const;

    [[nodiscard]] const std::string& GetName() const noexcept;
    [[nodiscard]] bool IsActive() const noexcept;
    void SetActive(bool isActive) noexcept;

    [[nodiscard]] Transform2D& GetTransform() noexcept;
    [[nodiscard]] const Transform2D& GetTransform() const noexcept;

    [[nodiscard]] std::optional<MovementComponent>& GetMovement() noexcept;
    [[nodiscard]] const std::optional<MovementComponent>& GetMovement() const noexcept;

    [[nodiscard]] std::optional<Collider2D>& GetCollider() noexcept;
    [[nodiscard]] const std::optional<Collider2D>& GetCollider() const noexcept;

    [[nodiscard]] std::optional<RenderComponent2D>& GetRenderComponent() noexcept;
    [[nodiscard]] const std::optional<RenderComponent2D>& GetRenderComponent() const noexcept;

    MovementComponent& AddMovementComponent();

    Collider2D& AddColliderComponent(
        const DirectX::SimpleMath::Vector2& halfSize,
        const DirectX::SimpleMath::Vector2& offset = {0.0f, 0.0f}
    );

    RenderComponent2D& AddRenderComponent(
        const DirectX::SimpleMath::Vector2& size,
        const Color& tint = {1.0f, 1.0f, 1.0f, 1.0f},
        int layer = 0
    );

protected:
    virtual void OnUpdate(float deltaTime);
    virtual void OnRender(const Renderer& renderer) const;

private:
    std::string m_name;
    bool m_isActive{true};
    Transform2D m_transform{};
    std::optional<MovementComponent> m_movement{};
    std::optional<Collider2D> m_collider{};
    std::optional<RenderComponent2D> m_renderComponent{};
};

#endif //PINGPONG_ENTITY_H