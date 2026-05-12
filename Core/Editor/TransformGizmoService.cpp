#include "Core/Editor/TransformGizmoService.h"

#include "Core/Gameplay/GameplayComponents.h"
#include "Core/Gameplay/Scene.h"
#include "Core/Graphics/Camera.h"

#include "imgui.h"
#include "ImGuizmo.h"

#include <algorithm>
#include <cmath>
#include <cstring>

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;

namespace
{
constexpr float kDegreesToRadians = 3.14159265358979323846f / 180.0f;

void ApplyDynamicImGuizmoVisualScale(
    const Camera &camera,
    const float *const objectMatrixColumnMajor16,
    const int viewportWidth,
    const int viewportHeight
)
{
    const Vector3 pivot(
        objectMatrixColumnMajor16[12],
        objectMatrixColumnMajor16[13],
        objectMatrixColumnMajor16[14]
    );
    float distanceWorld = Vector3::Distance(pivot, camera.GetPosition());
    distanceWorld = (std::max)(distanceWorld, 0.12f);

    const float minViewportSide = static_cast<float>((std::min)(viewportWidth, viewportHeight));
    const float resolutionScale = std::clamp(minViewportSide / 900.0f, 0.88f, 2.6f);

    constexpr float referenceDistanceWorld = 5.5f;
    const float distanceRatio = distanceWorld / referenceDistanceWorld;
    const float lengthBoost = std::clamp(std::pow(distanceRatio, 0.42f), 0.5f, 4.2f);
    const float widthBoost = std::clamp(std::sqrt(lengthBoost) * resolutionScale, 0.82f, 2.75f);

    const float clipSpace = std::clamp(0.27f * lengthBoost * resolutionScale, 0.22f, 0.72f);
    ImGuizmo::SetGizmoSizeClipSpace(clipSpace);

    ImGuizmo::Style &gizmoStyle = ImGuizmo::GetStyle();
    gizmoStyle.TranslationLineThickness = 8.5f * widthBoost;
    gizmoStyle.TranslationLineArrowSize = 22.0f * widthBoost;
    gizmoStyle.CenterCircleSize = 14.5f * widthBoost;
    gizmoStyle.ScaleLineThickness = 6.5f * widthBoost;
    gizmoStyle.ScaleLineCircleSize = 12.0f * widthBoost;
    gizmoStyle.RotationLineThickness = 5.5f * widthBoost;
    gizmoStyle.RotationOuterLineThickness = 6.5f * widthBoost;
    gizmoStyle.HatchedAxisLineThickness = 10.0f * widthBoost;
    gizmoStyle.Colors[ImGuizmo::DIRECTION_X] = ImVec4(0.92f, 0.22f, 0.20f, 1.0f);
    gizmoStyle.Colors[ImGuizmo::DIRECTION_Y] = ImVec4(0.38f, 0.90f, 0.32f, 1.0f);
    gizmoStyle.Colors[ImGuizmo::DIRECTION_Z] = ImVec4(0.30f, 0.58f, 1.0f, 1.0f);
    gizmoStyle.Colors[ImGuizmo::PLANE_X] = ImVec4(0.92f, 0.22f, 0.20f, 0.48f);
    gizmoStyle.Colors[ImGuizmo::PLANE_Y] = ImVec4(0.38f, 0.90f, 0.32f, 0.48f);
    gizmoStyle.Colors[ImGuizmo::PLANE_Z] = ImVec4(0.30f, 0.58f, 1.0f, 0.48f);
}

void MatrixRowMajorToImGuizmoColumnMajor(const Matrix &rowMajor, float *const outColumnMajor16)
{
    const Matrix transposed = rowMajor.Transpose();
    std::memcpy(outColumnMajor16, &transposed, sizeof(float) * 16);
}

ImGuizmo::OPERATION ToImGuizmoOperation(const TransformGizmoOperation operation)
{
    switch (operation)
    {
        case TransformGizmoOperation::Translate:
            return ImGuizmo::TRANSLATE;
        case TransformGizmoOperation::Rotate:
            return ImGuizmo::ROTATE;
        case TransformGizmoOperation::Scale:
            return ImGuizmo::SCALE;
    }
    return ImGuizmo::TRANSLATE;
}

ImGuizmo::MODE ToImGuizmoMode(const TransformGizmoSpace space)
{
    return space == TransformGizmoSpace::Local ? ImGuizmo::LOCAL : ImGuizmo::WORLD;
}
}

void TransformGizmoService::SetEnabled(const bool enabled) noexcept
{
    m_state.Enabled = enabled;
    if (!enabled)
    {
        ClearSelection();
    }
}

bool TransformGizmoService::IsEnabled() const noexcept
{
    return m_state.Enabled;
}

void TransformGizmoService::ClearSelection() noexcept
{
    m_state.HasSelection = false;
    m_state.SelectedEntityId = 0u;
    m_lastSyncedEntityId = 0u;
}

