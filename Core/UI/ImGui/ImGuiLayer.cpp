#include "Core/UI/ImGui/ImGuiLayer.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include <Windows.h>

#include <stdexcept>

namespace
{
bool g_imguiInitialized = false;
}

void ImGuiLayer::Initialize(HWND__ *const window, ID3D11Device *const device, ID3D11DeviceContext *const deviceContext)
{
    if (g_imguiInitialized)
    {
        return;
    }

    if (window == nullptr || device == nullptr || deviceContext == nullptr)
    {
        throw std::invalid_argument("ImGuiLayer::Initialize requires window, device, and context.");
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();

    if (!ImGui_ImplWin32_Init(window))
    {
        ImGui::DestroyContext();
        throw std::runtime_error("ImGui_ImplWin32_Init failed.");
    }

    if (!ImGui_ImplDX11_Init(device, deviceContext))
    {
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        throw std::runtime_error("ImGui_ImplDX11_Init failed.");
    }

    g_imguiInitialized = true;
}

void ImGuiLayer::Shutdown() noexcept
{
    if (!g_imguiInitialized)
    {
        return;
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    g_imguiInitialized = false;
}

bool ImGuiLayer::IsInitialized() noexcept
{
    return g_imguiInitialized;
}

void ImGuiLayer::BeginFrame()
{
    if (!g_imguiInitialized)
    {
        return;
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void ImGuiLayer::RenderDrawData()
{
    if (!g_imguiInitialized)
    {
        return;
    }

    ImGui::Render();
    ImDrawData *const drawData = ImGui::GetDrawData();
    if (drawData != nullptr)
    {
        ImGui_ImplDX11_RenderDrawData(drawData);
    }
}

void ImGuiLayer::OnBackbufferResize() noexcept
{
    if (!g_imguiInitialized)
    {
        return;
    }

    ImGui_ImplDX11_InvalidateDeviceObjects();
    ImGui_ImplDX11_CreateDeviceObjects();
}
