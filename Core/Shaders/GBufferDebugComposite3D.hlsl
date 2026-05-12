Texture2D AlbedoAmbientTexture : register(t0);
Texture2D NormalReceiveLightingTexture : register(t1);
Texture2D SpecularPowerTexture : register(t2);
Texture2D EmissiveTexture : register(t3);
Texture2D SceneDepthTexture : register(t4);
Texture2D<uint> ObjectIdTexture : register(t5);
SamplerState GBufferDebugSampler : register(s0);

struct VSOutput
{
    float4 Position : SV_POSITION;
    float2 Uv : TEXCOORD0;
};

VSOutput VSMain(uint vertexId : SV_VertexID)
{
    VSOutput output;
    const float2 positions[3] =
    {
        float2(-1.0f, -1.0f),
        float2(-1.0f, 3.0f),
        float2(3.0f, -1.0f)
    };
    const float2 uvs[3] =
    {
        float2(0.0f, 1.0f),
        float2(0.0f, -1.0f),
        float2(2.0f, 1.0f)
    };

    output.Position = float4(positions[vertexId], 0.0f, 1.0f);
    output.Uv = uvs[vertexId];
    return output;
}

float3 TileBorderColor(float2 localUv)
{
    const float border = 0.012f;
    const bool isBorder =
        localUv.x < border || localUv.y < border ||
        localUv.x > 1.0f - border || localUv.y > 1.0f - border;
    return isBorder ? float3(0.08f, 0.09f, 0.12f) : float3(-1.0f, -1.0f, -1.0f);
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    const float2 grid = float2(3.0f, 2.0f);
    const float2 scaledUv = saturate(input.Uv) * grid;
    const uint column = min((uint)floor(scaledUv.x), 2u);
    const uint row = min((uint)floor(scaledUv.y), 1u);
    const uint tileIndex = row * 3u + column;
    const float2 tileUv = frac(scaledUv);

    const float3 borderColor = TileBorderColor(tileUv);
    if (borderColor.x >= 0.0f)
    {
        return float4(borderColor, 1.0f);
    }

    if (tileIndex == 0u)
    {
        return float4(AlbedoAmbientTexture.Sample(GBufferDebugSampler, tileUv).rgb, 1.0f);
    }
    if (tileIndex == 1u)
    {
        return float4(NormalReceiveLightingTexture.Sample(GBufferDebugSampler, tileUv).rgb, 1.0f);
    }
    if (tileIndex == 2u)
    {
        return float4(SpecularPowerTexture.Sample(GBufferDebugSampler, tileUv).rgb, 1.0f);
    }
    if (tileIndex == 3u)
    {
        return float4(EmissiveTexture.Sample(GBufferDebugSampler, tileUv).rgb, 1.0f);
    }
    if (tileIndex == 4u)
    {
        const float sceneDepth = SceneDepthTexture.Sample(GBufferDebugSampler, tileUv).r;
        const float depthPreview = sceneDepth >= 0.999999f ? 0.0f : pow(saturate(1.0f - sceneDepth), 0.18f);
        return float4(depthPreview.xxx, 1.0f);
    }
    if (tileIndex == 5u)
    {
        uint objectIdTexWidth;
        uint objectIdTexHeight;
        ObjectIdTexture.GetDimensions(objectIdTexWidth, objectIdTexHeight);
        const int2 objectIdPixel = int2(
            clamp(int(floor(tileUv.x * float(objectIdTexWidth))), 0, int(objectIdTexWidth) - 1),
            clamp(int(floor(tileUv.y * float(objectIdTexHeight))), 0, int(objectIdTexHeight) - 1));
        const uint objectIdValue = ObjectIdTexture.Load(int3(objectIdPixel, 0));
        if (objectIdValue == 0u)
        {
            return float4(0.0f, 0.0f, 0.0f, 1.0f);
        }
        const float3 pseudoColor = frac(float3(
            float(objectIdValue) * 0.1031f,
            float(objectIdValue) * 0.11369f,
            float(objectIdValue) * 0.13787f));
        return float4(pseudoColor, 1.0f);
    }

    return float4(0.015f, 0.018f, 0.024f, 1.0f);
}
