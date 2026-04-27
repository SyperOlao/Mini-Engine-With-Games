#ifndef PINGPONG_KATAMARILEVELSETUP_H
#define PINGPONG_KATAMARILEVELSETUP_H

#include "Game/Katamari/Data/KatamariWorldContext.h"

#include <memory>
#include <vector>

struct PickupArchetype;

struct AppContext;
class Scene;
class ModelAsset;

namespace KatamariLevelSetup
{
[[nodiscard]] float SphereVolumeFromRadius(float radius) noexcept;

void CreateStaticWorldCollision(Scene &scene, KatamariGameConfig const &config);

void CreateStaticObstacles(
    Scene &scene,
    KatamariWorldContext &world,
    KatamariGameConfig const &config,
    std::shared_ptr<ModelAsset> const &cubeModel,
    std::shared_ptr<ModelAsset> const &triangularPrismModel
);

void CreatePlayerBall(
    Scene &scene,
    KatamariWorldContext &world,
    std::shared_ptr<ModelAsset> const &ballMeshModel
);

void SpawnPlayerBallAndPickups(
    Scene &scene,
    AppContext &context,
    KatamariWorldContext &world,
    KatamariGameConfig const &config,
    std::vector<PickupArchetype> const &archetypes,
    std::shared_ptr<ModelAsset> const &ballMeshModel
);
}

#endif
