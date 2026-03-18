//
// Created by SyperOlao on 17.03.2026.
//

#ifndef PINGPONG_SCENE_H
#define PINGPONG_SCENE_H
#include <algorithm>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include "Entity.h"

class Renderer;

class Scene
{
public:
    Scene() = default;
    virtual ~Scene() = default;

    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&) = delete;
    Scene& operator=(Scene&&) = delete;

    virtual void Update(float deltaTime);
    virtual void Render(const Renderer& renderer) const;

    template <typename TEntity, typename... TArgs>
    TEntity& CreateEntity(TArgs&&... args)
    {
        static_assert(std::is_base_of_v<Entity, TEntity>, "TEntity must derive from Entity");

        auto entity = std::make_unique<TEntity>(std::forward<TArgs>(args)...);
        TEntity& reference = *entity;
        m_entities.emplace_back(std::move(entity));
        return reference;
    }

    void AddEntity(std::unique_ptr<Entity> entity);
    void RemoveInactiveEntities();
    void Clear();

    [[nodiscard]] std::vector<std::unique_ptr<Entity>>& GetEntities() noexcept;
    [[nodiscard]] const std::vector<std::unique_ptr<Entity>>& GetEntities() const noexcept;

protected:
    std::vector<std::unique_ptr<Entity>> m_entities{};
};

#endif //PINGPONG_SCENE_H