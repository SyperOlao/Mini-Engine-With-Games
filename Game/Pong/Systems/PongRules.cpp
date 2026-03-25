//
// Created by SyperOlao on 18.03.2026.
//
#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "PongRules.h"

#include <sstream>
#include <array>
#include <string>
#include "Game/Pong/Common/Constants.h"
#include "Game/Pong/PongTypes.h"


const DifficultyTuning &PongRules::GetDifficultyTuning(const Difficulty difficulty) noexcept {
    static constexpr std::array difficultyTable
    {
        DifficultyTuning{
            "EASY",
            380.0f,
            160.0f,
            340.0f,
            1.30f,
            0.30f,
            0.75f,
            90.0f,
            0.48f
        },
        DifficultyTuning{
            "MEDIUM",
            500.0f,
            120.0f,
            500.0f,
            0.75f,
            0.20f,
            0.55f,
            48.0f,
            0.68f
        },
        DifficultyTuning{
            "HARD",
            820.0f,
            92.0f,
            780.0f,
            0.18f,
            0.10f,
            0.24f,
            18.0f,
            0.76f
        }
    };

    return difficultyTable[static_cast<std::size_t>(difficulty)];
}

float PongRules::GetPlayableTop() noexcept {
    return FieldMarginTop;
}

float PongRules::GetPlayableBottom() noexcept {
    return static_cast<float>(Constants::WindowHeight) - FieldMarginBottom;
}

std::string PongRules::BuildScoreText(const ScoreType leftScore, const ScoreType rightScore) {
    std::ostringstream stream;

    if (leftScore < 10) {
        stream << '0';
    }

    stream << leftScore << " : ";

    if (rightScore < 10) {
        stream << '0';
    }

    stream << rightScore;

    return stream.str();
}

std::string PongRules::BuildModeText(const GameMode mode) {
    return mode == GameMode::TwoPlayers ? "MODE PVP" : "MODE BOT";
}

std::string PongRules::BuildDifficultyText(const Difficulty difficulty) {
    return std::string{"LEVEL "} + GetDifficultyTuning(difficulty).Label;
}
