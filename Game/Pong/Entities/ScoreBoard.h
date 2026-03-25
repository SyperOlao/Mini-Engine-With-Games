//
// Created by SyperOlao on 17.03.2026.
//

#ifndef PINGPONG_SCOREBOARD_H
#define PINGPONG_SCOREBOARD_H

#include "Game/Pong/Common/Types.h"
#include "Game/Pong/PongTypes.h"

class ShapeRenderer2D;

class ScoreBoard final
{
public:
    static void Render(
        const ShapeRenderer2D& renderer,
        ScoreType leftScore,
        ScoreType rightScore,
        int fps,
        GameMode gameMode,
        Difficulty difficulty
    );
};

#endif //PINGPONG_SCOREBOARD_H