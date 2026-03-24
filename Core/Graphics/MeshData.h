//
// Created by SyperOlao on 19.03.2026.
//

#ifndef PINGPONG_MESHDATA_H
#define PINGPONG_MESHDATA_H

#include <vector>
#include <SimpleMath.h>

struct Vertex3D final {
    DirectX::SimpleMath::Vector3 Position;
    DirectX::SimpleMath::Color Color;
};

struct MeshData final {
    std::vector<Vertex3D> Vertices;
    std::vector<std::uint32_t> Indices;
};

#endif //PINGPONG_MESHDATA_H
