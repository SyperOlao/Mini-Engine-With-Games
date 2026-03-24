//
// Created by SyperOlao on 19.03.2026.
//

#ifndef PINGPONG_PROJECTIONMODE_H
#define PINGPONG_PROJECTIONMODE_H
#include <cstdint>

enum class ProjectionMode : std::uint8_t {
    PerspectiveFov = 0,
    PerspectiveOffCenter
};
#endif //PINGPONG_PROJECTIONMODE_H
