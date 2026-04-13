#ifndef PINGPONG_FRAMEPASSRENDERCONTEXT_H
#define PINGPONG_FRAMEPASSRENDERCONTEXT_H

#include <SimpleMath.h>
#include <functional>

class Camera;
class GraphicsDevice;
class FrameRenderResources;
class RenderContext;
class Scene;

class FramePassRenderContext final
{
public:
    FramePassRenderContext(
        RenderContext &renderContext,
        FrameRenderResources &frameRenderResources,
        Camera *activeCamera,
        Scene *gameplayScene,
        std::function<void()> gameRenderCallback,
        float viewportWidthPixels,
        float viewportHeightPixels,
        float frameDeltaTimeSeconds
    );

    [[nodiscard]] RenderContext &GetRenderContext() noexcept;

    [[nodiscard]] const RenderContext &GetRenderContext() const noexcept;

    [[nodiscard]] FrameRenderResources &GetFrameRenderResources() noexcept;

    [[nodiscard]] const FrameRenderResources &GetFrameRenderResources() const noexcept;

    [[nodiscard]] GraphicsDevice &GetGraphicsDevice() const;

    [[nodiscard]] Camera *GetActiveCamera() noexcept;

    [[nodiscard]] const Camera *GetActiveCamera() const noexcept;

    [[nodiscard]] Scene *GetGameplayScene() noexcept;

    [[nodiscard]] const Scene *GetGameplayScene() const noexcept;

    void ExecuteGameRenderCallback() const;
    [[nodiscard]] bool HasGameRenderCallback() const noexcept;

    [[nodiscard]] float GetViewportWidthPixels() const noexcept;

    [[nodiscard]] float GetViewportHeightPixels() const noexcept;

    [[nodiscard]] float GetFrameDeltaTimeSeconds() const noexcept;

    [[nodiscard]] float GetViewportAspectRatio() const noexcept;

private:
    RenderContext &m_renderContext;
    FrameRenderResources &m_frameRenderResources;
    Camera *m_activeCamera{nullptr};
    Scene *m_gameplayScene{nullptr};
    std::function<void()> m_gameRenderCallback{};
    float m_viewportWidthPixels{0.0f};
    float m_viewportHeightPixels{0.0f};
    float m_frameDeltaTimeSeconds{0.0f};
};

#endif
