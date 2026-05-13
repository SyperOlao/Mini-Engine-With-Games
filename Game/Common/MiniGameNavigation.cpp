#include "Game/Common/MiniGameNavigation.h"

#include "Core/App/AppContext.h"
#include "Core/App/IGameHost.h"
#include "Game/MainMenu/MainMenuGame.h"

#include <cstdio>
#include <memory>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

bool WasDebugReturnToMainMenuPressed(const InputSystem &input)
{
    return input.GetKeyboard().WasKeyPressed(Key::P);
}

bool RequestReturnToEngineMainMenu(AppContext &context, const char *const source)
{
    if (context.GameHost == nullptr)
    {
        char buffer[320]{};
        (void)std::snprintf(
            buffer,
            sizeof(buffer),
            "[NAV][RETURN_MAIN] ERROR GameHost=null source=%s\n",
            source != nullptr ? source : "(null)");
        OutputDebugStringA(buffer);
        return false;
    }

    {
        char buffer[320]{};
        (void)std::snprintf(
            buffer,
            sizeof(buffer),
            "[NAV][RETURN_MAIN] source=%s\n",
            source != nullptr ? source : "(null)");
        OutputDebugStringA(buffer);
    }

    context.GameHost->RequestSwitchGame(std::make_unique<MainMenuGame>());

    if (context.Input.System != nullptr)
    {
        context.Input.System->ConsumePressedStates();
    }
    return true;
}
