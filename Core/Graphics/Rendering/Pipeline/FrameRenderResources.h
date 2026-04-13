#ifndef PINGPONG_FRAMERENDERRESOURCES_H
#define PINGPONG_FRAMERENDERRESOURCES_H

class DeferredFrameResources;
class GraphicsDevice;

class FrameRenderResources final
{
public:
    void Initialize(GraphicsDevice &graphicsDevice);

    void SetDeferredFrameResources(DeferredFrameResources *deferredFrameResources) noexcept;

    void Shutdown() noexcept;

    void OnBackbufferResize(int width, int height) noexcept;

    [[nodiscard]] GraphicsDevice &GetGraphicsDevice() const;

    [[nodiscard]] DeferredFrameResources *GetDeferredFrameResources() noexcept;

    [[nodiscard]] const DeferredFrameResources *GetDeferredFrameResources() const noexcept;

    [[nodiscard]] int GetBackbufferWidth() const noexcept;

    [[nodiscard]] int GetBackbufferHeight() const noexcept;

private:
    GraphicsDevice *m_graphicsDevice{nullptr};
    DeferredFrameResources *m_deferredFrameResources{nullptr};
    int m_backbufferWidth{0};
    int m_backbufferHeight{0};
};

#endif
