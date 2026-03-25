#include "Core/App/Application.h"
#include "Core/App/FatalErrorReport.h"

#include <Windows.h>
#include <exception>
#include "Game/Pong/PongGame.h"
#include "Game/SolarSystem/SolarSystemGame.h"

enum class DemoType
{
    Pong,
    SolarSystem
};

int main()
{
    try
    {
        HINSTANCE__ *const hInstance = GetModuleHandleW(nullptr);

        constexpr auto demo = DemoType::SolarSystem;

        std::unique_ptr<IGame> game;

        switch (demo)
        {
            case DemoType::Pong:
                game = std::make_unique<PongGame>();
                break;

            case DemoType::SolarSystem:
                game = std::make_unique<SolarSystemGame>();
                break;
        }

        Application app(hInstance, std::move(game));
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