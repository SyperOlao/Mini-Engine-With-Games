#include "Core/Gameplay/RenderSystem.h"

#include "Core/App/AppContext.h"
#include "Core/Gameplay/Entity.h"
#include "Core/Gameplay/Scene.h"
#include "Core/Graphics/Camera.h"
#include "Core/Graphics/ModelAsset.h"
#include "Core/Graphics/ModelRenderer.h"
#include "Core/Graphics/Rendering/Lighting/SceneLighting3D.h"
#include "Core/Graphics/Rendering/Renderables/RenderMaterialParameters.h"
#include "Core/Platform/Window.h"

void RenderSystem::Initialize(Scene &, AppContext &)
{
}

void RenderSystem::Update(Scene &, AppContext &, float)
{
}

void RenderSystem::Render(Scene &scene, AppContext &context)
{
    const Camera *const camera = scene.GetActiveCamera();
    if (camera == nullptr)
    {
        return;
    }

    Window *const window = context.Platform.MainWindow;
    if (window == nullptr)
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
    const DirectX::SimpleMath::Matrix viewMatrix = camera->GetViewMatrix();
    const DirectX::SimpleMath::Matrix projectionMatrix = camera->GetProjectionMatrix(aspectRatio);
    const DirectX::SimpleMath::Vector3 cameraWorldPosition = CameraWorldPositionFromViewMatrix(viewMatrix);

    scene.ForEachEntityWithTransformAndModel([&](Entity &entity) {
        ModelComponent *const model = entity.TryGetModelComponent();
        TransformComponent *const transform = entity.TryGetTransformComponent();
        if (model == nullptr || transform == nullptr)
        {
            return;
        }

        if (!model->Visible || model->Asset == nullptr)
        {
            return;
        }

        RenderMaterialParameters material{};
        const MaterialComponent *const materialComponent = entity.TryGetMaterialComponent();
        if (materialComponent != nullptr)
        {
            material = materialComponent->Parameters;
        }

        if (scene.GetForwardLightingEnabled() && material.ReceiveLighting)
        {
            context.GetModelRenderer().DrawModelLit(
                *model->Asset,
                transform->WorldMatrix,
                viewMatrix,
                projectionMatrix,
                cameraWorldPosition,
                scene.GetSceneLightingDescriptor(),
                material
            );
        }
        else
        {
            context.GetModelRenderer().DrawModel(
                *model->Asset,
                transform->WorldMatrix,
                viewMatrix,
                projectionMatrix,
                material
            );
        }
    });
}
