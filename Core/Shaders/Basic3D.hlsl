cbuffer ObjectConstants : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
};

struct VSInput
{
    float3 Position : POSITION;
    float4 Color : COLOR;
};

struct PSInput
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
};

PSInput VSMain(VSInput input)
{
    PSInput output;

    float4 worldPos = mul(float4(input.Position, 1.0f), World);
    float4 viewPos = mul(worldPos, View);
    output.Position = mul(viewPos, Projection);
    output.Color = input.Color;

    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return input.Color;
}