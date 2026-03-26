#ifndef PINGPONG_MODELRENDERER_H
#define PINGPONG_MODELRENDERER_H

#include <SimpleMath.h>
#include <d3d11.h>
#include <directxtk/CommonStates.h>
#include <memory>
#include <wrl/client.h>

#include "Core/Graphics/Rendering/Lighting/SceneLighting3D.h"
#include "Core/Graphics/Rendering/Renderables/RenderMaterialParameters.h"

class GraphicsDevice;
class ModelAsset;
class RenderContext;

class ModelRenderer final
{
public:
    void Initialize(GraphicsDevice &graphics);

    void SetShadowBindingHost(RenderContext *host) noexcept;

    void DrawModelShadowDepth(
        ID3D11RasterizerState *shadowRasterizerState,
        const ModelAsset &model,
        const DirectX::SimpleMath::Matrix &world,
        const DirectX::SimpleMath::Matrix &lightViewProjection
    ) const;

    void DrawModel(
        const ModelAsset &model,
        const DirectX::SimpleMath::Matrix &world,
        const DirectX::SimpleMath::Matrix &view,
        const DirectX::SimpleMath::Matrix &projection,
        const RenderMaterialParameters &material
    ) const;

    void DrawModelLit(
        const ModelAsset &model,
        const DirectX::SimpleMath::Matrix &world,
        const DirectX::SimpleMath::Matrix &view,
        const DirectX::SimpleMath::Matrix &projection,
        const DirectX::SimpleMath::Vector3 &cameraWorldPosition,
        const SceneLightingDescriptor3D &lighting,
        const RenderMaterialParameters &material
    ) const;

private:
    void CreateForwardPhongResources(GraphicsDevice &graphics);

    void CreateShadowDepthPassResources(GraphicsDevice &graphics);

private:
    GraphicsDevice *m_graphics{nullptr};
    RenderContext *m_shadowBindingHost{nullptr};
    std::unique_ptr<DirectX::CommonStates> m_commonStates{};

    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_forwardPhongModelVertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_forwardPhongPixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_forwardPhongModelInputLayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_forwardPhongCameraConstantBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_forwardPhongObjectConstantBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_forwardPhongMaterialConstantBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_forwardPhongLightsConstantBuffer;

    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_shadowDepthVertexShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_shadowDepthInputLayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_shadowDepthConstantBuffer;
};

#endif
