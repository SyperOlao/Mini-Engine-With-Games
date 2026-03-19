#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "Game/Pong/PongGame.h"

#include <algorithm>
#include <cmath>
#include <string>
#include <string_view>

#include "Core/App/AppContext.h"
#include "Core/Common/Constants.h"
#include "Core/Graphics/Color.h"
#include "Core/Graphics2D/ShapeRenderer2D.h"
#include "Core/Input/InputSystem.h"
#include "Core/Math/MathHelpers.h"
#include "Core/UI/BitmapFont.h"
#include "Game/Pong/Systems/PongCollisionSystem.h"
#include "Game/Pong/Systems/PongRules.h"

namespace {
    constexpr Color kWhite{1.0f, 1.0f, 1.0f, 1.0f};
    constexpr Color kMuted{0.65f, 0.68f, 0.78f, 1.0f};
    constexpr Color kAccent{0.82f, 0.84f, 0.93f, 1.0f};

    constexpr float kMenuButtonWidth = 420.0f;
    constexpr float kMenuButtonHeight = 64.0f;
    constexpr float kMenuButtonGap = 18.0f;

    constexpr float kMenuTitleY = 60.0f;
    constexpr float kMenuSubtitleY = 145.0f;

    bool WasMenuUpPressed(const InputSystem &input) {
        return input.GetKeyboard().WasKeyPressed(Key::W)
               || input.GetKeyboard().WasKeyPressed(Key::Up);
    }

    bool WasMenuDownPressed(const InputSystem &input) {
        return input.GetKeyboard().WasKeyPressed(Key::S)
               || input.GetKeyboard().WasKeyPressed(Key::Down);
    }

    RectF BuildCenteredButtonRect(const int index) {
        const float totalHeight = 4.0f * kMenuButtonHeight + 3.0f * kMenuButtonGap;

        constexpr float reservedTop = 230.0f;
        constexpr float reservedBottom = 110.0f;

        const float availableHeight =
                static_cast<float>(Constants::WindowHeight) - reservedTop - reservedBottom;

        const float startY =
                reservedTop + std::max(0.0f, (availableHeight - totalHeight) * 0.5f);

        const float x =
                (static_cast<float>(Constants::WindowWidth) - kMenuButtonWidth) * 0.5f;

        const float y =
                startY + static_cast<float>(index) * (kMenuButtonHeight + kMenuButtonGap);

        return RectF{x, y, kMenuButtonWidth, kMenuButtonHeight};
    }

    void DrawCenteredText(
        ShapeRenderer2D &renderer,
        std::string_view text,
        const float y,
        const Color &color,
        const float scale
    ) {
        const std::string value{text};
        const float width = BitmapFont::MeasureTextWidth(value, scale);
        const float x = (static_cast<float>(Constants::WindowWidth) - width) * 0.5f;
        BitmapFont::DrawString(renderer, x, y, value, color, scale);
    }

    void DrawRightAlignedText(
        ShapeRenderer2D &renderer,
        std::string_view text,
        const float rightX,
        const float y,
        const Color &color,
        const float scale
    ) {
        const std::string value{text};
        const float width = BitmapFont::MeasureTextWidth(value, scale);
        BitmapFont::DrawString(renderer, rightX - width, y, value, color, scale);
    }
}

void PongGame::Initialize(AppContext &context) {
    (void) context;

    m_mainMenuButtons[0] = Button{BuildCenteredButtonRect(0), "START VS PLAYER", true};
    m_mainMenuButtons[1] = Button{BuildCenteredButtonRect(1), "START VS BOT", true};
    m_mainMenuButtons[2] = Button{BuildCenteredButtonRect(2), "SETTINGS", true};
    m_mainMenuButtons[3] = Button{BuildCenteredButtonRect(3), "EXIT", true};

    m_settingsButtons[0] = Button{BuildCenteredButtonRect(0), "EASY", true};
    m_settingsButtons[1] = Button{BuildCenteredButtonRect(1), "MEDIUM", true};
    m_settingsButtons[2] = Button{BuildCenteredButtonRect(2), "HARD", true};
    m_settingsButtons[3] = Button{BuildCenteredButtonRect(3), "BACK", true};

    m_leftPaddle.SetDimensions(Constants::PaddleWidth, Constants::PaddleHeight);
    m_rightPaddle.SetDimensions(Constants::PaddleWidth, Constants::PaddleHeight);
    m_ball.SetSize(Constants::BallSize);

    m_leftPaddle.ColorTint = kWhite;
    m_rightPaddle.ColorTint = kWhite;
    m_ball.ColorTint = kWhite;

    m_scene.Initialize();
    ResetMatch();
}

