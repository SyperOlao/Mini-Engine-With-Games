//
// Created by SyperOlao on 18.03.2026.
//

#ifndef MYPROJECT_PONGRULES_H
#define MYPROJECT_PONGRULES_H
#include <string>

#include "Core/Common/Types.h"
#include "Game/Pong/PongTypes.h"


class PongRules final
{
public:
    static constexpr float FieldMarginTop = 80.0f;
    static constexpr float FieldMarginBottom = 40.0f;

    [[nodiscard]] static const DifficultyTuning& GetDifficultyTuning(Difficulty difficulty) noexcept;
    [[nodiscard]] static float GetPlayableTop() noexcept;
    [[nodiscard]] static float GetPlayableBottom() noexcept;

    [[nodiscard]] static std::string BuildScoreText(ScoreType leftScore, ScoreType rightScore);
    [[nodiscard]] static std::string BuildModeText(GameMode mode);
    [[nodiscard]] static std::string BuildDifficultyText(Difficulty difficulty);
};

#endif //MYPROJECT_PONGRULES_H