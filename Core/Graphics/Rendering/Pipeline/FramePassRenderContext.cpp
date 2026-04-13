#include "Core/Graphics/Rendering/Pipeline/FramePassRenderContext.h"

#include "Core/Graphics/GraphicsDevice.h"
#include "Core/Graphics/Rendering/Pipeline/FrameRenderResources.h"
#include "Core/Graphics/Rendering/RenderContext.h"
#include <utility>

FramePassRenderContext::FramePassRenderContext(
    RenderContext &renderContext,
    FrameRenderResources &frameRenderResources,
    Camera *const activeCamera,
    Scene *const gameplayScene,
    std::function<void()> gameRenderCallback,
    const float viewportWidthPixels,
    const float viewportHeightPixels,
    const float frameDeltaTimeSeconds
)
    : m_renderContext(renderContext)
    , m_frameRenderResources(frameRenderResources)
    , m_activeCamera(activeCamera)
    , m_gameplayScene(gameplayScene)
    , m_gameRenderCallback(std::move(gameRenderCallback))
    , m_viewportWidthPixels(viewportWidthPixels)
    , m_viewportHeightPixels(viewportHeightPixels)
    , m_frameDeltaTimeSeconds(frameDeltaTimeSeconds)
{
}

RenderContext &FramePassRenderContext::GetRenderContext() noexcept
{
    return m_renderContext;
}

const RenderContext &FramePassRenderContext::GetRenderContext() const noexcept
{
    return m_renderContext;
}

FrameRenderResources &FramePassRenderContext::GetFrameRenderResources() noexcept
{
    return m_frameRenderResources;
}

const FrameRenderResources &FramePassRenderContext::GetFrameRenderResources() const noexcept
{
    return m_frameRenderResources;
}

GraphicsDevice &FramePassRenderContext::GetGraphicsDevice() const
{
    return m_frameRenderResources.GetGraphicsDevice();
}

Camera *FramePassRenderContext::GetActiveCamera() noexcept
{
    return m_activeCamera;
}

const Camera *FramePassRenderContext::GetActiveCamera() const noexcept
{
    return m_activeCamera;
}

Scene *FramePassRenderContext::GetGameplayScene() noexcept
{
    return m_gameplayScene;
}

const Scene *FramePassRenderContext::GetGameplayScene() const noexcept
{
    return m_gameplayScene;
}

void FramePassRenderContext::ExecuteGameRenderCallback() const
{
    if (m_gameRenderCallback)
    {
        m_gameRenderCallback();
    }
}

bool FramePassRenderContext::HasGameRenderCallback() const noexcept
{
    return static_cast<bool>(m_gameRenderCallback);
}

float FramePassRenderContext::GetViewportWidthPixels() const noexcept
{
    return m_viewportWidthPixels;
}

float FramePassRenderContext::GetViewportHeightPixels() const noexcept
{
    return m_viewportHeightPixels;
}

float FramePassRenderContext::GetFrameDeltaTimeSeconds() const noexcept
{
    return m_frameDeltaTimeSeconds;
}

float FramePassRenderContext::GetViewportAspectRatio() const noexcept
{
    if (m_viewportHeightPixels <= 0.0f)
    {
        return 1.0f;
    }

    return m_viewportWidthPixels / m_viewportHeightPixels;
}