void PongGame::Update(AppContext &context, const float deltaTime) {
    m_fpsAccumulator += deltaTime;
    ++m_fpsFrames;

    if (m_fpsAccumulator >= 0.25f) {
        m_displayFps = static_cast<int>(std::round(static_cast<float>(m_fpsFrames) / m_fpsAccumulator));
        m_fpsAccumulator = 0.0f;
        m_fpsFrames = 0;
    }

    switch (m_screenState) {
        case ScreenState::MainMenu:
            UpdateMainMenu(context);
            break;

        case ScreenState::Settings:
            UpdateSettingsMenu(context);
            break;

        case ScreenState::Playing:
            UpdateGameplay(context, deltaTime);
            break;

        default:
            break;
    }
}

void PongGame::Render(AppContext &context) {
    switch (m_screenState) {
        case ScreenState::MainMenu:
            RenderMainMenu(context);
            break;

        case ScreenState::Settings:
            RenderSettingsMenu(context);
            break;

        case ScreenState::Playing:
            RenderGameplay(context);
            break;

        default:
            break;
    }
}

void PongGame::ApplyDifficulty() {
    const auto &tuning = PongRules::GetDifficultyTuning(m_difficulty);

    m_leftPaddle.SetDimensions(Constants::PaddleWidth, tuning.PaddleHeight);
    m_rightPaddle.SetDimensions(Constants::PaddleWidth, tuning.PaddleHeight);

    const float centerY =
            (PongRules::GetPlayableTop() + PongRules::GetPlayableBottom()) * 0.5f;

    m_leftPaddle.Transform.SetPosition(
        Constants::PaddleMarginX,
        centerY - m_leftPaddle.Height() * 0.5f
    );

    m_rightPaddle.Transform.SetPosition(
        static_cast<float>(Constants::WindowWidth) - Constants::PaddleMarginX - m_rightPaddle.Width(),
        centerY - m_rightPaddle.Height() * 0.5f
    );

    ResetRound();
}

void PongGame::StartGame(const GameMode mode) {
    m_gameMode = mode;
    m_screenState = ScreenState::Playing;
    ResetMatch();
}

void PongGame::ResetRound() {
    const float centerX =
            static_cast<float>(Constants::WindowWidth) * 0.5f - m_ball.Size() * 0.5f;

    const float centerY =
            (PongRules::GetPlayableTop() + PongRules::GetPlayableBottom()) * 0.5f - m_ball.Size() * 0.5f;

    m_ball.Transform.SetPosition(centerX, centerY);
    m_ball.Movement.Stop();

    m_paddleAI.Reset();
    m_roundResetTimer = Constants::BallResetDelaySeconds;
}

void PongGame::ResetMatch() {
    m_leftScore = 0;
    m_rightScore = 0;
    ApplyDifficulty();
}

void PongGame::UpdateMainMenu(AppContext &context) {
    auto &input = *context.Input;

    if (WasMenuUpPressed(input)) {
        m_selectedMainMenuIndex = (m_selectedMainMenuIndex + 3) % 4;
    } else if (WasMenuDownPressed(input)) {
        m_selectedMainMenuIndex = (m_selectedMainMenuIndex + 1) % 4;
    }

    if (!m_mainMenuButtons[m_selectedMainMenuIndex].HandleKeyboard(input, true)) {
        return;
    }

    switch (m_selectedMainMenuIndex) {
        case 0:
            StartGame(GameMode::TwoPlayers);
            break;

        case 1:
            StartGame(GameMode::VersusAI);
            break;

        case 2:
            m_selectedSettingsIndex = static_cast<int>(m_difficulty);
            m_screenState = ScreenState::Settings;
            break;

        case 3:
            PostQuitMessage(0);
            break;

        default:
            break;
    }
}

