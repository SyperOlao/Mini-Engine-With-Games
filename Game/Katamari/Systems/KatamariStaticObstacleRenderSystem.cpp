#include "Game/Katamari/Systems/KatamariStaticObstacleRenderSystem.h"

#include "Core/App/AppContext.h"
#include "Core/Gameplay/GameplayComponents.h"
#include "Core/Gameplay/Scene.h"
#include "Core/Graphics/Camera.h"
#include "Core/Graphics/Rendering/Lighting/SceneLighting3D.h"
#include "Core/Graphics/Rendering/PrimitiveRenderer3D.h"
#include "Core/Graphics/Rendering/RenderContext.h"
#include "Core/Platform/Window.h"

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;

KatamariStaticObstacleRenderSystem::KatamariStaticObstacleRenderSystem(KatamariWorldContext *const world) noexcept
    : World(world)
{
}

void KatamariStaticObstacleRenderSystem::Initialize(Scene &, AppContext &)
{
}

void KatamariStaticObstacleRenderSystem::Update(Scene &, AppContext &, float)
{
}

void KatamariStaticObstacleRenderSystem::Render(Scene &scene, AppContext &context)
{
    if (World == nullptr || World->StaticObstacles.empty())
    {
        return;
    }

    const Camera *const camera = scene.GetActiveCamera();
    const Window *const window = context.Platform.MainWindow;
    if (camera == nullptr || window == nullptr || context.Graphics.Render == nullptr)
    {
        return;
    }

    if (context.GetRenderMode() == RenderMode::Deferred
        && context.Graphics.Render->GetActiveRenderPassKind() != RenderPassKind::DeferredGeometry)
    {
        return;
    }

    const int width = window->GetWidth();
    const int height = window->GetHeight();
    if (width <= 0 || height <= 0)
    {
        return;
    }

    const float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    const Matrix view = camera->GetViewMatrix();
    const Matrix projection = camera->GetProjectionMatrix(aspectRatio);
    const Vector3 cameraWorldPosition = CameraWorldPositionFromViewMatrix(view);

    PrimitiveRenderer3D &primitives = context.GetPrimitiveRenderer3D();
    const SceneLightingDescriptor3D &lighting = scene.GetSceneLightingDescriptor();

    for (const KatamariStaticObstacleRecord &obstacle : World->StaticObstacles)
    {
        const ModelComponent *const model = scene.TryGetModelComponent(obstacle.Entity);
        if (model != nullptr && model->Visible && model->Asset != nullptr)
        {
            continue;
        }

        const TransformComponent *const transform = scene.TryGetTransformComponent(obstacle.Entity);
        if (transform == nullptr)
        {
            continue;
        }

        const Matrix world = transform->WorldMatrix;
        if (obstacle.Shape == KatamariObstacleShape::TriangularPrism)
        {
            primitives.DrawTriangularPrismLit(
                world,
                view,
                projection,
                cameraWorldPosition,
                lighting,
                obstacle.Material
            );
            continue;
        }

        primitives.DrawBoxLit(
            world,
            view,
            projection,
            cameraWorldPosition,
            lighting,
            obstacle.Material
        );
    }
}
