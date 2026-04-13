#include "Core/Graphics/Rendering/Pipeline/FrameRenderPipeline.h"

#include "Core/Graphics/GraphicsDevice.h"
#include "Core/Graphics/Rendering/Pipeline/FramePassRenderContext.h"
#include "Core/Graphics/Rendering/Pipeline/FrameRenderResources.h"

#include <utility>

FrameRenderPipeline::~FrameRenderPipeline()
{
    Shutdown();
}

void FrameRenderPipeline::Initialize(GraphicsDevice &graphicsDevice, FrameRenderResources &frameRenderResources)
{
    m_graphicsDevice = &graphicsDevice;
    m_frameRenderResources = &frameRenderResources;

    for (std::unique_ptr<IRenderPass> &pass : m_passes)
    {
        if (pass != nullptr)
        {
            pass->Initialize(graphicsDevice, frameRenderResources);
        }
    }

    m_initialized = true;
}

void FrameRenderPipeline::Shutdown()
{
    for (std::unique_ptr<IRenderPass> &pass : m_passes)
    {
        if (pass != nullptr)
        {
            pass->Shutdown();
        }
    }

    m_passes.clear();
    m_initialized = false;
    m_graphicsDevice = nullptr;
    m_frameRenderResources = nullptr;
}

void FrameRenderPipeline::Resize(const int width, const int height)
{
    for (std::unique_ptr<IRenderPass> &pass : m_passes)
    {
        if (pass != nullptr)
        {
            pass->Resize(width, height);
        }
    }
}

void FrameRenderPipeline::ClearPasses()
{
    Shutdown();
}

void FrameRenderPipeline::AddPass(std::unique_ptr<IRenderPass> renderPass)
{
    if (renderPass == nullptr)
    {
        return;
    }

    if (m_initialized && m_graphicsDevice != nullptr && m_frameRenderResources != nullptr)
    {
        renderPass->Initialize(*m_graphicsDevice, *m_frameRenderResources);
    }

    m_passes.push_back(std::move(renderPass));
}

void FrameRenderPipeline::InsertPass(const std::size_t index, std::unique_ptr<IRenderPass> renderPass)
{
    if (renderPass == nullptr)
    {
        return;
    }

    if (m_initialized && m_graphicsDevice != nullptr && m_frameRenderResources != nullptr)
    {
        renderPass->Initialize(*m_graphicsDevice, *m_frameRenderResources);
    }

    if (index > m_passes.size())
    {
        m_passes.push_back(std::move(renderPass));
        return;
    }

    m_passes.insert(m_passes.begin() + static_cast<std::ptrdiff_t>(index), std::move(renderPass));
}

void FrameRenderPipeline::ExecuteFrame(FramePassRenderContext &framePassRenderContext)
{
    for (std::unique_ptr<IRenderPass> &pass : m_passes)
    {
        if (pass != nullptr)
        {
            pass->Execute(framePassRenderContext);
        }
    }
}

std::size_t FrameRenderPipeline::GetPassCount() const noexcept
{
    return m_passes.size();
}
