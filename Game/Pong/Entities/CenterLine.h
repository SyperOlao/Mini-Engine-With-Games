//
// Created by SyperOlao on 17.03.2026.
//

#ifndef PINGPONG_CENTERLINE_H
#define PINGPONG_CENTERLINE_H

class ShapeRenderer2D;

class CenterLine final
{
public:
    void Configure(float x, float topY, float playableHeight) noexcept;
    void Render(const ShapeRenderer2D& renderer) const;

private:
    float m_x{0.0f};
    float m_topY{0.0f};
    float m_playableHeight{0.0f};
};

#endif //PINGPONG_CENTERLINE_H