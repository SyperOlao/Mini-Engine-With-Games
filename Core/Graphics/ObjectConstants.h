//
// Created by SyperOlao on 19.03.2026.
//

#ifndef PINGPONG_OBJECTCONSTANTS_H
#define PINGPONG_OBJECTCONSTANTS_H

#include <SimpleMath.h>

struct ObjectConstants final
{
    DirectX::SimpleMath::Matrix World;
    DirectX::SimpleMath::Matrix View;
    DirectX::SimpleMath::Matrix Projection;
};


#endif //PINGPONG_OBJECTCONSTANTS_H