//
// Created by SyperOlao on 19.03.2026.
//

#ifndef PINGPONG_VERTEX3D_H
#define PINGPONG_VERTEX3D_H

#include <SimpleMath.h>

struct Vertex3D final
{
    DirectX::SimpleMath::Vector3 Position;
    DirectX::SimpleMath::Color Color;

    Vertex3D() = default;

    Vertex3D(
        const DirectX::SimpleMath::Vector3& position,
        const DirectX::SimpleMath::Color& color
    )
        : Position(position), Color(color)
    {
    }
};

#endif //PINGPONG_VERTEX3D_H