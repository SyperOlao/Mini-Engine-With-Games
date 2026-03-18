//
// Created by SyperOlao on 18.03.2026.
//

#ifndef MYPROJECT_PADDLEAI_H
#define MYPROJECT_PADDLEAI_H

#include "Game/Pong/PongTypes.h"

class Ball;
class Paddle;

class PaddleAI final
{
public:
    void Reset() noexcept;
    void Update(Paddle& paddle, const Ball& ball, const DifficultyTuning& tuning, float deltaTime);

private:
    AIState m_state{};
};


#endif //MYPROJECT_PADDLEAI_H