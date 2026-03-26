//
// Created by SyperOlao on 19.03.2026.
//

#ifndef PINGPONG_PONGUI_H
#define PINGPONG_PONGUI_H

#include <array>
#include <cstdint>
#include <string>

#include "Game/Pong/Common/Types.h"
#include "Core/UI/Button.h"
#include "Core/UI/Switcher.h"
#include "Game/Pong/PongTypes.h"

struct AppContext;
class PongScene;
class Paddle;
class Ball;

class PongUI final {
public:
    enum class ScreenState : std::uint8_t {
        MainMenu = 0,
        Settings,
        Playing,
        GameOver
    };

    enum class Action : std::uint8_t {
        None = 0,
        StartVsPlayer,
        StartVsBot,
        ExitRequested,
        DifficultyChanged,
        MatchRuleChanged,
        ToggleRenderMode,
        ResetMatch,
        RestartGame
    };

public:
    void Initialize();

    [[nodiscard]] Action Update(AppContext &context);

    void Render(
        const AppContext &context,
        const PongScene &scene,
        const Paddle &leftPaddle,
        const Paddle &rightPaddle,
        const Ball &ball,
        ScoreType leftScore,
        ScoreType rightScore,
        GameMode gameMode
    ) const;

    void SetScreen(ScreenState screen) noexcept;
    [[nodiscard]] ScreenState GetScreen() const noexcept;

    void SyncSettings(Difficulty difficulty, MatchRule matchRule);
    [[nodiscard]] Difficulty SelectedDifficulty() const noexcept;
    [[nodiscard]] MatchRule SelectedMatchRule() const noexcept;

    void SetGameOverText(const std::string &text);
    void ClearGameOverText();
    void SetDisplayedFps(int fps) noexcept;

private:
    [[nodiscard]] Action UpdateMainMenu(AppContext &context);
    [[nodiscard]] Action UpdateSettingsMenu(AppContext &context);
    [[nodiscard]] Action UpdatePlaying(AppContext &context);
    [[nodiscard]] Action UpdateGameOver(AppContext &context);

    void RenderMainMenu(const AppContext &context) const;
    void RenderSettingsMenu(const AppContext &context) const;

    void RenderPlaying(
        const AppContext &context,
        const PongScene &scene,
        const Paddle &leftPaddle,
        const Paddle &rightPaddle,
        const Ball &ball,
        ScoreType leftScore,
        ScoreType rightScore,
        GameMode gameMode
    ) const;

    void RenderGameOver(
        const AppContext &context,
        const PongScene &scene,
        const Paddle &leftPaddle,
        const Paddle &rightPaddle,
        const Ball &ball,
        ScoreType leftScore,
        ScoreType rightScore,
        GameMode gameMode
    ) const;

    static void RenderButtons(
        const AppContext &context,
        const std::array<Button, 4> &buttons,
        int selectedIndex,
        const char *title
    );

private:
    ScreenState m_screenState{ScreenState::MainMenu};
    Difficulty m_difficulty{Difficulty::Medium};
    MatchRule m_matchRule{MatchRule::FirstTo10};

    std::array<Button, 4> m_mainMenuButtons{};
    Switcher m_difficultySwitcher{};
    Switcher m_matchRuleSwitcher{};
    Button m_renderModeButton{};
    Button m_settingsBackButton{};

    int m_selectedMainMenuIndex{0};
    int m_selectedSettingsIndex{0};
    int m_displayFps{0};

    std::string m_resultText{};
};



#endif //PINGPONG_PONGUI_H