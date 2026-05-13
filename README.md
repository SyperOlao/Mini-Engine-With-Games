# Mini Engine With Games

<p align="center">
  <a href="https://github.com/SyperOlao/Mini-Engine-With-Games/actions/workflows/windows-build.yml">
    <img src="https://github.com/SyperOlao/Mini-Engine-With-Games/actions/workflows/windows-build.yml/badge.svg" alt="Windows Build">
  </a>
  <img src="https://img.shields.io/badge/C%2B%2B-20-00599C?logo=cplusplus&logoColor=white" alt="C++20">
  <img src="https://img.shields.io/badge/DirectX-11-0078D4?logo=windows&logoColor=white" alt="DirectX 11">
  <img src="https://img.shields.io/badge/CMake-3.21%2B-064F8C?logo=cmake&logoColor=white" alt="CMake">
  <img src="https://img.shields.io/badge/dependencies-vcpkg-2C5BB4" alt="vcpkg">
  <img src="https://img.shields.io/badge/platform-Windows%2010%2F11-0078D4?logo=windows&logoColor=white" alt="Windows">
  <a href="https://github.com/SyperOlao/Mini-Engine-With-Games/releases">
    <img src="https://img.shields.io/github/v/release/SyperOlao/Mini-Engine-With-Games?include_prereleases&label=release" alt="Latest Release">
  </a>
  <a href="https://github.com/SyperOlao/Mini-Engine-With-Games/stargazers">
    <img src="https://img.shields.io/github/stars/SyperOlao/Mini-Engine-With-Games?style=social" alt="GitHub Stars">
  </a>
</p>

A compact **educational C++20 / DirectX 11 mini-engine**: one shared `Core` runtime, several playable demos, and a small **ECS-style** gameplay layer you can read end-to-end without commercial-engine baggage.

This is a **learning and experimentation sandbox**, not a production game engine.

**Russian README / русская версия:** [README_RU.md](README_RU.md) (mirror of this document; keep both in sync).

**Quick links**

