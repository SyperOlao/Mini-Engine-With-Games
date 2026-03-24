//
// Created by SyperOlao on 19.03.2026.
//
#include "Core/Graphics/MeshGenerator.h"

#include <SimpleMath.h>
using DirectX::SimpleMath::Color;
using DirectX::SimpleMath::Vector3;


MeshData MeshGenerator::CreateBox(float width, float height, float depth) {
    MeshData mesh{};

    const float hx = width * 0.5f;
    const float hy = height * 0.5f;
    const float hz = depth * 0.5f;

    constexpr auto color = Color(1.0f, 1.0f, 1.0f, 1.0f);

    mesh.Vertices =
    {
        Vertex3D(Vector3(-hx, -hy, -hz), color),
        Vertex3D(Vector3(-hx, +hy, -hz), color),
        Vertex3D(Vector3(+hx, +hy, -hz), color),
        Vertex3D(Vector3(+hx, -hy, -hz), color),

        Vertex3D(Vector3(-hx, -hy, +hz), color),
        Vertex3D(Vector3(-hx, +hy, +hz), color),
        Vertex3D(Vector3(+hx, +hy, +hz), color),
        Vertex3D(Vector3(+hx, -hy, +hz), color),
    };

    mesh.Indices =
    {
        0, 1, 2,  0, 2, 3,
        4, 6, 5,  4, 7, 6,
        4, 5, 1,  4, 1, 0,
        3, 2, 6,  3, 6, 7,
        1, 5, 6,  1, 6, 2,
        4, 0, 3,  4, 3, 7
    };

    return mesh;
}

MeshData MeshGenerator::CreateSphere(float radius, int slices, int stacks) {
    MeshData mesh{};

    if (slices < 3 || stacks < 2) {
        return mesh;
    }

    constexpr auto color = Color(1.0f, 1.0f, 1.0f, 1.0f);

    for (int stack = 0; stack <= stacks; ++stack) {
        const float v = static_cast<float>(stack) / static_cast<float>(stacks);
        const float phi = DirectX::XM_PI * v;

        const float y = radius * std::cos(phi);
        const float ringRadius = radius * std::sin(phi);

        for (int slice = 0; slice <= slices; ++slice) {
            const float u = static_cast<float>(slice) / static_cast<float>(slices);
            const float theta = DirectX::XM_2PI * u;

            const float x = ringRadius * std::cos(theta);
            const float z = ringRadius * std::sin(theta);

            mesh.Vertices.emplace_back(Vector3(x, y, z), color);
        }
    }

    const int ringVertexCount = slices + 1;

    for (int stack = 0; stack < stacks; ++stack) {
        for (int slice = 0; slice < slices; ++slice) {
            const std::uint32_t i0 = static_cast<std::uint32_t>(stack * ringVertexCount + slice);
            const std::uint32_t i1 = i0 + 1;
            const std::uint32_t i2 = static_cast<std::uint32_t>((stack + 1) * ringVertexCount + slice);
            const std::uint32_t i3 = i2 + 1;

            mesh.Indices.push_back(i0);
            mesh.Indices.push_back(i2);
            mesh.Indices.push_back(i1);

            mesh.Indices.push_back(i1);
            mesh.Indices.push_back(i2);
            mesh.Indices.push_back(i3);
        }
    }

    return mesh;
}
