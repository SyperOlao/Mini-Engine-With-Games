#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "Core/Graphics/D3d11Helpers.h"

#include <directxtk/Effects.h>
#include <directxtk/Model.h>
#include <directxtk/VertexTypes.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <d3d11.h>

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace
{
    DirectX::XMMATRIX AssimpMatrixToDirectXMatrix(const aiMatrix4x4 &matrixAssimp)
    {
        return DirectX::XMMATRIX(
            matrixAssimp.a1,
            matrixAssimp.b1,
            matrixAssimp.c1,
            matrixAssimp.d1,
            matrixAssimp.a2,
            matrixAssimp.b2,
            matrixAssimp.c2,
            matrixAssimp.d2,
            matrixAssimp.a3,
            matrixAssimp.b3,
            matrixAssimp.c3,
            matrixAssimp.d3,
            matrixAssimp.a4,
            matrixAssimp.b4,
            matrixAssimp.c4,
            matrixAssimp.d4
        );
    }

    void AppendAiMesh(
        const aiMesh *mesh,
        const DirectX::XMMATRIX &world,
        std::vector<DirectX::VertexPositionNormalTexture> &vertices,
        std::vector<std::uint32_t> &indices
    )
    {
        const std::uint32_t BaseVertex = static_cast<std::uint32_t>(vertices.size());

        for (unsigned vertexIndex = 0u; vertexIndex < mesh->mNumVertices; ++vertexIndex)
        {
            const aiVector3D &positionLocal = mesh->mVertices[vertexIndex];
            DirectX::XMVECTOR positionVector = DirectX::XMVectorSet(positionLocal.x, positionLocal.y, positionLocal.z, 1.0f);
            positionVector = DirectX::XMVector3Transform(positionVector, world);

            DirectX::XMVECTOR normalVector = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
            if (mesh->HasNormals())
            {
                const aiVector3D &normalLocal = mesh->mNormals[vertexIndex];
                normalVector = DirectX::XMVectorSet(normalLocal.x, normalLocal.y, normalLocal.z, 0.0f);
                normalVector = DirectX::XMVector3TransformNormal(normalVector, world);
                normalVector = DirectX::XMVector3Normalize(normalVector);
            }

            DirectX::XMFLOAT2 textureCoordinate(0.0f, 0.0f);
            if (mesh->HasTextureCoords(0))
            {
                const aiVector3D &textureLocal = mesh->mTextureCoords[0][vertexIndex];
                textureCoordinate.x = textureLocal.x;
                textureCoordinate.y = textureLocal.y;
            }

            DirectX::XMFLOAT3 positionStored{};
            DirectX::XMFLOAT3 normalStored{};
            DirectX::XMStoreFloat3(&positionStored, positionVector);
            DirectX::XMStoreFloat3(&normalStored, normalVector);

            vertices.emplace_back(positionStored, normalStored, textureCoordinate);
        }

        for (unsigned faceIndex = 0u; faceIndex < mesh->mNumFaces; ++faceIndex)
        {
            const aiFace &face = mesh->mFaces[faceIndex];
            for (unsigned cornerIndex = 0u; cornerIndex < face.mNumIndices; ++cornerIndex)
            {
                indices.push_back(BaseVertex + face.mIndices[cornerIndex]);
            }
        }
    }

    void ProcessAssimpNode(
        const aiScene *scene,
        aiNode *node,
        const aiMatrix4x4 &parentTransform,
        std::vector<DirectX::VertexPositionNormalTexture> &vertices,
        std::vector<std::uint32_t> &indices
    )
    {
        const aiMatrix4x4 transform = parentTransform * node->mTransformation;
        const DirectX::XMMATRIX world = AssimpMatrixToDirectXMatrix(transform);

        for (unsigned meshSlot = 0u; meshSlot < node->mNumMeshes; ++meshSlot)
        {
            const aiMesh *mesh = scene->mMeshes[node->mMeshes[meshSlot]];
            AppendAiMesh(mesh, world, vertices, indices);
        }

        for (unsigned childIndex = 0u; childIndex < node->mNumChildren; ++childIndex)
        {
            ProcessAssimpNode(scene, node->mChildren[childIndex], transform, vertices, indices);
        }
    }
}

