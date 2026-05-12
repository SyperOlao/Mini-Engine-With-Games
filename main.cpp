#include "Core/App/Application.h"
#include "Core/App/FatalErrorReport.h"
#include "Game/MainMenu/MainMenuGame.h"

#include <Windows.h>
#include <exception>
#include <memory>

int main()
{
    try
    {
        HINSTANCE__ *const hInstance = GetModuleHandleW(nullptr);

        ApplicationDesc applicationDescription{};
        applicationDescription.Title = L"Mini Engine";

        Application app(hInstance, std::make_unique<MainMenuGame>(), applicationDescription);
        return app.Run();
    }
    catch (const std::exception &exception)
    {
        const std::string message = std::string("Fatal error:\n") + exception.what();
        FatalErrorReport::Report(message);
        return -1;
    }
    catch (...)
    {
        FatalErrorReport::Report("Unknown fatal error (non-std::exception).");
        return -1;
    }
}
