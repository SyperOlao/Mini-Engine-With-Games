#ifndef PINGPONG_FORWARDPHONGGPUUPLOAD_H
#define PINGPONG_FORWARDPHONGGPUUPLOAD_H

#include "Core/Graphics/Rendering/Lighting/ShaderConstants3D.h"

#include <d3d11.h>

void ForwardPhongUploadConstantBuffers(
    ID3D11DeviceContext *context,
    const CameraGpuConstants &camera,
    const ObjectGpuConstants &object,
    const MaterialGpuConstants &material,
    const LightsGpuConstants &lights,
    ID3D11Buffer *cameraBuffer,
    ID3D11Buffer *objectBuffer,
    ID3D11Buffer *materialBuffer,
    ID3D11Buffer *lightsBuffer
);

#endif
