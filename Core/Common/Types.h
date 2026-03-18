//
// Created by SyperOlao on 17.03.2026.
//

#ifndef PINGPONG_TYPES_H
#define PINGPONG_TYPES_H

#include <cstddef>
#include <cstdint>

using uint = unsigned int;
using byte = std::uint8_t;
using ScoreType = std::int32_t;
using PlayerIndex = std::size_t;

enum class CourtSide : std::uint8_t
{
    None = 0,
    Left,
    Right
};

enum class GameMode : std::uint8_t
{
    TwoPlayers = 0,
    VersusAI = 1
};

#endif //PINGPONG_TYPES_H