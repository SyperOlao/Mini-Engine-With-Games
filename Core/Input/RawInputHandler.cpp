//
// Created by SyperOlao on 17.03.2026.
//

#include "RawInputHandler.h"
#include <cstddef>
#include <stdexcept>
#include <vector>

RawInputHandler& RawInputHandler::Instance()
{
    static RawInputHandler instance;
    return instance;
}

void RawInputHandler::Initialize(HWND__ *const targetWindow)
{
    if (targetWindow == nullptr)
    {
        throw std::invalid_argument("RawInputHandler::Initialize received a null window handle");
    }

    m_targetWindow = targetWindow;
    RegisterKeyboardDevice(targetWindow);
    m_isInitialized = true;
}

LRESULT RawInputHandler::HandleMessage(const UINT message, const WPARAM /*wParam*/, const LPARAM lParam)
{
    if (message != WM_INPUT)
    {
        return 1;
    }

    ProcessRawInput(reinterpret_cast<HRAWINPUT>(lParam));
    return 0;
}

void RawInputHandler::Clear() noexcept
{
    m_currentState.fill(false);
}

bool RawInputHandler::IsKeyDown(const USHORT virtualKey) const noexcept
{
    if (virtualKey >= m_currentState.size())
    {
        return false;
    }

    return m_currentState[virtualKey];
}

void RawInputHandler::RegisterKeyboardDevice(HWND targetWindow)
{
    RAWINPUTDEVICE rawInputDevice{};
    rawInputDevice.usUsagePage = 0x01;
    rawInputDevice.usUsage = 0x06;
    rawInputDevice.dwFlags = RIDEV_INPUTSINK;
    rawInputDevice.hwndTarget = targetWindow;

    if (!RegisterRawInputDevices(&rawInputDevice, 1, sizeof(rawInputDevice)))
    {
        throw std::runtime_error("RegisterRawInputDevices failed for keyboard");
    }
}

void RawInputHandler::ProcessRawInput(const HRAWINPUT rawInputHandle)
{
    if (!m_isInitialized)
    {
        return;
    }

    UINT dataSize = 0;
    if (GetRawInputData(rawInputHandle, RID_INPUT, nullptr, &dataSize, sizeof(RAWINPUTHEADER)) != 0)
    {
        return;
    }

    std::vector<std::byte> buffer(dataSize);
    if (GetRawInputData(rawInputHandle, RID_INPUT, buffer.data(), &dataSize, sizeof(RAWINPUTHEADER)) != dataSize)
    {
        return;
    }

    const auto* rawInput = reinterpret_cast<const RAWINPUT*>(buffer.data());
    if (rawInput->header.dwType != RIM_TYPEKEYBOARD)
    {
        return;
    }

    const RAWKEYBOARD& keyboardData = rawInput->data.keyboard;
    const USHORT virtualKey = NormalizeVirtualKey(keyboardData);
    if (virtualKey >= m_currentState.size())
    {
        return;
    }

    if (keyboardData.Message == WM_KEYDOWN || keyboardData.Message == WM_SYSKEYDOWN)
    {
        m_currentState[virtualKey] = true;
        return;
    }

    if (keyboardData.Message == WM_KEYUP || keyboardData.Message == WM_SYSKEYUP)
    {
        m_currentState[virtualKey] = false;
        return;
    }

    const bool isBreak = (keyboardData.Flags & RI_KEY_BREAK) != 0;
    m_currentState[virtualKey] = !isBreak;
}

USHORT RawInputHandler::NormalizeVirtualKey(const RAWKEYBOARD& keyboardData) noexcept
{
    USHORT virtualKey = keyboardData.VKey;
    if (virtualKey == 255)
    {
        return 255;
    }

    const UINT scanCode = keyboardData.MakeCode;
    const bool isExtended = (keyboardData.Flags & RI_KEY_E0) != 0;

    switch (virtualKey)
    {
        case VK_SHIFT:
            virtualKey = static_cast<USHORT>(MapVirtualKeyW(scanCode, MAPVK_VSC_TO_VK_EX));
            break;
        case VK_CONTROL:
            virtualKey = static_cast<USHORT>(isExtended ? VK_RCONTROL : VK_LCONTROL);
            break;
        case VK_MENU:
            virtualKey = static_cast<USHORT>(isExtended ? VK_RMENU : VK_LMENU);
            break;
        default:
            break;
    }

    return virtualKey;
}