void TransformGizmoService::SelectEntity(const EntityId entityId) noexcept
{
    m_state.SelectedEntityId = entityId;
    m_state.HasSelection = entityId != 0u;
    m_lastSyncedEntityId = 0u;
}

void TransformGizmoService::TrySelectFromGBufferPick(Scene &scene, const GBufferPickResult &pick) noexcept
{
    m_lastPickDebug = pick;

    if (!pick.Hit || pick.ObjectId == 0u)
    {
        ClearSelection();
        return;
    }

    const EntityId selectedId = static_cast<EntityId>(pick.ObjectId);
    if (!scene.IsEntityAlive(selectedId))
    {
        ClearSelection();
        return;
    }

    if (scene.TryGetTransformComponent(selectedId) == nullptr)
    {
        ClearSelection();
        return;
    }

    m_state.HasSelection = true;
    m_state.SelectedEntityId = selectedId;
    m_lastSyncedEntityId = 0u;
}

void TransformGizmoService::SyncEditBuffersFromTransform(TransformComponent &transform) noexcept
{
    MatrixRowMajorToImGuizmoColumnMajor(transform.Local.GetWorldMatrix(), m_objectMatrixColumnMajor);
    ImGuizmo::DecomposeMatrixToComponents(
        m_objectMatrixColumnMajor,
        m_editTranslation,
        m_editRotationDegrees,
        m_editScale
    );
}

void TransformGizmoService::ApplyObjectMatrixColumnMajorToTransform(TransformComponent &transform) noexcept
{
    ImGuizmo::DecomposeMatrixToComponents(
        m_objectMatrixColumnMajor,
        m_editTranslation,
        m_editRotationDegrees,
        m_editScale
    );
    transform.Local.Position = Vector3(m_editTranslation[0], m_editTranslation[1], m_editTranslation[2]);
    transform.Local.Scale = Vector3(m_editScale[0], m_editScale[1], m_editScale[2]);
    transform.Local.RotationEulerRad = Vector3(
        m_editRotationDegrees[0] * kDegreesToRadians,
        m_editRotationDegrees[1] * kDegreesToRadians,
        m_editRotationDegrees[2] * kDegreesToRadians
    );
    transform.Local.UseQuaternionRotation = false;
    transform.WorldMatrix = transform.Local.GetWorldMatrix();
}

void TransformGizmoService::ApplyEditBuffersToTransform(TransformComponent &transform) noexcept
{
    ImGuizmo::RecomposeMatrixFromComponents(
        m_editTranslation,
        m_editRotationDegrees,
        m_editScale,
        m_objectMatrixColumnMajor
    );
    ApplyObjectMatrixColumnMajorToTransform(transform);
}

TransformGizmoState const &TransformGizmoService::GetState() const noexcept
{
    return m_state;
}

GBufferPickResult const &TransformGizmoService::GetLastPickDebug() const noexcept
{
    return m_lastPickDebug;
}

bool TransformGizmoService::IsUsingGizmo() const noexcept
{
    if (!m_state.Enabled)
    {
        return false;
    }

    if (ImGui::GetCurrentContext() == nullptr)
    {
        return false;
    }

    return ImGuizmo::IsUsing();
}

bool TransformGizmoService::WantsInputCapture() const noexcept
{
    if (!m_state.Enabled)
    {
        return false;
    }

    if (ImGui::GetCurrentContext() == nullptr)
    {
        return false;
    }

    return ImGuizmo::IsUsing() || ImGuizmo::IsOver();
}

