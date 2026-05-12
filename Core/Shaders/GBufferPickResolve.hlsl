Texture2D<float4> GBufferNormalTexture : register(t0);
Texture2D<uint> GBufferObjectIdTexture : register(t1);
Texture2D<float> GBufferDepthTexture : register(t2);

cbuffer GBufferPickConstants : register(b0)
{
    row_major float4x4 InverseProjection;
    row_major float4x4 InverseView;
    int2 PickPixel;
    int2 GBufferTextureDimensions;
};

RWBuffer<uint> PickPackedUintBuffer : register(u0);

[numthreads(1, 1, 1)]
void PickMain(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    const int2 dimensions = GBufferTextureDimensions;
    const int2 pick = PickPixel;

    if (pick.x < 0 || pick.y < 0 || pick.x >= dimensions.x || pick.y >= dimensions.y)
    {
        PickPackedUintBuffer[0] = 0u;
        PickPackedUintBuffer[1] = 0u;
        PickPackedUintBuffer[2] = asuint(1.0f);
        PickPackedUintBuffer[3] = 0u;
        PickPackedUintBuffer[4] = 0u;
        PickPackedUintBuffer[5] = 0u;
        PickPackedUintBuffer[6] = 0u;
        PickPackedUintBuffer[7] = 0u;
        PickPackedUintBuffer[8] = 0u;
        PickPackedUintBuffer[9] = 0u;
        return;
    }

    const uint objectId = GBufferObjectIdTexture.Load(int3(pick, 0));
    const float deviceDepth = GBufferDepthTexture.Load(int3(pick, 0)).r;

    PickPackedUintBuffer[0] = objectId;
    PickPackedUintBuffer[2] = asuint(deviceDepth);

    if (objectId == 0u || deviceDepth >= 1.0f)
    {
        PickPackedUintBuffer[1] = 0u;
        PickPackedUintBuffer[3] = 0u;
        PickPackedUintBuffer[4] = 0u;
        PickPackedUintBuffer[5] = 0u;
        PickPackedUintBuffer[6] = 0u;
        PickPackedUintBuffer[7] = 0u;
        PickPackedUintBuffer[8] = 0u;
        PickPackedUintBuffer[9] = 0u;
        return;
    }

    const float3 encodedNormal = GBufferNormalTexture.Load(int3(pick, 0)).xyz;
    float3 worldNormal = encodedNormal * 2.0f - 1.0f;
    worldNormal = normalize(worldNormal);

    const float normalizedDeviceX =
        ((float)pick.x + 0.5f) / (float)dimensions.x * 2.0f - 1.0f;
    const float normalizedDeviceY =
        1.0f - ((float)pick.y + 0.5f) / (float)dimensions.y * 2.0f;

    const float4 clipPosition = float4(normalizedDeviceX, normalizedDeviceY, deviceDepth, 1.0f);
    float4 viewPosition = mul(clipPosition, InverseProjection);
    viewPosition.xyz /= max(abs(viewPosition.w), 1e-5f);

    float4 worldPosition4 = mul(float4(viewPosition.x, viewPosition.y, viewPosition.z, 1.0f), InverseView);
    worldPosition4.xyz /= max(abs(worldPosition4.w), 1e-5f);

    PickPackedUintBuffer[1] = 1u;
    PickPackedUintBuffer[3] = asuint(worldNormal.x);
    PickPackedUintBuffer[4] = asuint(worldNormal.y);
    PickPackedUintBuffer[5] = asuint(worldNormal.z);
    PickPackedUintBuffer[6] = asuint(worldPosition4.x);
    PickPackedUintBuffer[7] = asuint(worldPosition4.y);
    PickPackedUintBuffer[8] = asuint(worldPosition4.z);
    PickPackedUintBuffer[9] = 0u;
}