- [Download latest Windows release](https://github.com/SyperOlao/Mini-Engine-With-Games/releases/latest)
- [Quick start (build & run)](#quick-start)
- [Architecture overview](#architecture-overview)
- [How to add a new mini-game](#how-to-add-a-new-mini-game)
- [Controls](#controls)

## Table of contents

- [Preview](#preview)
- [Why this project exists](#why-this-project-exists)
- [What you can learn from this repo](#what-you-can-learn-from-this-repo)
- [Quick start](#quick-start)
- [Games and demos](#games-and-demos)
- [Architecture overview](#architecture-overview)
- [Rendering features](#rendering-features)
- [Controls](#controls)
- [Build requirements](#build-requirements)
- [Troubleshooting](#troubleshooting)
- [How to add a new mini-game](#how-to-add-a-new-mini-game)
- [Project structure](#project-structure)
- [Known limitations](#known-limitations)
- [Roadmap](#roadmap)
- [Contributing](#contributing)
- [Media files and Git](#media-files-and-git)

## Preview

![Overview demo](Images/demo2.gif)

## Why this project exists

Students and **beginner / intermediate** graphics and gameplay programmers often need a **small, buildable DirectX 11 codebase** to study:

- how a **window, input, timing, and render loop** fit together in modern C++;
- how to keep **engine-ish Core** code separate from **Game** modules;
- how **forward vs deferred** rendering and simple **shadow / GBuffer** tooling look in practice;
- how a **lightweight ECS-style** scene can drive a mini-game without a full editor stack.

This repository stays intentionally smaller than a commercial engine: you can trace a feature from `main.cpp` through `Application`, `IGame`, rendering passes, and a concrete demo.

## What you can learn from this repo

- **C++20** usage in a real (small) executable: language features and the STL where they simplify the code, plus clear ownership at game boundaries.
- **Game loop and lifecycle**: window messages, `Application` update/render, delta time.
- **`IGame` module switching**: each demo is a class implementing one interface; the shell can swap games at runtime (`IGameHost`, `RequestSwitchGame`).
- **Core / Game separation**: shared services in `AppContext` (see `Core/App/AppContext.h`, `Core/App/EngineServices.h`); gameplay and content live under `Game/*`.
- **DirectX 11 basics**: device/swap chain, shaders compiled at runtime from copied `Core/Shaders`, simple 2D and 3D draw paths.
- **Forward and deferred** rendering paths orchestrated by `FrameRenderer` and passes under `Core/Graphics/Rendering/Pipeline/Passes`.
- **GBuffer** layout, deferred lighting/composite passes, and **GBuffer debug visualization** (demo hotkeys).
- **Cascaded shadow mapping** for directional lights and **cascade debug visualization**.
- **ECS-style gameplay**: `Scene`, entities, components, systems under `Core/Gameplay` (plus Katamari-specific systems).
- **Collision helpers** in `Core/Physics` and `Core/Gameplay` (2D and 3D utilities, debug draw hooks).
- **UI**: bitmap font, buttons, switchers, sliders, simple layout patterns in `Core/UI`.
- **Audio** via **DirectXTK** (`Core/Audio`).
- **Debug / tools**: **ImGui** layer, **ImGuizmo**-based transform gizmo, and **GBuffer picking** in Katamari (experimental / learning-oriented, not a full editor).

## Quick start

### A) Download prebuilt Windows binary

1. Open the [latest release](https://github.com/SyperOlao/Mini-Engine-With-Games/releases/latest).
2. Download the Windows **x64** archive.
3. Extract and run **`MiniEngineDemo.exe`**.

The app starts in the **engine main menu** (`MainMenuGame`). Choose a demo, or **EXIT** to quit.

### B) Build from source (Windows 10/11, Visual Studio 2022, CMake 3.21+, vcpkg, x64-windows)

**1. vcpkg (first time)**

```powershell
git clone https://github.com/microsoft/vcpkg.git C:\dev\vcpkg
cd C:\dev\vcpkg
.\bootstrap-vcpkg.bat
vcpkg install directxtk:x64-windows assimp:x64-windows
```

For the current shell session (example paths):

```powershell
$env:VCPKG_ROOT = "C:\dev\vcpkg"
$env:Path = "$env:VCPKG_ROOT;$env:Path"
```

**2. Configure and build (Visual Studio generator)**

From the repository root (adjust `C:/dev/vcpkg` if your vcpkg root differs):

```powershell
git clone https://github.com/SyperOlao/Mini-Engine-With-Games.git
cd Mini-Engine-With-Games

cmake -S . -B cmake-build-debug `
  -G "Visual Studio 17 2022" `
  -A x64 `
  -DCMAKE_TOOLCHAIN_FILE=C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake `
  -DVCPKG_TARGET_TRIPLET=x64-windows

cmake --build cmake-build-debug --config Debug
```

**3. Run**

With the **Visual Studio** multi-config generator, the Debug binary is typically:

```powershell
.\cmake-build-debug\Debug\MiniEngineDemo.exe
```

If you use a **single-config** generator (for example **Ninja**), the executable is usually next to the build tree root, for example `.\build\MiniEngineDemo.exe`.

### Optional: CMake preset (Ninja)

[`CMakePresets.json`](CMakePresets.json) defines a **Ninja** preset that expects:

- `VCPKG_ROOT` pointing at your vcpkg clone
- **Ninja** on your `PATH`

Example:

```powershell
$env:VCPKG_ROOT = "C:\dev\vcpkg"
cmake --preset default
cmake --build build
.\build\MiniEngineDemo.exe
```

`CMakePresets.json` uses `VCPKG_TARGET_TRIPLET` **x64-windows** to match the vcpkg install above.

## Games and demos

Runtime flow: **`MiniEngineDemo.exe`** always boots **`MainMenuGame`** from [`main.cpp`](main.cpp). Pick a demo from the menu. To quit cleanly, use **EXIT** on that menu (see [Controls](#controls) for `Esc` behavior).

### Main menu (`Game/MainMenu`)

**Demonstrates:** simple UI navigation, audio feedback, switching active `IGame` through `IGameHost::RequestSwitchGame`, and returning later via `Game/Common/MiniGameNavigation`.

**Read first:** [`Game/MainMenu/MainMenuGame.cpp`](Game/MainMenu/MainMenuGame.cpp), [`Game/Common/MiniGameNavigation.cpp`](Game/Common/MiniGameNavigation.cpp), [`Core/App/Application.cpp`](Core/App/Application.cpp) (`RequestSwitchGame`, `RequestQuitApplication`).

### Pong (`Game/Pong`)

**Demonstrates:** 2D gameplay, local UI flow (menus, settings, match), collision, and shared audio/UI patterns.

**Read first:** [`Game/Pong/PongGame.cpp`](Game/Pong/PongGame.cpp), [`Game/Pong/UI/PongUI.cpp`](Game/Pong/UI/PongUI.cpp).

#### Gameplay

![Pong gameplay](Images/pong_game_play.gif)

### Solar System (`Game/SolarSystem`)

**Demonstrates:** 3D scene update, FPS vs orbit cameras, tuning UI, and movement-driven audio.

**Read first:** [`Game/SolarSystem/SolarSystemGame.cpp`](Game/SolarSystem/SolarSystemGame.cpp), [`Game/SolarSystem/SolarSystemScene.cpp`](Game/SolarSystem/SolarSystemScene.cpp).

Still frame (large GIFs are intentionally avoided in-repo; compress locally or attach clips to Releases if needed):

![Solar system still](Images/img5.png)

### Katamari (`Game/Katamari`)

**Demonstrates:** rolling-ball gameplay, follow camera, deferred integration, GPU particles, collision debug, shadow cascade debug, GBuffer debug, GBuffer picking, and an **ImGuizmo**-based transform tool (learning-oriented).

**Read first:** [`Game/Katamari/KatamariGame.cpp`](Game/Katamari/KatamariGame.cpp), systems under [`Game/Katamari/Systems`](Game/Katamari/Systems), [`Core/Graphics/Rendering/FrameRenderer.cpp`](Core/Graphics/Rendering/FrameRenderer.cpp).

![Katamari gameplay](Images/img12.png)

![Katamari particles](Images/img13.png)

![Katamari shadow cascades debug](Images/img14.png)

![Katamari GBuffer debug](Images/img9.png)

### Lighting test (`Game/LightingTest`)

**Demonstrates:** forward-lit primitives and imported models in a small scene for lighting and camera sanity checks.

**Read first:** [`Game/LightingTest/LightingTestGame.cpp`](Game/LightingTest/LightingTestGame.cpp).

![Lighting test scene](Images/img15.png)

## Architecture overview

High level: **`Application`** owns the window, DirectX device, input, audio, asset cache, and render context. It holds the active **`IGame`**, calls `Initialize` / `Update` / `Render` / `Shutdown`, and applies pending game switches from **`IGameHost`**.

- **`Core/App`** — `Application`, `IGame`, `IGameHost`, `AppContext`, timing, fatal error reporting, default window size (`ApplicationDefaults.h`).
- **`Core/Graphics`** — `GraphicsDevice`, 2D/3D render helpers, cameras, model rendering, deferred resources, **`FrameRenderer`** and **`Core/Graphics/Rendering/Pipeline/Passes/*`** (geometry, deferred, shadows, particles, overlays, UI).
- **`Core/Gameplay`** — `Scene`, entities, components, built-in systems (transform, velocity, collision, render sync).
- **`Core/Input`** — keyboard state and raw mouse deltas (`RawInputHandler`).
- **`Core/Audio`** — DirectXTK-based loading, one-shots, loops.
- **`Core/UI`** — bitmap font and lightweight widgets; ImGui integration under `Core/UI/ImGui`.
- **`Core/Physics`** — shared collision types, queries, helpers used by gameplay and demos.
- **`Core/Editor`** — small helpers such as **`TransformGizmoService`** (ImGuizmo), used from Katamari.
- **`Game/*`** — self-contained demos implementing `IGame`; shared navigation helpers live in **`Game/Common`**.

### Start reading here

1. [`main.cpp`](main.cpp) — entry point; constructs `Application` with `MainMenuGame`.
2. [`Core/App/Application.cpp`](Core/App/Application.cpp) — frame loop, resize, game switching, quit routing.
3. [`Core/App/IGame.h`](Core/App/IGame.h) — contract every demo implements.
4. [`Game/MainMenu/MainMenuGame.cpp`](Game/MainMenu/MainMenuGame.cpp) — runtime demo launcher UI.
5. [`Game/Pong/PongGame.cpp`](Game/Pong/PongGame.cpp) — smallest full game loop example.
6. [`Core/Graphics/Rendering/FrameRenderer.cpp`](Core/Graphics/Rendering/FrameRenderer.cpp) — how passes are wired per frame.
7. [`Core/Graphics/Rendering/Pipeline/Passes`](Core/Graphics/Rendering/Pipeline/Passes) — individual render passes (forward/deferred branches, debug, UI).
8. [`Game/Katamari`](Game/Katamari) — ECS-style systems plus rendering/debug integration.

### Developer option: skip the main menu

For quick iteration you can still construct another `IGame` directly in [`main.cpp`](main.cpp) (for example `std::make_unique<KatamariGame>()` instead of `MainMenuGame`). The supported student path is the **runtime menu**; editing `main.cpp` is optional.

## Rendering features

Present in code today (read passes under `Core/Graphics/Rendering/Pipeline/Passes` for details):

- **Forward** Phong-style path for simpler scenes.
- **Deferred** geometry + lighting + composite passes; **GBuffer** targets and layouts under `Core/Graphics/Rendering/Deferred`.
- **GBuffer debug** visualization (toggle in Katamari; see [Controls](#controls)).
- **Directional cascaded shadow maps** and **cascade debug** visualization.
- **GPU particles** (compute/update + draw integration; tuned from Katamari UI).
- **GBuffer picking** (`Core/Graphics/Picking/GBufferPickingService`) — used for entity hit inspection and gizmo selection in Katamari when the picking inspector is enabled (**deferred** mode).
- **ImGuizmo** transform manipulation (`Core/Editor/TransformGizmoService`) — experimental learning tool, not a shipped editor.

Some combinations (for example picking + forward mode) are intentionally limited; treat advanced tooling as **experimental** where the HUD or code paths indicate so.

## Controls

### Engine main menu (`MainMenuGame`)

- **W / S** or **Up / Down** — move selection
- **Enter** or **mouse click** — activate item (**Pong**, **Solar System**, **Katamari**, **Lighting test**, **EXIT**)
- **`Esc`** — **ignored** on this menu (nowhere to go “back”); use **EXIT** to quit or choose a demo
- **`P`** — intentionally ignored here so it does not leak when returning from demos that use **`P`** as a debug shortcut

### Global (while a demo is running)

- **`P`** — instant return to the **engine main menu** (debug shortcut shared via [`Game/Common/MiniGameNavigation`](Game/Common/MiniGameNavigation.h); also used during development for fast iteration)
- **Forward / deferred** — in **Katamari**, a **clickable overlay button** toggles render mode (`Application` global overlay when the active game opts in via `WantsGlobalRenderModeToggleOverlay`)

### `Esc` behavior (current implementation)

- **From demos** — `Esc` acts as **Back**: nested UI (for example Pong’s internal flow, Solar System settings, Katamari particle panel) closes first; otherwise the game requests a switch back to **`MainMenuGame`**.
- **From the engine main menu** — `Esc` does **not** exit the process; use **EXIT** or close the window.
- **Quitting the process** — `Application::RequestQuitApplication` is used from the main menu **EXIT** item (see comments in [`Core/App/Application.cpp`](Core/App/Application.cpp)).

### Pong

- **Menus:** **W / S** or arrows, **Enter** / click; **`Esc`** steps back (from Pong’s main menu this returns to the **engine main menu**)
- **Playing:** **W / S** — left paddle; **Up / Down** — right paddle in **two-player** mode
- **`P`** — return to engine main menu

### Solar System

- **`F1`** — FPS camera
- **`F2`** — orbit camera
- **`O`** — projection mode toggle (standard FOV vs off-center perspective)
- **`Tab`** — settings panel
- **`W` `A` `S` `D`** — move (mode-dependent) / orbit zoom where applicable
- **Arrow keys** — look / orbit adjustments
- **Right mouse** — mouse look / orbit rotation
- **`Esc`** — closes the settings panel if open, otherwise returns to the engine main menu
- **`P`** — return to engine main menu

### Katamari

- **`W` `A` `S` `D`** — move
- **`Space`** — jump
- **`R`** — reset level
- **`Right mouse`** — camera orbit / drag
- **`Esc`** — closes particle settings if open, otherwise returns to the engine main menu
- **`P`** — return to engine main menu
- **`F3`** — collision debug draw
- **`F4`** — shadow cascade debug
- **`F5`** — GBuffer debug visualization
- **`F6`** — GBuffer picking inspector (see code paths; **deferred** mode)
- **`F7`** — transform gizmo / editor-style manipulator (ImGuizmo)

### Lighting test

- **`W` `A` `S` `D`** — move camera
- **Arrow keys** — rotate
- **Right mouse** — mouse look
- **`Esc`** or **`P`** — return to engine main menu

## Build requirements

- **OS:** Windows 10/11 (**DirectX 11** focused; not a cross-platform project today)
- **Toolchain:** **Visual Studio 2022** / MSVC recommended
- **CMake:** **3.21+**
- **Language:** **C++20**
- **Package manager:** [vcpkg](https://github.com/microsoft/vcpkg) with triplet **x64-windows**
- **Ports:** `directxtk`, `assimp` (see root [`CMakeLists.txt`](CMakeLists.txt))
- **System libs linked:** `d3d11`, `dxgi`, `d3dcompiler`, `dxguid`, `user32`, `gdi32`
- **Bundled third-party sources:** Dear **ImGui** and **ImGuizmo** under [`ThirdParty`](ThirdParty) (compiled as part of `MiniEngineDemo`)

## Troubleshooting

- **`CMAKE_TOOLCHAIN_FILE` not set / wrong path** — `find_package(directxtk)` / `find_package(assimp)` fails. Point `-DCMAKE_TOOLCHAIN_FILE=.../scripts/buildsystems/vcpkg.cmake` at your vcpkg checkout and keep **`VCPKG_TARGET_TRIPLET=x64-windows`** aligned with installed packages.
- **Wrong Visual Studio generator string** — use **`"Visual Studio 17 2022"`** for VS 2022. A mismatched year/name breaks configuration.
- **CMake preset fails** — ensure **`VCPKG_ROOT`** is set, **`Ninja`** is installed and on `PATH`, and vcpkg triplets match.
- **Shaders or assets missing at runtime** — `CMakeLists.txt` copies `Core/Shaders` and selected `Game/*/Assets` into the build tree; run a fresh build so **POST_BUILD** copy steps run. Run **`MiniEngineDemo.exe`** from a working directory that can see those folders next to the binary (Visual Studio places the exe under `Debug/` or `Release/`).
- **Black screen** — verify GPU supports D3D11, try **windowed** mode from your environment, and confirm shaders finished copying (reconfigure + rebuild).
- **Lighting test loads no meshes** — the sample loads models from paths such as `Core/Data/*.fbx` in code; if those files are absent in your tree or archive, the scene can still run with reduced geometry—check logs and asset paths in [`LightingTestGame.cpp`](Game/LightingTest/LightingTestGame.cpp).
- **Debug vs Release confusion** — multi-config generators require `--config Debug` or `--config Release` and matching output folders.

## How to add a new mini-game

1. Add **`Game/MyDemo/MyDemoGame.h`** and **`.cpp`** implementing **`IGame`** (`Initialize`, `Update`, `Render`, `Shutdown`, and optional overrides such as `OnRenderModeChanged`).
2. Register all new `.cpp` / `.h` files in the main **`add_executable(MiniEngineDemo ...)`** list in [`CMakeLists.txt`](CMakeLists.txt) (this project currently uses **one consolidated target**).
3. Wire the demo into the **engine main menu**: extend the button row and switch in [`Game/MainMenu/MainMenuGame.cpp`](Game/MainMenu/MainMenuGame.cpp) (mirror patterns from existing games).
4. If you need navigation back to the hub, call **`RequestReturnToEngineMainMenu`** from [`Game/Common/MiniGameNavigation`](Game/Common/MiniGameNavigation.h) on **`Esc`** / **`P`** consistent with other demos.
5. Add assets under **`Game/MyDemo/Assets`** and extend **`CMakeLists.txt`** `file(COPY ...)` / `add_custom_command` rules like existing games.
6. Document **controls** and learning goals in this README (and keep **`README_RU.md`** aligned or clearly noted).

## Project structure

```text
Mini-Engine-With-Games/
├── Core/                              # Shared runtime
│   ├── App/                           # Application, IGame / IGameHost, AppContext, services headers
│   ├── Platform/                      # Win32 window
│   ├── Input/                         # Keyboard + raw mouse
│   ├── Graphics/                      # D3D11 device, renderers, cameras, shaders, FrameRenderer, passes, picking
│   ├── Gameplay/                      # ECS-style scene, components, systems
│   ├── Physics/                       # Collision helpers and queries
│   ├── Math/                          # Transforms and helpers
│   ├── Assets/                        # Resolver + cache
│   ├── Audio/                         # DirectXTK audio wrapper
│   ├── UI/                            # Bitmap font, widgets, ImGui layer
│   ├── Editor/                        # Small tools (transform gizmo service)
│   └── Shaders/                       # HLSL sources copied next to the build
├── Game/
│   ├── MainMenu/                      # Runtime demo picker (engine hub)
│   ├── Common/                        # Shared mini-game helpers (main-menu navigation)
│   ├── Pong/
│   ├── SolarSystem/
│   ├── Katamari/
│   └── LightingTest/
├── ThirdParty/                        # imgui, imguizmo (built with the executable)
├── Images/                            # README media (GIFs / screenshots)
├── main.cpp                           # Entry: Application + MainMenuGame
└── CMakeLists.txt                     # Targets, dependencies, runtime copy rules
```

## Known limitations

- **Windows-only**, **DirectX 11-only** learning stack.
- **Not** a production engine: no asset pipeline, streaming, networking, or editor productization.
- **CMake** currently attaches most sources to a **single executable target**—easy to grep, harder to modularize.
- Several debug / tooling paths are **experimental** and demo-scoped (ImGui, gizmo, picking).
- **No** full level editor; content is mostly code-driven plus on-disk assets.
- Large binary GIFs are discouraged in Git; see [Media files and Git](#media-files-and-git).

## Roadmap

- More **student-oriented docs** (render pass walkthroughs, diagrams).
- **Cleaner CMake** split (core library vs. game modules vs. third-party).
- **Packaged releases** with known-good assets and a short verification checklist.
- Extra **debug visualizations** and “good first issue” tasks for newcomers.
- Optional **video clips** hosted on Releases to complement in-repo stills.

## Contributing

Contributions that keep the project approachable are welcome:

- Documentation and README fixes
- Build / preset / CI improvements
- Small, well-scoped **rendering** or **debug-tool** experiments (clearly marked when experimental)
- **Student-friendly** sample code or exercises

Suggested issue labels: **`good first issue`**, **`documentation`**, **`build`**, **`rendering`**, **`gameplay`**, **`bug`**, **`student-friendly`**.

## Media files and Git

GIFs grow huge quickly (full-screen, long duration, high FPS). This repo **ignores** `Images/game_play.gif` and `Images/solar_game_play.gif` so they are not committed. Keep small assets in Git (for example `pong_game_play.gif` is fine).

**Practical options:**

1. **Compress before committing** (stay roughly under a few MB for README): lower resolution, fewer colors, shorter loop, ~10–15 FPS. Tools: [ezgif.com](https://ezgif.com/optimize), ScreenToGif export settings, or `ffmpeg` (scale + fps + palette).
2. **MP4 in a Release** instead of GIF: much smaller for the same quality; link it from the README.
3. **Git LFS** only if you really need large binaries in Git: install [Git LFS](https://git-lfs.com/), then `git lfs track "*.gif"` before adding. Note GitHub LFS storage and bandwidth quotas.

Example `ffmpeg` idea (adjust width and fps to taste):

```bash
ffmpeg -i game_play.gif -vf "fps=12,scale=960:-1:flags=lanczos,split[s0][s1];[s0]palettegen[p];[s1][p]paletteuse" game_play_small.gif
```
