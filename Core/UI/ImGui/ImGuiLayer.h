#ifndef PINGPONG_IMGUI_LAYER_H
#define PINGPONG_IMGUI_LAYER_H

#include <d3d11.h>

struct HWND__;

class ImGuiLayer final
{
public:
    static void Initialize(HWND__ *window, ID3D11Device *device, ID3D11DeviceContext *deviceContext);

    static void Shutdown() noexcept;

    [[nodiscard]] static bool IsInitialized() noexcept;

    static void BeginFrame();

    static void RenderDrawData();

    static void OnBackbufferResize() noexcept;

private:
    ImGuiLayer() = default;
};

#endif
