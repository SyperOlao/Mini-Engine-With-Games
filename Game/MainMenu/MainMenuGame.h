#ifndef PINGPONG_MAINMENUGAME_H
#define PINGPONG_MAINMENUGAME_H

#include "Core/App/IGame.h"
#include "Core/UI/Button.h"

#include <array>

class MainMenuGame final : public IGame {
public:
    void Initialize(AppContext &context) override;

    void Update(AppContext &context, float deltaTime) override;

    void Render(AppContext &context) override;

    void Shutdown(AppContext &context) override;

    [[nodiscard]] bool TryGetPreferredClearColor(Color &clearColor) const noexcept override;

private:
    void SyncButtonLayout(const AppContext &context);

    void TryActivateSelection(AppContext &context, int itemIndex);

    std::array<Button, 5> m_menuButtons{};
    int m_selectedMenuIndex{0};
    int m_activationSuppressFrames{0};
    bool m_previousLeftMouseDown{false};
};

#endif
