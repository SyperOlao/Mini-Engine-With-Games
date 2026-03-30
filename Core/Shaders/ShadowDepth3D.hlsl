cbuffer ShadowDepthObjectConstants : register(b0)
{
    matrix World;
    matrix LightViewProjection;
};

struct VSInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
};

float4 VSMain(VSInput input) : SV_POSITION
{
    float4 worldPosition = mul(float4(input.Position, 1.0f), World);
    return mul(worldPosition, LightViewProjection);
}
