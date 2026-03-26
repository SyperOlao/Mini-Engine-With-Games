//
// Created by SyperOlao on 17.03.2026.
//

#ifndef PINGPONG_GAME_H
#define PINGPONG_GAME_H

#include "Core/App/IGame.h"
#include "Game/Pong/Common/Types.h"
#include "Core/Input/InputSystem.h"
#include "Game/Pong/Entities/Ball.h"
#include "Game/Pong/Entities/Paddle.h"
#include "Game/Pong/PongScene.h"
#include "Game/Pong/PongTypes.h"
#include "Game/Pong/Systems/PaddleAI.h"
#include "UI/PongUI.h"

struct AppContext;

class PongGame final : public IGame {
public:
    void Initialize(AppContext &context) override;

    void Update(AppContext &context, float deltaTime) override;

    void Render(AppContext &context) override;

private:
    void HandleUiAction(AppContext &context, PongUI::Action action);

    void ApplyDifficulty();

    void StartGame(GameMode mode);

    void ResetRound();

    void ResetMatch();

    void UpdateGameplay(const AppContext &context, float deltaTime);

    static void UpdatePlayerPaddle(const AppContext &context, Paddle &paddle, InputPlayer player, float deltaTime);

    void UpdateBall(float deltaTime, const AppContext &context);

    static void ClampPaddleToField(Paddle &paddle) noexcept;

    void LaunchBallRandomDirection();

    void ScorePoint(CourtSide outSide);

    void FinishMatch();

    [[nodiscard]] bool IsEndlessModeActive() const noexcept;

private:
    GameMode m_gameMode{GameMode::TwoPlayers};
    Difficulty m_difficulty{Difficulty::Medium};
    MatchRule m_matchRule{MatchRule::FirstTo10};

    Paddle m_leftPaddle{};
    Paddle m_rightPaddle{};
    Ball m_ball{};

    PongScene m_scene{};
    PaddleAI m_paddleAI{};
    PongUI m_ui{};

    ScoreType m_leftScore{0};
    ScoreType m_rightScore{0};
    float m_roundResetTimer{0.0f};

    float m_fpsAccumulator{0.0f};
    int m_fpsFrames{0};
    int m_displayFps{0};
};

#endif //PINGPONG_GAME_H