void TransformGizmoService::DrawInspectorAndGizmo(
    Scene &scene,
    const Camera &camera,
    const float aspectRatio,
    const int viewportWidth,
    const int viewportHeight
)
{
    if (!m_state.Enabled)
    {
        return;
    }

    if (ImGui::GetCurrentContext() == nullptr)
    {
        return;
    }

    ImGuizmo::BeginFrame();

    ImGui::SetNextWindowPos(ImVec2(24.0f, 120.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(360.0f, 520.0f), ImGuiCond_FirstUseEver);
    ImGui::Begin("Matrix Inspector", nullptr, ImGuiWindowFlags_None);

    if (ImGui::RadioButton("Translate", m_state.Operation == TransformGizmoOperation::Translate))
    {
        m_state.Operation = TransformGizmoOperation::Translate;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", m_state.Operation == TransformGizmoOperation::Rotate))
    {
        m_state.Operation = TransformGizmoOperation::Rotate;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", m_state.Operation == TransformGizmoOperation::Scale))
    {
        m_state.Operation = TransformGizmoOperation::Scale;
    }

    ImGui::Separator();
    ImGui::Checkbox("Snap", &m_state.SnapEnabled);
    if (m_state.SnapEnabled)
    {
        if (m_state.Operation == TransformGizmoOperation::Translate)
        {
            ImGui::InputFloat3("Snap translate", &m_state.TranslationSnap.x);
        }
        else if (m_state.Operation == TransformGizmoOperation::Rotate)
        {
            ImGui::InputFloat("Snap rotate (deg)", &m_state.RotationSnapDegrees);
        }
        else
        {
            ImGui::InputFloat3("Snap scale", &m_state.ScaleSnap.x);
        }
    }

    if (ImGui::RadioButton("Local", m_state.Space == TransformGizmoSpace::Local))
    {
        m_state.Space = TransformGizmoSpace::Local;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("World", m_state.Space == TransformGizmoSpace::World))
    {
        m_state.Space = TransformGizmoSpace::World;
    }

    ImGui::Separator();

    if (!m_state.HasSelection)
    {
        ImGui::TextUnformatted("No entity selected. Left-click an object in the viewport.");
    }
    else
    {
        ImGui::Text("Selected entity: %llu", static_cast<unsigned long long>(m_state.SelectedEntityId));
    }

    ImGui::Separator();
    ImGui::TextUnformatted("Last pick (debug)");
    ImGui::Text("Hit: %s", m_lastPickDebug.Hit ? "true" : "false");
    ImGui::Text("ObjectId: %u", m_lastPickDebug.ObjectId);
    ImGui::Text("Pixel: %d %d", m_lastPickDebug.ScreenX, m_lastPickDebug.ScreenY);
    ImGui::Text("Depth: %.4f", static_cast<double>(m_lastPickDebug.Depth));
    ImGui::Text(
        "World pos: %.2f %.2f %.2f",
        static_cast<double>(m_lastPickDebug.WorldPosition.x),
        static_cast<double>(m_lastPickDebug.WorldPosition.y),
        static_cast<double>(m_lastPickDebug.WorldPosition.z)
    );
    ImGui::Text(
        "World nrm: %.2f %.2f %.2f",
        static_cast<double>(m_lastPickDebug.WorldNormal.x),
        static_cast<double>(m_lastPickDebug.WorldNormal.y),
        static_cast<double>(m_lastPickDebug.WorldNormal.z)
    );

    TransformComponent *transform = nullptr;
    if (m_state.HasSelection)
    {
        transform = scene.TryGetTransformComponent(m_state.SelectedEntityId);
        if (transform == nullptr)
        {
            ClearSelection();
        }
    }

    if (transform != nullptr)
    {
        const bool selectionChanged = m_state.SelectedEntityId != m_lastSyncedEntityId;
        const bool allowSyncFromEntity =
            !ImGuizmo::IsUsing()
            && !ImGui::IsAnyItemActive();

        if (selectionChanged || allowSyncFromEntity)
        {
            SyncEditBuffersFromTransform(*transform);
            m_lastSyncedEntityId = m_state.SelectedEntityId;
        }

        ImGui::Separator();
        ImGui::TextUnformatted("Attached entities with parents are not fully supported yet.");
        ImGui::TextUnformatted("TODO: apply gizmo delta in parent-local space when AttachmentComponent exists.");

        if (ImGui::InputFloat3("Tr", m_editTranslation))
        {
            ApplyEditBuffersToTransform(*transform);
        }
        if (ImGui::InputFloat3("Rt (deg)", m_editRotationDegrees))
        {
            ApplyEditBuffersToTransform(*transform);
        }
        if (ImGui::InputFloat3("Sc", m_editScale))
        {
            ApplyEditBuffersToTransform(*transform);
        }
    }

    ImGui::End();

    if (transform == nullptr || viewportWidth <= 0 || viewportHeight <= 0)
    {
        return;
    }

    float viewColumnMajor[16]{};
    float projectionColumnMajor[16]{};
    MatrixRowMajorToImGuizmoColumnMajor(camera.GetViewMatrix(), viewColumnMajor);
    MatrixRowMajorToImGuizmoColumnMajor(camera.GetProjectionMatrix(aspectRatio), projectionColumnMajor);

    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());
    ImGuizmo::SetRect(0.0f, 0.0f, static_cast<float>(viewportWidth), static_cast<float>(viewportHeight));

    ApplyDynamicImGuizmoVisualScale(camera, m_objectMatrixColumnMajor, viewportWidth, viewportHeight);

    const ImGuizmo::OPERATION imOperation = ToImGuizmoOperation(m_state.Operation);
    const ImGuizmo::MODE imMode = ToImGuizmoMode(m_state.Space);

    const float *snapPointer = nullptr;
    float rotationSnapDegrees[1]{};
    if (m_state.SnapEnabled)
    {
        if (m_state.Operation == TransformGizmoOperation::Translate)
        {
            snapPointer = &m_state.TranslationSnap.x;
        }
        else if (m_state.Operation == TransformGizmoOperation::Rotate)
        {
            rotationSnapDegrees[0] = m_state.RotationSnapDegrees;
            snapPointer = rotationSnapDegrees;
        }
        else
        {
            snapPointer = &m_state.ScaleSnap.x;
        }
    }

    ImGuizmo::Manipulate(
        viewColumnMajor,
        projectionColumnMajor,
        imOperation,
        imMode,
        m_objectMatrixColumnMajor,
        nullptr,
        snapPointer
    );

    if (ImGuizmo::IsUsing())
    {
        ApplyObjectMatrixColumnMajorToTransform(*transform);
    }
}
