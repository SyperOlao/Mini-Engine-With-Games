//
// Created by SyperOlao on 19.03.2026.
//

#ifndef PINGPONG_SOLARSYSTEMSCENE_H
#define PINGPONG_SOLARSYSTEMSCENE_H

#include <memory>
#include <vector>
#include "Game/SolarSystem/Entities/OrbitalBody.h"

class SolarSystemScene final {
public:
    void Initialize();

    void Update(float deltaTime) const;

    [[nodiscard]] const std::vector<std::shared_ptr<OrbitalBody> > &GetRoots() const noexcept;

private:
    void CreateDemoSystem();

private:
    std::vector<std::shared_ptr<OrbitalBody> > m_roots;
};

#endif //PINGPONG_SOLARSYSTEMSCENE_H
