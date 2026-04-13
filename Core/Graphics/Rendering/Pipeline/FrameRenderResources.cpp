#include "Core/Graphics/Rendering/Pipeline/FrameRenderResources.h"

#include "Core/Graphics/GraphicsDevice.h"
#include "Core/Graphics/Rendering/Deferred/DeferredFrameResources.h"

void FrameRenderResources::Initialize(GraphicsDevice &graphicsDevice)
{
    m_graphicsDevice = &graphicsDevice;
    m_backbufferWidth = graphicsDevice.GetWidth();
    m_backbufferHeight = graphicsDevice.GetHeight();
}

void FrameRenderResources::SetDeferredFrameResources(DeferredFrameResources *const deferredFrameResources) noexcept
{
    m_deferredFrameResources = deferredFrameResources;
}

void FrameRenderResources::Shutdown() noexcept
{
    m_graphicsDevice = nullptr;
    m_deferredFrameResources = nullptr;
    m_backbufferWidth = 0;
    m_backbufferHeight = 0;
}

void FrameRenderResources::OnBackbufferResize(const int width, const int height) noexcept
{
    m_backbufferWidth = width;
    m_backbufferHeight = height;
}

GraphicsDevice &FrameRenderResources::GetGraphicsDevice() const
{
    return *m_graphicsDevice;
}

DeferredFrameResources *FrameRenderResources::GetDeferredFrameResources() noexcept
{
    return m_deferredFrameResources;
}

const DeferredFrameResources *FrameRenderResources::GetDeferredFrameResources() const noexcept
{
    return m_deferredFrameResources;
}

int FrameRenderResources::GetBackbufferWidth() const noexcept
{
    return m_backbufferWidth;
}

int FrameRenderResources::GetBackbufferHeight() const noexcept
{
    return m_backbufferHeight;
}
