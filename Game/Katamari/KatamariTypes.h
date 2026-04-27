#ifndef PINGPONG_KATAMARITYPES_H
#define PINGPONG_KATAMARITYPES_H

#include <cstdint>

namespace KatamariTagId
{
inline constexpr std::uint32_t PlayerBall = 1u;
inline constexpr std::uint32_t Pickup = 2u;
inline constexpr std::uint32_t StaticObstacle = 3u;
}

namespace KatamariCollisionLayer
{
inline constexpr std::uint32_t WorldStatic = 0u;
inline constexpr std::uint32_t PlayerBall = 1u;
inline constexpr std::uint32_t Pickup = 2u;
}

enum class KatamariObstacleShape : std::uint8_t
{
    Cube,
    TriangularPrism
};

#endif