void PongGame::UpdateSettingsMenu(AppContext &context) {
    auto &input = *context.Input;

    if (input.GetKeyboard().WasKeyPressed(Key::Escape)) {
        m_screenState = ScreenState::MainMenu;
        return;
    }

    if (WasMenuUpPressed(input)) {
        m_selectedSettingsIndex = (m_selectedSettingsIndex + 3) % 4;
    } else if (WasMenuDownPressed(input)) {
        m_selectedSettingsIndex = (m_selectedSettingsIndex + 1) % 4;
    }

    if (!m_settingsButtons[m_selectedSettingsIndex].HandleKeyboard(input, true)) {
        return;
    }

    switch (m_selectedSettingsIndex) {
        case 0:
            m_difficulty = Difficulty::Easy;
            ApplyDifficulty();
            break;

        case 1:
            m_difficulty = Difficulty::Medium;
            ApplyDifficulty();
            break;

        case 2:
            m_difficulty = Difficulty::Hard;
            ApplyDifficulty();
            break;

        case 3:
            m_screenState = ScreenState::MainMenu;
            break;

        default:
            break;
    }
}

void PongGame::UpdateGameplay(AppContext &context, const float deltaTime) {
    auto &input = *context.Input;

    if (input.GetKeyboard().WasKeyPressed(Key::Escape)) {
        m_screenState = ScreenState::MainMenu;
        return;
    }

    if (input.GetKeyboard().WasKeyPressed(Key::Enter)) {
        ResetMatch();
        return;
    }

    UpdatePlayerPaddle(context, m_leftPaddle, InputPlayer::Player1, deltaTime);

    if (m_gameMode == GameMode::TwoPlayers) {
        UpdatePlayerPaddle(context, m_rightPaddle, InputPlayer::Player2, deltaTime);
    } else {
        m_paddleAI.Update(
            m_rightPaddle,
            m_ball,
            PongRules::GetDifficultyTuning(m_difficulty),
            deltaTime
        );

        ClampPaddleToField(m_rightPaddle);
    }

    if (m_roundResetTimer > 0.0f) {
        m_roundResetTimer = std::max(0.0f, m_roundResetTimer - deltaTime);

        if (m_roundResetTimer == 0.0f) {
            LaunchBallRandomDirection();
        }

        return;
    }

    UpdateBall(deltaTime);

    PongCollisionSystem::HandlePaddleCollision(m_ball, m_leftPaddle, m_rightPaddle);

    const CourtSide outOfBounds = PongCollisionSystem::CheckScoring(m_ball);
    if (outOfBounds != CourtSide::None) {
        ScorePoint(outOfBounds);
    }
}

void PongGame::UpdatePlayerPaddle(
    const AppContext &context,
    Paddle &paddle,
    const InputPlayer player,
    const float deltaTime
) {
    const float axis = context.Input->GetVerticalAxis(player);
    paddle.Transform.Position.y += axis * Constants::PaddleSpeed * deltaTime;
    ClampPaddleToField(paddle);
}

void PongGame::UpdateBall(const float deltaTime) {
    m_ball.Movement.Update(m_ball.Transform, deltaTime);

    PongCollisionSystem::HandleWallCollision(
        m_ball,
        PongRules::GetPlayableTop(),
        PongRules::GetPlayableBottom()
    );
}

void PongGame::ClampPaddleToField(Paddle &paddle) noexcept {
    const float minY = PongRules::GetPlayableTop();
    const float maxY = PongRules::GetPlayableBottom() - paddle.Height();
    paddle.Transform.Position.y = MathHelpers::Clamp(paddle.Transform.Position.y, minY, maxY);
}

