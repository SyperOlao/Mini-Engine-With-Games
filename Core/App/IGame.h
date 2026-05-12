//
// Created by SyperOlao on 18.03.2026.
//

#ifndef PINGPONG_IGAME_H
#define PINGPONG_IGAME_H

#include <cstdint>

#include "../Graphics/Color.h"

struct AppContext;
enum class RenderMode : std::uint8_t;

class IGame {
public:
    virtual ~IGame() = default;

    virtual void Initialize(AppContext &context) = 0;

    virtual void Update(AppContext &context, float deltaTime) = 0;

    virtual void Render(AppContext &context) = 0;

    virtual void OnRenderModeChanged(AppContext &, RenderMode) {
    }

    virtual void Shutdown(AppContext &context) {
    }

    [[nodiscard]] virtual bool TryGetPreferredClearColor(Color &clearColor) const noexcept {
        return false;
    }

    [[nodiscard]] virtual bool WantsGlobalRenderModeToggleOverlay() const noexcept {
        return false;
    }
};

#endif //PINGPONG_IGAME_H