bool ModelAssetTryLoadAssimp(
    ID3D11Device *device,
    DirectX::IEffectFactory &effectFactory,
    const std::filesystem::path &resolvedFilePath,
    std::unique_ptr<DirectX::Model> &outModel
)
{
    outModel.reset();

    if (device == nullptr)
    {
        return false;
    }

    const std::u8string pathAsU8 = resolvedFilePath.u8string();
    const std::string pathUtf8(reinterpret_cast<const char *>(pathAsU8.data()), pathAsU8.size());

    Assimp::Importer importer;
    constexpr unsigned importFlags =
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_GenSmoothNormals |
        aiProcess_ImproveCacheLocality |
        aiProcess_ConvertToLeftHanded |
        aiProcess_SortByPType;

    const aiScene *scene = importer.ReadFile(pathUtf8.c_str(), importFlags);
    if (scene == nullptr || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) != 0u || scene->mRootNode == nullptr)
    {
        return false;
    }

    std::vector<DirectX::VertexPositionNormalTexture> vertices{};
    std::vector<std::uint32_t> indices{};

    aiMatrix4x4 identity{};
    identity.a1 = 1.0f;
    identity.b2 = 1.0f;
    identity.c3 = 1.0f;
    identity.d4 = 1.0f;
    ProcessAssimpNode(scene, scene->mRootNode, identity, vertices, indices);

    if (vertices.empty() || indices.empty())
    {
        return false;
    }

    D3D11_BUFFER_DESC vertexBufferDescription{};
    vertexBufferDescription.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDescription.ByteWidth = static_cast<UINT>(sizeof(DirectX::VertexPositionNormalTexture) * vertices.size());
    vertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vertexSubresource{};
    vertexSubresource.pSysMem = vertices.data();

    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    D3d11Helpers::ThrowIfFailed(
        device->CreateBuffer(&vertexBufferDescription, &vertexSubresource, vertexBuffer.GetAddressOf()),
        "Assimp model load failed to create vertex buffer."
    );

    D3D11_BUFFER_DESC indexBufferDescription{};
    indexBufferDescription.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDescription.ByteWidth = static_cast<UINT>(sizeof(std::uint32_t) * indices.size());
    indexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA indexSubresource{};
    indexSubresource.pSysMem = indices.data();

    Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
    D3d11Helpers::ThrowIfFailed(
        device->CreateBuffer(&indexBufferDescription, &indexSubresource, indexBuffer.GetAddressOf()),
        "Assimp model load failed to create index buffer."
    );

    DirectX::IEffectFactory::EffectInfo effectInfo{};
    effectInfo.name = nullptr;
    effectInfo.perVertexColor = false;
    effectInfo.enableSkinning = false;
    effectInfo.enableDualTexture = false;
    effectInfo.enableNormalMaps = false;
    effectInfo.biasedVertexNormals = false;
    effectInfo.specularPower = 32.0f;
    effectInfo.alpha = 1.0f;
    effectInfo.ambientColor = DirectX::XMFLOAT3(0.18f, 0.18f, 0.18f);
    effectInfo.diffuseColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
    effectInfo.specularColor = DirectX::XMFLOAT3(0.25f, 0.25f, 0.25f);
    effectInfo.emissiveColor = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

    std::shared_ptr<DirectX::IEffect> effect = effectFactory.CreateEffect(effectInfo, nullptr);
    if (effect == nullptr)
    {
        return false;
    }

    auto meshPart = std::make_unique<DirectX::ModelMeshPart>();
    meshPart->indexCount = static_cast<std::uint32_t>(indices.size());
    meshPart->startIndex = 0u;
    meshPart->vertexOffset = 0;
    meshPart->vertexStride = sizeof(DirectX::VertexPositionNormalTexture);
    meshPart->primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    meshPart->indexFormat = DXGI_FORMAT_R32_UINT;
    meshPart->indexBuffer = indexBuffer;
    meshPart->vertexBuffer = vertexBuffer;
    meshPart->effect = effect;
    meshPart->isAlpha = false;

    meshPart->vbDecl = std::make_shared<DirectX::ModelMeshPart::InputLayoutCollection>();
    meshPart->vbDecl->assign(
        DirectX::VertexPositionNormalTexture::InputElements,
        DirectX::VertexPositionNormalTexture::InputElements + DirectX::VertexPositionNormalTexture::InputElementCount
    );

    meshPart->CreateInputLayout(device, meshPart->effect.get(), meshPart->inputLayout.GetAddressOf());

    DirectX::BoundingSphere boundingSphere{};
    DirectX::BoundingSphere::CreateFromPoints(
        boundingSphere,
        vertices.size(),
        reinterpret_cast<const DirectX::XMFLOAT3 *>(vertices.data()),
        sizeof(DirectX::VertexPositionNormalTexture)
    );

    DirectX::BoundingBox boundingBox{};
    DirectX::BoundingBox::CreateFromPoints(
        boundingBox,
        vertices.size(),
        reinterpret_cast<const DirectX::XMFLOAT3 *>(vertices.data()),
        sizeof(DirectX::VertexPositionNormalTexture)
    );

    auto mesh = std::make_shared<DirectX::ModelMesh>();
    mesh->boundingSphere = boundingSphere;
    mesh->boundingBox = boundingBox;
    mesh->meshParts.push_back(std::move(meshPart));
    mesh->ccw = true;
    mesh->pmalpha = false;
    mesh->name = L"assimp";

    auto model = std::make_unique<DirectX::Model>();
    model->name = L"assimp";
    model->meshes.push_back(mesh);

    outModel = std::move(model);
    return true;
}
