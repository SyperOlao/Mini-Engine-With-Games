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

Компактный **учебный мини-движок на C++20 и DirectX 11**: общий модуль `Core`, несколько играбельных демо и небольшой **ECS-стильный** слой геймплея, который можно прочитать от начала до конца без «багажа» коммерческого движка.

Это **песочница для обучения и экспериментов**, а не промышленный игровой движок.

**Англоязычная версия (эталон для PR и международной аудитории):** [README.md](README.md). Этот файл — зеркальный перевод; при изменении структуры или фактов правьте **оба** README.

**Быстрые ссылки**

- [Скачать последний релиз для Windows](https://github.com/SyperOlao/Mini-Engine-With-Games/releases/latest)
- [Быстрый старт (сборка и запуск)](#quick-start)
- [Обзор архитектуры](#architecture-overview)
- [Как добавить мини-игру](#how-to-add-a-new-mini-game)
- [Управление](#controls)

## Содержание

- [Превью](#preview)
- [Зачем этот репозиторий](#why-this-project-exists)
- [Чему можно научиться](#what-you-can-learn-from-this-repo)
- [Быстрый старт](#quick-start)
- [Игры и демо](#games-and-demos)
- [Обзор архитектуры](#architecture-overview)
- [Возможности рендера](#rendering-features)
- [Управление](#controls)
- [Требования к сборке](#build-requirements)
- [Решение проблем](#troubleshooting)
- [Как добавить новую мини-игру](#how-to-add-a-new-mini-game)
- [Структура проекта](#project-structure)
- [Известные ограничения](#known-limitations)
- [Дорожная карта](#roadmap)
- [Участие в проекте](#contributing)
- [Медиафайлы и Git](#media-files-and-git)

<a id="preview"></a>

## Превью

![Обзорное демо](Images/demo2.gif)

Ниже — **те же файлы**, что вставлены в [README.md](README.md) в разделах **Games and demos** и **Preview** (пути `Images/...` совпадают с английской версией).

![Pong — геймплей](Images/pong_game_play.gif)

![Солнечная система — кадр](Images/img5.png)

![Katamari — геймплей](Images/img12.png)

![Katamari — частицы](Images/img13.png)

![Katamari — отладка каскадов теней](Images/img14.png)

![Katamari — отладка GBuffer](Images/img9.png)

![Lighting test — сцена](Images/img15.png)

<a id="why-this-project-exists"></a>

## Зачем этот репозиторий

Студентам и разработчикам **начального / среднего** уровня по графике и геймплею часто нужен **небольшой, собираемый код на DirectX 11**, чтобы разобраться:

- как в современном C++ сочетаются **окно, ввод, тайминг и цикл рендера**;
- как отделить **«движковый» Core** от модулей **Game**;
- как на практике выглядят **forward и deferred**, простые **тени и GBuffer**;
- как **лёгкая ECS-подобная** сцена тянет мини-игру без полноценного редактора.

Репозиторий намеренно меньше коммерческого движка: можно проследить фичу от `main.cpp` через `Application`, `IGame`, проходы рендера и до конкретного демо.

<a id="what-you-can-learn-from-this-repo"></a>

## Чему можно научиться в этом репозитории

- **C++20** в небольшом исполняемом файле: возможности языка и STL там, где они упрощают код, плюс явные границы владения между играми и ядром.
- **Игровой цикл и жизненный цикл**: сообщения окна, `Application` (обновление и отрисовка), дельта времени.
- **Смена модулей `IGame`**: каждое демо — класс с одним контрактом; оболочка может менять игру в рантайме (`IGameHost`, `RequestSwitchGame`).
- **Разделение Core / Game**: общие сервисы в `AppContext` (см. `Core/App/AppContext.h`, `Core/App/EngineServices.h`); контент и логика демо — в `Game/*`.
- **Основы DirectX 11**: устройство и swap chain, шейдеры из скопированного `Core/Shaders`, простые 2D и 3D пути отрисовки.
- **Forward и deferred**: оркестрация через `FrameRenderer` и проходы в `Core/Graphics/Rendering/Pipeline/Passes`.
- **GBuffer**, отложенное освещение и композит, **отладочная визуализация GBuffer** (горячие клавиши в демо).
- **Каскадные тени** для направленного света и **отладка каскадов**.
- **ECS-стиль геймплея**: `Scene`, сущности, компоненты, системы в `Core/Gameplay` (плюс системы Katamari).
- **Помощники коллизий** в `Core/Physics` и `Core/Gameplay` (2D и 3D, отладочная отрисовка).
- **UI**: битовый шрифт, кнопки, switcher, слайдеры, простая вёрстка в `Core/UI`.
- **Звук** через **DirectXTK** (`Core/Audio`).
- **Отладка и инструменты**: слой **ImGui**, гизмо на **ImGuizmo**, **GBuffer picking** в Katamari (экспериментально, для обучения, не полноценный редактор).

<a id="quick-start"></a>

## Быстрый старт

### A) Готовый бинарник под Windows

1. Откройте [последний релиз](https://github.com/SyperOlao/Mini-Engine-With-Games/releases/latest).
2. Скачайте архив для Windows **x64**.
3. Распакуйте и запустите **`MiniEngineDemo.exe`**.

Приложение открывается в **главном меню движка** (`MainMenuGame`). Выберите демо или пункт **EXIT** для выхода.

### B) Сборка из исходников (Windows 10/11, Visual Studio 2022, CMake 3.21+, vcpkg, x64-windows)

**1. vcpkg (первый раз)**

```powershell
git clone https://github.com/microsoft/vcpkg.git C:\dev\vcpkg
cd C:\dev\vcpkg
.\bootstrap-vcpkg.bat
vcpkg install directxtk:x64-windows assimp:x64-windows
```

Для текущей сессии терминала (пример путей):

```powershell
$env:VCPKG_ROOT = "C:\dev\vcpkg"
$env:Path = "$env:VCPKG_ROOT;$env:Path"
```

**2. Конфигурация и сборка (генератор Visual Studio)**

Из корня репозитория (подставьте свой путь к vcpkg вместо `C:/dev/vcpkg`, если отличается):

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

**3. Запуск**

При **мультиконфигурационном** генераторе Visual Studio исполняемый файл Debug обычно здесь:

```powershell
.\cmake-build-debug\Debug\MiniEngineDemo.exe
```

При **одноконфигурационном** генераторе (например **Ninja**) exe чаще лежит рядом с корнем дерева сборки, например `.\build\MiniEngineDemo.exe`.

### По желанию: пресет CMake (Ninja)

В [`CMakePresets.json`](CMakePresets.json) задан пресет **Ninja**, которому нужны:

- переменная окружения `VCPKG_ROOT` на ваш клон vcpkg;
- **Ninja** в `PATH`.

Пример:

```powershell
$env:VCPKG_ROOT = "C:\dev\vcpkg"
cmake --preset default
cmake --build build
.\build\MiniEngineDemo.exe
```

В `CMakePresets.json` для triplet указано **x64-windows** — как в команде `vcpkg install` выше.

<a id="games-and-demos"></a>

## Игры и демо

В рантайме **`MiniEngineDemo.exe`** всегда стартует с **`MainMenuGame`** из [`main.cpp`](main.cpp). Демо выбираются из меню. Корректный выход из процесса — пункт **EXIT** в этом меню (поведение `Esc` см. [Управление](#controls)).

### Главное меню (`Game/MainMenu`)

**Что показывает:** простая навигация по UI, звук при действиях, переключение активной игры через `IGameHost::RequestSwitchGame`, возврат в хаб через `Game/Common/MiniGameNavigation`.

**С чего читать код:** [`Game/MainMenu/MainMenuGame.cpp`](Game/MainMenu/MainMenuGame.cpp), [`Game/Common/MiniGameNavigation.cpp`](Game/Common/MiniGameNavigation.cpp), [`Core/App/Application.cpp`](Core/App/Application.cpp) (`RequestSwitchGame`, `RequestQuitApplication`).

### Pong (`Game/Pong`)

**Что показывает:** 2D-геймплей, локальный UI (меню, настройки, матч), коллизии, общие паттерны аудио и UI.

**С чего читать код:** [`Game/Pong/PongGame.cpp`](Game/Pong/PongGame.cpp), [`Game/Pong/UI/PongUI.cpp`](Game/Pong/UI/PongUI.cpp).

#### Геймплей

![Pong — геймплей](Images/pong_game_play.gif)

### Solar System (`Game/SolarSystem`)

**Что показывает:** обновление 3D-сцены, камеры FPS и орбиты, панель настроек, звук от «движка» камеры.

**С чего читать код:** [`Game/SolarSystem/SolarSystemGame.cpp`](Game/SolarSystem/SolarSystemGame.cpp), [`Game/SolarSystem/SolarSystemScene.cpp`](Game/SolarSystem/SolarSystemScene.cpp).

Кадр-стопка (тяжёлые GIF в репозиторий не кладём — сожмите локально или приложите ролик к релизу):

![Солнечная система — кадр](Images/img5.png)

### Katamari (`Game/Katamari`)

**Что показывает:** шар и сбор объектов, follow-камера, интеграция с deferred, GPU-частицы, отладка коллизий и каскадов теней, отладка GBuffer, picking по GBuffer, трансформ-гизмо на **ImGuizmo** (в учебных целях).

**С чего читать код:** [`Game/Katamari/KatamariGame.cpp`](Game/Katamari/KatamariGame.cpp), системы в [`Game/Katamari/Systems`](Game/Katamari/Systems), [`Core/Graphics/Rendering/FrameRenderer.cpp`](Core/Graphics/Rendering/FrameRenderer.cpp).

![Katamari — геймплей](Images/img12.png)

![Katamari — частицы](Images/img13.png)

![Katamari — отладка каскадов теней](Images/img14.png)

![Katamari — отладка GBuffer](Images/img9.png)

### Lighting test (`Game/LightingTest`)

**Что показывает:** forward-освещение примитивов и загруженных моделей в маленькой сцене для проверки света и камеры.

**С чего читать код:** [`Game/LightingTest/LightingTestGame.cpp`](Game/LightingTest/LightingTestGame.cpp).

![Lighting test — сцена](Images/img15.png)

<a id="architecture-overview"></a>

## Обзор архитектуры

Сверху вниз: **`Application`** владеет окном, устройством DirectX, вводом, аудио, кэшем ассетов и контекстом рендера. Держит активную **`IGame`**, вызывает `Initialize` / `Update` / `Render` / `Shutdown` и применяет отложенную смену игры из **`IGameHost`**.

- **`Core/App`** — `Application`, `IGame`, `IGameHost`, `AppContext`, таймер, фатальные ошибки, размер окна по умолчанию (`ApplicationDefaults.h`).
- **`Core/Graphics`** — `GraphicsDevice`, 2D/3D-хелперы отрисовки, камеры, модели, ресурсы deferred, **`FrameRenderer`** и **`Core/Graphics/Rendering/Pipeline/Passes/*`** (геометрия, deferred, тени, частицы, оверлеи, UI).
- **`Core/Gameplay`** — `Scene`, сущности, компоненты, встроенные системы (трансформ, скорость, коллизии, синхронизация с рендером).
- **`Core/Input`** — клавиатура и сырые дельты мыши (`RawInputHandler`).
- **`Core/Audio`** — загрузка через DirectXTK, one-shot, лупы.
- **`Core/UI`** — битовый шрифт и лёгкие виджеты; ImGui в `Core/UI/ImGui`.
- **`Core/Physics`** — общие типы коллизий, запросы, хелперы для демо и геймплея.
- **`Core/Editor`** — мелкие утилиты вроде **`TransformGizmoService`** (ImGuizmo), используется в Katamari.
- **`Game/*`** — самостоятельные демо с `IGame`; общая навигация — в **`Game/Common`**.

### С чего начать чтение кода

1. [`main.cpp`](main.cpp) — точка входа: `Application` и `MainMenuGame`.
2. [`Core/App/Application.cpp`](Core/App/Application.cpp) — цикл кадра, ресайз, смена игры, выход.
3. [`Core/App/IGame.h`](Core/App/IGame.h) — контракт для каждого демо.
4. [`Game/MainMenu/MainMenuGame.cpp`](Game/MainMenu/MainMenuGame.cpp) — меню выбора демо.
5. [`Game/Pong/PongGame.cpp`](Game/Pong/PongGame.cpp) — минимальный целостный пример игры.
6. [`Core/Graphics/Rendering/FrameRenderer.cpp`](Core/Graphics/Rendering/FrameRenderer.cpp) — как на кадр вешаются проходы.
7. [`Core/Graphics/Rendering/Pipeline/Passes`](Core/Graphics/Rendering/Pipeline/Passes) — отдельные проходы (forward/deferred, отладка, UI).
8. [`Game/Katamari`](Game/Katamari) — ECS-системы плюс связка с рендером и отладкой.

### Опция разработчика: пропустить главное меню

Для быстрой итерации в [`main.cpp`](main.cpp) по-прежнему можно создать другой `IGame` напрямую (например `std::make_unique<KatamariGame>()` вместо `MainMenuGame`). Для учебной траектории предполагается **меню в рантайме**; правка `main.cpp` — необязательна.

<a id="rendering-features"></a>

## Возможности рендера

Есть в коде сегодня (детали — в проходах `Core/Graphics/Rendering/Pipeline/Passes`):

- **Forward**-путь в стиле Phong для простых сцен.
- **Deferred**: геометрия, свет, композит; цели и разметка **GBuffer** в `Core/Graphics/Rendering/Deferred`.
- **Отладочная визуализация GBuffer** (переключатель в Katamari; см. [Управление](#controls)).
- **Каскадные карты теней** для направленного света и **отладка каскадов**.
- **GPU-частицы** (compute/update + отрисовка; настройки из UI Katamari).
- **Picking по GBuffer** (`Core/Graphics/Picking/GBufferPickingService`) — инспекция попаданий и выбор для гизмо в Katamari при включённом инспекторе (**режим deferred**).
- **ImGuizmo** и манипуляция трансформом (`Core/Editor/TransformGizmoService`) — экспериментальный учебный инструмент, не готовый редактор.

Некоторые сочетания (например picking в forward) намеренно ограничены; продвинутый инструментарий считайте **экспериментальным**, где об этом говорят HUD или ветки кода.

<a id="controls"></a>

## Управление

### Главное меню движка (`MainMenuGame`)

- **W / S** или **стрелки вверх/вниз** — смена пункта
- **Enter** или **клик мышью** — выбор (**Pong**, **Solar System**, **Katamari**, **Lighting test**, **EXIT**)
- **`Esc`** — **игнорируется** (некуда «возвращаться» назад); для выхода — **EXIT** или выбор демо
- **`P`** — намеренно игнорируется, чтобы не «протекал» ввод после демо, где **`P`** — отладочный возврат в меню

### Глобально (пока запущено демо)

- **`P`** — мгновенный возврат в **главное меню движка** (отладочный шорткат через [`Game/Common/MiniGameNavigation`](Game/Common/MiniGameNavigation.h))
- **Forward / deferred** — в **Katamari** кнопка-**оверлей** переключает режим (`Application` показывает глобальную кнопку, если игра вернула `true` из `WantsGlobalRenderModeToggleOverlay`)

### Поведение `Esc` (как в коде)

- **Из демо** — `Esc` = **назад**: сначала закрывается вложенный UI (меню Pong, панель Solar System, панель частиц Katamari и т.д.), иначе запрос смены игры на **`MainMenuGame`**.
- **Из главного меню движка** — `Esc` **не** завершает процесс; выход — **EXIT** или закрытие окна.
- **Завершение процесса** — `Application::RequestQuitApplication` вызывается с пункта **EXIT** главного меню (см. комментарии в [`Core/App/Application.cpp`](Core/App/Application.cpp)).

### Pong

- **Меню:** **W / S** или стрелки, **Enter** / клик; **`Esc`** — шаг назад (с главного меню Pong — возврат в **главное меню движка**)
- **Игра:** **W / S** — левая ракетка; **стрелки вверх/вниз** — правая ракетка в режиме **двух игроков**
- **`P`** — возврат в главное меню движка

### Solar System

- **`F1`** — камера FPS
- **`F2`** — орбитальная камера
- **`O`** — смена режима проекции (обычный FOV и off-center perspective)
- **`Tab`** — панель настроек
- **`W` `A` `S` `D`** — движение (зависит от режима) / зум орбиты где применимо
- **Стрелки** — поворот / орбита
- **Правая кнопка мыши** — обзор мышью / орбита
- **`Esc`** — закрыть панель настроек, если открыта, иначе возврат в главное меню движка
- **`P`** — возврат в главное меню движка

### Katamari

- **`W` `A` `S` `D`** — движение
- **`Space`** — прыжок
- **`R`** — сброс уровня
- **Правая кнопка мыши** — камера (орбита / перетаскивание)
- **`Esc`** — закрыть настройки частиц, если открыты, иначе возврат в главное меню движка
- **`P`** — возврат в главное меню движка
- **`F3`** — отладочная отрисовка коллизий
- **`F4`** — отладка каскадов теней
- **`F5`** — отладочная визуализация GBuffer
- **`F6`** — инспектор picking по GBuffer (см. код; **режим deferred**)
- **`F7`** — режим гизмо / «редакторского» манипулятора (ImGuizmo)

### Lighting test

- **`W` `A` `S` `D`** — движение камеры
- **Стрелки** — поворот
- **Правая кнопка мыши** — обзор мышью
- **`Esc`** или **`P`** — возврат в главное меню движка

<a id="build-requirements"></a>

## Требования к сборке

- **ОС:** Windows 10/11 (фокус на **DirectX 11**; кроссплатформенности сейчас нет)
- **Инструменты:** рекомендуется **Visual Studio 2022** / MSVC
- **CMake:** **3.21+**
- **Язык:** **C++20**
- **Пакеты:** [vcpkg](https://github.com/microsoft/vcpkg), triplet **x64-windows**
- **Порты:** `directxtk`, `assimp` (см. корневой [`CMakeLists.txt`](CMakeLists.txt))
- **Системные библиотеки:** `d3d11`, `dxgi`, `d3dcompiler`, `dxguid`, `user32`, `gdi32`
- **Исходники сторонних библиотек в репозитории:** **ImGui** и **ImGuizmo** в [`ThirdParty`](ThirdParty) (собираются в составе `MiniEngineDemo`)

<a id="troubleshooting"></a>

## Решение проблем

- **`CMAKE_TOOLCHAIN_FILE` не задан или неверный путь** — падает `find_package(directxtk)` / `find_package(assimp)`. Укажите `-DCMAKE_TOOLCHAIN_FILE=.../scripts/buildsystems/vcpkg.cmake` и **`VCPKG_TARGET_TRIPLET=x64-windows`**, согласованный с установленными пакетами.
- **Неверная строка генератора Visual Studio** — для VS 2022 используйте **`"Visual Studio 17 2022"`**. Ошибка в годе/имени ломает конфигурацию.
- **Пресет CMake не работает** — проверьте **`VCPKG_ROOT`**, установлен ли **Ninja** в `PATH`, совпадают ли triplets.
- **Нет шейдеров или ассетов в рантайме** — `CMakeLists.txt` копирует `Core/Shaders` и выбранные `Game/*/Assets` в дерево сборки; выполните пересборку, чтобы сработали шаги **POST_BUILD**. Запускайте **`MiniEngineDemo.exe`** из каталога, где рядом с exe видны эти папки (у VS exe лежит в `Debug/` или `Release/`).
- **Чёрный экран** — проверьте поддержку D3D11, оконный режим, что шейдеры скопировались (переконфигурация + сборка).
- **Lighting test без мешей** — в коде загрузка путей вида `Core/Data/*.fbx`; если файлов нет в дереве или архиве, сцена может быть неполной — смотрите логи и пути в [`LightingTestGame.cpp`](Game/LightingTest/LightingTestGame.cpp).
- **Путаница Debug / Release** — для мультиконфигурации нужны `--config Debug` или `--config Release` и соответствующие подкаталоги вывода.

<a id="how-to-add-a-new-mini-game"></a>

## Как добавить новую мини-игру

1. Добавьте **`Game/MyDemo/MyDemoGame.h`** и **`.cpp`** с реализацией **`IGame`** (`Initialize`, `Update`, `Render`, `Shutdown` и при необходимости `OnRenderModeChanged` и др.).
2. Зарегистрируйте все новые `.cpp` / `.h` в списке **`add_executable(MiniEngineDemo ...)`** в [`CMakeLists.txt`](CMakeLists.txt) (сейчас проект использует **одну общую цель**).
3. Подключите демо к **главному меню движка**: расширьте ряд кнопок и `switch` в [`Game/MainMenu/MainMenuGame.cpp`](Game/MainMenu/MainMenuGame.cpp) по образцу существующих игр.
4. Для возврата в хаб вызывайте **`RequestReturnToEngineMainMenu`** из [`Game/Common/MiniGameNavigation`](Game/Common/MiniGameNavigation.h) на **`Esc`** / **`P`** в том же духе, что и в других демо.
5. Ассеты кладите в **`Game/MyDemo/Assets`** и добавьте в **`CMakeLists.txt`** правила `file(COPY ...)` / `add_custom_command`, как у других игр.
6. Опишите **управление** и учебные цели в **README.md** и в этом **README_RU.md**.

<a id="project-structure"></a>

## Структура проекта

```text
Mini-Engine-With-Games/
├── Core/                              # Общий рантайм
│   ├── App/                           # Application, IGame / IGameHost, AppContext, заголовки сервисов
│   ├── Platform/                      # Окно Win32
│   ├── Input/                         # Клавиатура и сырая мышь
│   ├── Graphics/                      # D3D11, рендереры, камеры, шейдеры, FrameRenderer, проходы, picking
│   ├── Gameplay/                      # ECS-стиль: сцена, компоненты, системы
│   ├── Physics/                       # Коллизии и запросы
│   ├── Math/                          # Трансформы и хелперы
│   ├── Assets/                        # Резолвер и кэш
│   ├── Audio/                         # Обёртка над DirectXTK Audio
│   ├── UI/                            # Шрифт, виджеты, слой ImGui
│   ├── Editor/                        # Мелкие инструменты (сервис гизмо)
│   └── Shaders/                       # Исходники HLSL, копируются рядом со сборкой
├── Game/
│   ├── MainMenu/                      # Выбор демо в рантайме (хаб движка)
│   ├── Common/                        # Общие хелперы мини-игр (возврат в меню)
│   ├── Pong/
│   ├── SolarSystem/
│   ├── Katamari/
│   └── LightingTest/
├── ThirdParty/                        # imgui, imguizmo (собираются с exe)
├── Images/                            # Медиа для README (GIF и скриншоты)
├── main.cpp                           # Вход: Application + MainMenuGame
└── CMakeLists.txt                     # Цели, зависимости, правила копирования
```

<a id="known-limitations"></a>

## Известные ограничения

- Только **Windows** и только **DirectX 11** как учебный стек.
- **Не** промышленный движок: нет полноценного пайплайна ассетов, стриминга, сети, продуктового редактора.
- **CMake** сейчас тянет большинство исходников в **одну цель** `MiniEngineDemo` — удобно искать по коду, неудобно модуляризовать.
- Часть отладочного инструментария **экспериментальна** и привязана к демо (ImGui, гизмо, picking).
- **Нет** полноценного level-редактора; контент в основном из кода и файлов на диске.
- Большие бинарные GIF в Git не приветствуются; см. [Медиафайлы и Git](#media-files-and-git).

<a id="roadmap"></a>

## Дорожная карта

- Больше **документации для студентов** (разбор проходов рендера, схемы).
- **Чище разнести CMake** (ядро, модули игр, third-party).
- **Релизы** с проверенным набором ассетов и коротким чеклистом проверки.
- Дополнительные **отладочные визуализации** и задачи с меткой «good first issue».
- По желанию — **видеоклипы** на странице Releases в дополнение к скриншотам в репозитории.

<a id="contributing"></a>

## Участие в проекте

Приветствуются изменения, которые сохраняют проект доступным для чтения:

- правки документации и README;
- улучшения сборки, пресетов, CI;
- небольшие **рендер**- или **отладочные** эксперименты (явно помечайте экспериментальность);
- **учебные** примеры и упражнения.

Предлагаемые метки задач: **`good first issue`**, **`documentation`**, **`build`**, **`rendering`**, **`gameplay`**, **`bug`**, **`student-friendly`**.

<a id="media-files-and-git"></a>

## Медиафайлы и Git

GIF быстро раздуваются (полный экран, длинная длительность, высокий FPS). В репозитории **игнорируются** `Images/game_play.gif` и `Images/solar_game_play.gif`, чтобы они не попадали в коммиты. В Git лучше держать небольшие ассеты (например `pong_game_play.gif` — нормально).

**Практичные варианты:**

1. **Сжимать перед коммитом** (ориентир — порядка нескольких МБ для README): ниже разрешение, меньше цветов, короткий цикл, ~10–15 FPS. Инструменты: [ezgif.com](https://ezgif.com/optimize), настройки экспорта ScreenToGif или `ffmpeg` (масштаб + fps + палитра).
2. **MP4 в релизе** вместо GIF: при том же качестве файл меньше; дайте ссылку из README.
3. **Git LFS** только если большие бинарники действительно нужны в Git: [Git LFS](https://git-lfs.com/), затем `git lfs track "*.gif"` до добавления файлов. Учитывайте квоты GitHub LFS.

Пример идеи с `ffmpeg` (подберите ширину и fps):

```bash
ffmpeg -i game_play.gif -vf "fps=12,scale=960:-1:flags=lanczos,split[s0][s1];[s0]palettegen[p];[s1][p]paletteuse" game_play_small.gif
```
