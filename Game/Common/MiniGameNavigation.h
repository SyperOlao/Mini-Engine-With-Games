#pragma once

#include "Core/Input/InputSystem.h"

struct AppContext;

[[nodiscard]] bool WasDebugReturnToMainMenuPressed(const InputSystem &input);

bool RequestReturnToEngineMainMenu(AppContext &context, const char *source);
