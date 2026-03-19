//
// Created by SyperOlao on 17.03.2026.
//

#ifndef PINGPONG_PLAYFIELD_H
#define PINGPONG_PLAYFIELD_H



#include "Game/Pong/Entities/CenterLine.h"
#include "Game/Pong/Entities/Wall.h"

class ShapeRenderer2D;
class Paddle;
class Ball;

class PlayField final
{
public:
    void Initialize() noexcept;
    void UpdateLayout() noexcept;

    void Render(
        ShapeRenderer2D& renderer,
        const Paddle& leftPaddle,
        const Paddle& rightPaddle,
        const Ball& ball
    ) const;

private:
    Wall m_topWall{};
    Wall m_bottomWall{};
    CenterLine m_centerLine{};
};

#endif //PINGPONG_PLAYFIELD_H