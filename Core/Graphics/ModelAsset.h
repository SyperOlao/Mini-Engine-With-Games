#ifndef PINGPONG_MODELASSET_H
#define PINGPONG_MODELASSET_H

#include <DirectXCollision.h>

#include <d3d11.h>

#include <filesystem>
#include <memory>
#include <vector>
#include <wrl/client.h>
#include <directxtk/Model.h>

namespace DirectX
{
    inline namespace DX11
    {
        class IEffectFactory;
    }
}

class GraphicsDevice;

class ModelAsset final
{
public:
    bool LoadFromFile(
        GraphicsDevice &graphics,
        DirectX::IEffectFactory &effectFactory,
        const std::filesystem::path &resolvedFilePath
    );

    bool LoadFromCmo(
        GraphicsDevice &graphics,
        DirectX::IEffectFactory &effectFactory,
        const std::filesystem::path &resolvedFilePath
    );

    bool LoadFromSdkMesh(
        GraphicsDevice &graphics,
        DirectX::IEffectFactory &effectFactory,
        const std::filesystem::path &resolvedFilePath
    );

    [[nodiscard]] DirectX::Model *Get() const noexcept;

    [[nodiscard]] bool IsLoaded() const noexcept;

    [[nodiscard]] const std::filesystem::path &GetResolvedSourcePath() const noexcept;

    [[nodiscard]] DirectX::BoundingSphere GetMergedBoundingSphere() const noexcept;

    [[nodiscard]] ID3D11ShaderResourceView *TryGetDiffuseTextureForMeshPart(std::size_t flatMeshPartIndex) const noexcept;

private:
    std::unique_ptr<DirectX::Model> m_model{};
    std::filesystem::path m_resolvedSourcePath{};
    std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_meshPartDiffuseTextures{};
};

#endif
