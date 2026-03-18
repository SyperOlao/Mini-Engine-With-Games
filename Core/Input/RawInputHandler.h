//
// Created by SyperOlao on 17.03.2026.
//

#ifndef PINGPONG_RAWINPUTHANDLER_H
#define PINGPONG_RAWINPUTHANDLER_H
#include <Windows.h>

#include <array>

class RawInputHandler final {
public:
    static RawInputHandler &Instance();

    RawInputHandler(const RawInputHandler &);

    RawInputHandler &operator=(const RawInputHandler &) = delete;

    RawInputHandler(RawInputHandler &&) = delete;

    RawInputHandler &operator=(RawInputHandler &&) = delete;

    void Initialize(HWND targetWindow);

    [[nodiscard]] LRESULT HandleMessage(UINT message, WPARAM wParam, LPARAM lParam);

    void Clear() noexcept;

    [[nodiscard]] bool IsKeyDown(USHORT virtualKey) const noexcept;

private:
    RawInputHandler() = default;

    void RegisterKeyboardDevice(HWND targetWindow);

    void ProcessRawInput(HRAWINPUT rawInputHandle);

    static USHORT NormalizeVirtualKey(const RAWKEYBOARD &keyboardData) noexcept;

private:
    HWND m_targetWindow{nullptr};
    bool m_isInitialized{false};
    std::array<bool, 256> m_currentState{};
};


#endif //PINGPONG_RAWINPUTHANDLER_H
