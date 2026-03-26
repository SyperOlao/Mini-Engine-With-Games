//
// Created by SyperOlao on 19.03.2026.
//

#include "Button.h"
#include "BitmapFont.h"
#include "../Input/InputSystem.h"

bool Button::HandleKeyboard(const InputSystem &input, const bool isSelected) const noexcept {
    if (!Enabled || !isSelected) {
        return false;
    }

    return input.GetKeyboard().WasKeyPressed(Key::Enter);
}

bool Button::IsHovered(const float mousePositionX, const float mousePositionY) const noexcept {
    if (!Enabled) {
        return false;
    }

    if (mousePositionX < Bounds.X || mousePositionX > Bounds.X + Bounds.Width) {
        return false;
    }

    if (mousePositionY < Bounds.Y || mousePositionY > Bounds.Y + Bounds.Height) {
        return false;
    }

    return true;
}

bool Button::HandleMouseClick(
    const float mousePositionX,
    const float mousePositionY,
    const bool wasLeftPressed
) const noexcept {
    if (!wasLeftPressed) {
        return false;
    }

    return IsHovered(mousePositionX, mousePositionY);
}

void Button::Draw(
    const ShapeRenderer2D &renderer,
    const BitmapFont &font,
    const ButtonStyle &style,
    const bool isSelected
) const {
    const Color background = isSelected ? style.SelectedBackground : style.NormalBackground;
    const Color border = isSelected ? style.Accent : Color(0.35f, 0.35f, 0.42f, 1.0f);
    const Color textColor = isSelected ? style.SelectedText : style.Text;

    renderer.DrawFilledRect(Bounds, background);

    renderer.DrawFilledRect(Bounds.X, Bounds.Y, Bounds.Width, style.BorderThickness, border);
    renderer.DrawFilledRect(Bounds.X, Bounds.Y + Bounds.Height - style.BorderThickness, Bounds.Width,
                            style.BorderThickness, border);
    renderer.DrawFilledRect(Bounds.X, Bounds.Y, style.BorderThickness, Bounds.Height, border);
    renderer.DrawFilledRect(Bounds.X + Bounds.Width - style.BorderThickness, Bounds.Y, style.BorderThickness,
                            Bounds.Height, border);

    const float textWidth = BitmapFont::MeasureTextWidth(Label, style.TextScale);
    const float textHeight = BitmapFont::MeasureTextHeight(style.TextScale);

    const float textX = Bounds.X + (Bounds.Width - textWidth) * 0.5f;
    const float textY = Bounds.Y + (Bounds.Height - textHeight) * 0.5f;

    BitmapFont::DrawString(renderer, textX, textY, Label, textColor, style.TextScale);
}
