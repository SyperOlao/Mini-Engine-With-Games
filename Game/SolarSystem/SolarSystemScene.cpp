//
// Created by SyperOlao on 19.03.2026.
//

#include "SolarSystemScene.h"

#include <array>
#include <DirectXMath.h>

using namespace DirectX::SimpleMath;

void SolarSystemScene::Initialize() {
    CreateDemoSystem();
}

void SolarSystemScene::Update(const float deltaTime) const {
    for (const auto &root: m_roots) {
        root->Update(deltaTime);
        root->UpdateWorldMatrix(Matrix::Identity);
    }
}

const std::vector<std::shared_ptr<OrbitalBody> > &SolarSystemScene::GetRoots() const noexcept {
    return m_roots;
}

void SolarSystemScene::CreateDemoSystem() {
   m_roots.clear();

    const auto sun = std::make_shared<OrbitalBody>();
    sun->MeshType = BodyMeshType::Sphere;
    sun->Scale = Vector3(3.5f, 3.5f, 3.5f);
    sun->SelfRotationSpeed = 0.20f;

    struct PlanetSetup final {
        BodyMeshType meshType;
        Vector3 scale;
        OrbitalParams orbit;
        float selfRotationSpeed;
        BodyMeshType moonMeshType;
        Vector3 moonScale;
        OrbitalParams moonOrbit;
        float moonSelfRotationSpeed;
    };

   constexpr std::array<PlanetSetup, 5> planets{{
        {
            BodyMeshType::Sphere,
            Vector3(0.9f, 0.9f, 0.9f),
            {  8.0f, 0.02f,  5.0f, 1.20f, 0.0f },
            1.8f,
            BodyMeshType::Box,
            Vector3(0.22f, 0.22f, 0.22f),
            { 1.7f, 0.01f, 10.0f, 3.8f, 0.6f },
            2.4f
        },
        {
            BodyMeshType::Box,
            Vector3(1.1f, 1.1f, 1.1f),
            { 13.0f, 0.05f,  8.0f, 0.85f, 1.0f },
            1.2f,
            BodyMeshType::Sphere,
            Vector3(0.30f, 0.30f, 0.30f),
            { 2.2f, 0.03f, 16.0f, 2.7f, 0.4f },
            2.0f
        },
        {
            BodyMeshType::Sphere,
            Vector3(1.35f, 1.35f, 1.35f),
            { 19.0f, 0.08f, 12.0f, 0.60f, 1.7f },
            0.9f,
            BodyMeshType::Box,
            Vector3(0.26f, 0.26f, 0.26f),
            { 2.8f, 0.02f, 22.0f, 2.1f, 1.1f },
            1.7f
        },
        {
            BodyMeshType::Box,
            Vector3(0.75f, 0.75f, 0.75f),
            { 26.0f, 0.04f,  4.0f, 0.42f, 2.2f },
            1.5f,
            BodyMeshType::Sphere,
            Vector3(0.20f, 0.20f, 0.20f),
            { 1.6f, 0.00f, 14.0f, 4.5f, 0.2f },
            2.8f
        },
        {
            BodyMeshType::Sphere,
            Vector3(1.7f, 1.7f, 1.7f),
            { 34.0f, 0.10f, 18.0f, 0.30f, 0.8f },
            0.6f,
            BodyMeshType::Box,
            Vector3(0.38f, 0.38f, 0.38f),
            { 3.4f, 0.04f, 12.0f, 1.6f, 1.3f },
            1.4f
        }
    }};

    for (const auto&[meshType, scale, orbit, selfRotationSpeed, moonMeshType, moonScale, moonOrbit, moonSelfRotationSpeed] : planets) {
        const auto planet = std::make_shared<OrbitalBody>();
        planet->MeshType = meshType;
        planet->Scale = scale;
        planet->HasOrbit = true;
        planet->Orbit = orbit;
        planet->SelfRotationSpeed = selfRotationSpeed;

        const auto moon = std::make_shared<OrbitalBody>();
        moon->MeshType = moonMeshType;
        moon->Scale = moonScale;
        moon->HasOrbit = true;
        moon->Orbit = moonOrbit;
        moon->SelfRotationSpeed = moonSelfRotationSpeed;

        planet->AddChild(moon);
        sun->AddChild(planet);
    }

    m_roots.push_back(sun);
}
