#ifndef PINGPONG_KATAMARIBALLVISUALRADIUSSYSTEM_H
#define PINGPONG_KATAMARIBALLVISUALRADIUSSYSTEM_H

#include "Core/Gameplay/ISceneSystem.h"

struct KatamariWorldContext;

class KatamariBallVisualRadiusSystem final : public ISceneSystem
{
public:
    explicit KatamariBallVisualRadiusSystem(KatamariWorldContext *gameplayWorld) noexcept;

    void Initialize(Scene &scene, AppContext &context) override;

    void Update(Scene &scene, AppContext &context, float deltaTime) override;

private:
    KatamariWorldContext *GameplayWorld{nullptr};
};

#endif
