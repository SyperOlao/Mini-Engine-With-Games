//
// Created by SyperOlao on 17.03.2026.
//

#include "Scene.h"

#include "../Render/Renderer.h"

void Scene::Update(const float deltaTime)
{
    for (const auto& entity : m_entities)
    {
        if (entity != nullptr)
        {
            entity->Update(deltaTime);
        }
    }
}

void Scene::Render(const Renderer& renderer) const
{
    for (const auto& entity : m_entities)
    {
        if (entity != nullptr)
        {
            entity->Render(renderer);
        }
    }
}

void Scene::AddEntity(std::unique_ptr<Entity> entity)
{
    if (entity != nullptr)
    {
        m_entities.emplace_back(std::move(entity));
    }
}

void Scene::RemoveInactiveEntities()
{
    m_entities.erase(
        std::remove_if(
            m_entities.begin(),
            m_entities.end(),
            [](const std::unique_ptr<Entity>& entity)
            {
                return entity == nullptr || !entity->IsActive();
            }
        ),
        m_entities.end()
    );
}

void Scene::Clear()
{
    m_entities.clear();
}

std::vector<std::unique_ptr<Entity>>& Scene::GetEntities() noexcept
{
    return m_entities;
}

const std::vector<std::unique_ptr<Entity>>& Scene::GetEntities() const noexcept
{
    return m_entities;
}