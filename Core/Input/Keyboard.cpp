//
// Created by SyperOlao on 17.03.2026.
//

#include "Keyboard.h"
#include "RawInputHandler.h"


void Keyboard::Update()
{
    m_previousState = m_currentState;

    for (std::size_t index = 0; index < KeyCount; ++index)
    {
        m_currentState[index] = RawInputHandler::Instance().IsKeyDown(static_cast<USHORT>(index));
    }
}

bool Keyboard::IsKeyDown(const Key key) const noexcept
{
    return IsVirtualKeyDown(static_cast<USHORT>(key));
}

bool Keyboard::WasKeyPressed(const Key key) const noexcept
{
    return WasVirtualKeyPressed(static_cast<USHORT>(key));
}

bool Keyboard::WasKeyReleased(const Key key) const noexcept
{
    return WasVirtualKeyReleased(static_cast<USHORT>(key));
}

bool Keyboard::IsVirtualKeyDown(const USHORT virtualKey) const noexcept
{
    if (virtualKey >= KeyCount)
    {
        return false;
    }

    return m_currentState[virtualKey];
}

bool Keyboard::WasVirtualKeyPressed(const USHORT virtualKey) const noexcept
{
    if (virtualKey >= KeyCount)
    {
        return false;
    }

    return m_currentState[virtualKey] && !m_previousState[virtualKey];
}

bool Keyboard::WasVirtualKeyReleased(const USHORT virtualKey) const noexcept
{
    if (virtualKey >= KeyCount)
    {
        return false;
    }

    return !m_currentState[virtualKey] && m_previousState[virtualKey];
}