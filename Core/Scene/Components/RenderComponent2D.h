//
// Created by SyperOlao on 17.03.2026.
//

#ifndef PINGPONG_RENDERCOMPONENT2D_H
#define PINGPONG_RENDERCOMPONENT2D_H
#include <SimpleMath.h>

#include "../../Render/Color.h"
#include "../../Render/Renderer.h"
#include "../Transform2D.h"

struct RenderComponent2D final
{
    DirectX::SimpleMath::Vector2 Size{10.0f, 10.0f};
    Color Tint{1.0f, 1.0f, 1.0f, 1.0f};
    int Layer{0};
    bool Visible{true};

    void Render(const Transform2D& transform, const Renderer& renderer) const
    {
        if (!Visible)
        {
            return;
        }

        const auto scaledSize = DirectX::SimpleMath::Vector2
        {
            Size.x * transform.Scale.x,
            Size.y * transform.Scale.y
        };

        renderer.DrawRectangle(
            transform.Position.x,
            transform.Position.y,
            scaledSize.x,
            scaledSize.y,
            Tint
        );
    }
};

#endif //PINGPONG_RENDERCOMPONENT2D_H