#ifndef PINGPONG_KATAMARICAMERACONTROLLER_H
#define PINGPONG_KATAMARICAMERACONTROLLER_H

#include <SimpleMath.h>

class FollowCamera;

class KatamariCameraController final
{
public:
    void Initialize(FollowCamera &followCamera) noexcept;

    void Update(
        float deltaTime,
        const DirectX::SimpleMath::Vector3 &ballWorldPosition,
        const DirectX::SimpleMath::Vector3 &ballVelocityWorld,
        float ballRadiusWorld
    );

private:
    FollowCamera *m_followCamera{nullptr};
};

#endif