void PongGame::LaunchBallRandomDirection() {
    const float directionX = MathHelpers::RandomFloat(0.0f, 1.0f) < 0.5f ? -1.0f : 1.0f;
    const float directionY = MathHelpers::RandomFloat(-0.75f, 0.75f);

    DirectX::SimpleMath::Vector2 direction{directionX, directionY};
    direction = MathHelpers::SafeNormalize(direction, DirectX::SimpleMath::Vector2{directionX, 0.0f});

    m_ball.Movement.Velocity = direction * PongRules::GetDifficultyTuning(m_difficulty).BallStartSpeed;
}

void PongGame::ScorePoint(const CourtSide outSide) {
    if (outSide == CourtSide::Left) {
        ++m_rightScore;
    } else if (outSide == CourtSide::Right) {
        ++m_leftScore;
    }

    ResetRound();
}

void PongGame::RenderMainMenu(const AppContext &context) const {
    auto &renderer = *context.Shape2D;

    RenderButtons(context, m_mainMenuButtons, m_selectedMainMenuIndex, "PONG");

    DrawCenteredText(
        renderer,
        PongRules::BuildDifficultyText(m_difficulty),
        kMenuSubtitleY,
        kMuted,
        0.95f
    );

    BitmapFont::DrawString(
        renderer,
        26.0f,
        static_cast<float>(Constants::WindowHeight) - 44.0f,
        "W/S OR ARROWS  NAVIGATE",
        kMuted,
        0.62f
    );

    DrawRightAlignedText(
        renderer,
        "ENTER  SELECT",
        static_cast<float>(Constants::WindowWidth) - 26.0f,
        static_cast<float>(Constants::WindowHeight) - 44.0f,
        kMuted,
        0.62f
    );
}

void PongGame::RenderSettingsMenu(const AppContext &context) const {
    auto &renderer = *context.Shape2D;

    RenderButtons(context, m_settingsButtons, m_selectedSettingsIndex, "SETTINGS");

    DrawCenteredText(
        renderer,
        std::string{"CURRENT "} + PongRules::BuildDifficultyText(m_difficulty),
        kMenuSubtitleY,
        kAccent,
        0.84f
    );

    BitmapFont::DrawString(
        renderer,
        26.0f,
        static_cast<float>(Constants::WindowHeight) - 78.0f,
        "BALL SPEED",
        kMuted,
        0.55f
    );

    DrawCenteredText(
        renderer,
        "PADDLE SIZE",
        static_cast<float>(Constants::WindowHeight) - 78.0f,
        kMuted,
        0.55f
    );

    DrawRightAlignedText(
        renderer,
        "BOT ACCURACY",
        static_cast<float>(Constants::WindowWidth) - 26.0f,
        static_cast<float>(Constants::WindowHeight) - 78.0f,
        kMuted,
        0.55f
    );

    BitmapFont::DrawString(
        renderer,
        26.0f,
        static_cast<float>(Constants::WindowHeight) - 44.0f,
        "ESC  BACK",
        kMuted,
        0.62f
    );

    DrawRightAlignedText(
        renderer,
        "ENTER  APPLY",
        static_cast<float>(Constants::WindowWidth) - 26.0f,
        static_cast<float>(Constants::WindowHeight) - 44.0f,
        kMuted,
        0.62f
    );
}

void PongGame::RenderGameplay(const AppContext &context) const {
    m_scene.RenderGameplay(
      context,
      m_leftPaddle,
      m_rightPaddle,
      m_ball,
      m_leftScore,
      m_rightScore,
      m_displayFps,
      m_gameMode,
      m_difficulty
  );
}


void PongGame::RenderButtons(
    const AppContext &context,
    const std::array<Button, 4> &buttons,
    const int selectedIndex,
    const char *title
) {
    auto &renderer = *context.Shape2D;

    DrawCenteredText(renderer, title, kMenuTitleY, kWhite, 1.8f);

    ButtonStyle style{};
    style.TextScale = 0.85f;

    for (int index = 0; index < static_cast<int>(buttons.size()); ++index) {
        buttons[static_cast<std::size_t>(index)].Draw(
            renderer,
            *context.Font,
            style,
            index == selectedIndex
        );
    }
}
