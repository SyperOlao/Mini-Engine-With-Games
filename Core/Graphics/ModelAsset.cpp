#include "Core/Graphics/ModelAsset.h"

#include "Core/Graphics/GraphicsDevice.h"

#include <directxtk/Effects.h>
#include <directxtk/Model.h>

#include <cwctype>
#include <exception>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

bool ModelAssetTryLoadAssimp(
    ID3D11Device *device,
    DirectX::IEffectFactory &effectFactory,
    const std::filesystem::path &resolvedFilePath,
    std::unique_ptr<DirectX::Model> &outModel,
    std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> &outMeshPartDiffuseTextures
);

namespace
{
    std::wstring ToLowerInvariant(std::wstring value)
    {
        for (wchar_t &character : value)
        {
            character = static_cast<wchar_t>(std::towlower(character));
        }
        return value;
    }
}

bool ModelAsset::LoadFromFile(
    GraphicsDevice &graphics,
    DirectX::IEffectFactory &effectFactory,
    const std::filesystem::path &resolvedFilePath
)
{
    m_model.reset();
    m_resolvedSourcePath.clear();
    m_meshPartDiffuseTextures.clear();

    const std::wstring extensionLower = ToLowerInvariant(resolvedFilePath.extension().wstring());
    if (extensionLower == L".cmo")
    {
        return LoadFromCmo(graphics, effectFactory, resolvedFilePath);
    }
    if (extensionLower == L".sdkmesh")
    {
        return LoadFromSdkMesh(graphics, effectFactory, resolvedFilePath);
    }
    if (extensionLower == L".obj" || extensionLower == L".fbx" || extensionLower == L".gltf" || extensionLower == L".glb" ||
        extensionLower == L".dae" || extensionLower == L".blend" || extensionLower == L".3ds" || extensionLower == L".ase")
    {
        try
        {
            if (!ModelAssetTryLoadAssimp(
                graphics.GetDevice(),
                effectFactory,
                resolvedFilePath,
                m_model,
                m_meshPartDiffuseTextures
            ))
            {
                return false;
            }
        }
        catch (const std::exception &)
        {
            return false;
        }
        catch (...)
        {
            return false;
        }

        m_resolvedSourcePath = resolvedFilePath;
        return true;
    }

    return false;
}

bool ModelAsset::LoadFromSdkMesh(
    GraphicsDevice &graphics,
    DirectX::IEffectFactory &effectFactory,
    const std::filesystem::path &resolvedFilePath
)
{
    m_model.reset();
    m_resolvedSourcePath.clear();
    m_meshPartDiffuseTextures.clear();

    m_model = DirectX::Model::CreateFromSDKMESH(graphics.GetDevice(), resolvedFilePath.c_str(), effectFactory);
    if (m_model == nullptr)
    {
        return false;
    }

    m_resolvedSourcePath = resolvedFilePath;
    return true;
}

bool ModelAsset::LoadFromCmo(
    GraphicsDevice &graphics,
    DirectX::IEffectFactory &effectFactory,
    const std::filesystem::path &resolvedFilePath
)
{
    m_model.reset();
    m_resolvedSourcePath.clear();
    m_meshPartDiffuseTextures.clear();

    m_model = DirectX::Model::CreateFromCMO(graphics.GetDevice(), resolvedFilePath.c_str(), effectFactory);
    if (m_model == nullptr)
    {
        return false;
    }

    m_resolvedSourcePath = resolvedFilePath;
    return true;
}

DirectX::Model *ModelAsset::Get() const noexcept
{
    return m_model.get();
}

bool ModelAsset::IsLoaded() const noexcept
{
    return m_model != nullptr;
}

const std::filesystem::path &ModelAsset::GetResolvedSourcePath() const noexcept
{
    return m_resolvedSourcePath;
}

DirectX::BoundingSphere ModelAsset::GetMergedBoundingSphere() const noexcept
{
    if (m_model == nullptr || m_model->meshes.empty())
    {
        return {};
    }

    DirectX::BoundingSphere merged = m_model->meshes[0]->boundingSphere;
    for (size_t meshIndex = 1; meshIndex < m_model->meshes.size(); ++meshIndex)
    {
        DirectX::BoundingSphere combined{};
        DirectX::BoundingSphere::CreateMerged(combined, merged, m_model->meshes[meshIndex]->boundingSphere);
        merged = combined;
    }

    return merged;
}

ID3D11ShaderResourceView *ModelAsset::TryGetDiffuseTextureForMeshPart(const std::size_t flatMeshPartIndex) const noexcept
{
    if (flatMeshPartIndex >= m_meshPartDiffuseTextures.size())
    {
        return nullptr;
    }

    return m_meshPartDiffuseTextures[flatMeshPartIndex].Get();
}
