#ifndef PINGPONG_KATAMARISTATICOBSTACLERENDERSYSTEM_H
#define PINGPONG_KATAMARISTATICOBSTACLERENDERSYSTEM_H

#include "Core/Gameplay/ISceneSystem.h"
#include "Game/Katamari/Data/KatamariWorldContext.h"

class KatamariStaticObstacleRenderSystem final : public ISceneSystem
{
public:
    explicit KatamariStaticObstacleRenderSystem(KatamariWorldContext *world) noexcept;

    void Initialize(Scene &scene, AppContext &context) override;
    void Update(Scene &scene, AppContext &context, float deltaTime) override;
    void Render(Scene &scene, AppContext &context) override;

private:
    KatamariWorldContext *World{nullptr};
};

#endif
