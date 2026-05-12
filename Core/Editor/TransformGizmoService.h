#ifndef PINGPONG_TRANSFORMGIZMOSERVICE_H
#define PINGPONG_TRANSFORMGIZMOSERVICE_H

#include "Core/Gameplay/EntityId.h"
#include "Core/Graphics/Picking/GBufferPickingService.h"

#include <SimpleMath.h>

#include <cstdint>

class Camera;
class Scene;

enum class TransformGizmoOperation : std::uint8_t
{
    Translate,
    Rotate,
    Scale
};

enum class TransformGizmoSpace : std::uint8_t
{
    Local,
    World
};

struct TransformGizmoState final
{
    bool Enabled{false};
    bool HasSelection{false};
    EntityId SelectedEntityId{0u};
    TransformGizmoOperation Operation{TransformGizmoOperation::Translate};
    TransformGizmoSpace Space{TransformGizmoSpace::World};
    bool SnapEnabled{false};
    DirectX::SimpleMath::Vector3 TranslationSnap{1.0f, 1.0f, 1.0f};
    float RotationSnapDegrees{15.0f};
    DirectX::SimpleMath::Vector3 ScaleSnap{0.1f, 0.1f, 0.1f};
};

class TransformGizmoService final
{
public:
    void SetEnabled(bool enabled) noexcept;

    [[nodiscard]] bool IsEnabled() const noexcept;

    void ClearSelection() noexcept;

    void SelectEntity(EntityId entityId) noexcept;

    void TrySelectFromGBufferPick(Scene &scene, const GBufferPickResult &pick) noexcept;

    void DrawInspectorAndGizmo(
        Scene &scene,
        const Camera &camera,
        float aspectRatio,
        int viewportWidth,
        int viewportHeight
    );

    [[nodiscard]] bool IsUsingGizmo() const noexcept;

    [[nodiscard]] bool WantsInputCapture() const noexcept;

    [[nodiscard]] TransformGizmoState const &GetState() const noexcept;

    [[nodiscard]] GBufferPickResult const &GetLastPickDebug() const noexcept;

private:
    void SyncEditBuffersFromTransform(struct TransformComponent &transform) noexcept;

    void ApplyEditBuffersToTransform(struct TransformComponent &transform) noexcept;

    void ApplyObjectMatrixColumnMajorToTransform(struct TransformComponent &transform) noexcept;

    TransformGizmoState m_state{};
    GBufferPickResult m_lastPickDebug{};
    EntityId m_lastSyncedEntityId{0u};
    float m_editTranslation[3]{};
    float m_editRotationDegrees[3]{};
    float m_editScale[3]{};
    float m_objectMatrixColumnMajor[16]{};
};

#endif
