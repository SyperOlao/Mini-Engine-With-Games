//
// Created by SyperOlao on 17.03.2026.
//

#ifndef PINGPONG_PONGSCENE_H
#define PINGPONG_PONGSCENE_H

#include "Core/Common/Types.h"
#include "Game/Pong/Entities/PlayField.h"
#include "Game/Pong/Entities/ScoreBoard.h"
#include "Game/Pong/PongTypes.h"

struct AppContext;
class Paddle;
class Ball;


class PongScene final {
public:
    void Initialize() noexcept;

    void RenderGameplay(
        const AppContext &context,
        const Paddle &leftPaddle,
        const Paddle &rightPaddle,
        const Ball &ball,
        ScoreType leftScore,
        ScoreType rightScore,
        int fps,
        GameMode gameMode,
        Difficulty difficulty
    ) const;

private:
    PlayField m_playField{};
    ScoreBoard m_scoreBoard{};
};
#endif //PINGPONG_PONGSCENE_H
