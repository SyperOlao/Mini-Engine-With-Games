struct VSInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float3 Normal : TEXCOORD0;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    output.Position = float4(input.Position, 1.0f);
    output.Normal = input.Normal * 0.5f + 0.5f;
    return output;
}

struct GBufferOutput
{
    float4 AlbedoOcclusion : SV_Target0;
    float4 NormalTarget : SV_Target1;
    float4 MaterialTarget : SV_Target2;
};

GBufferOutput PSMain(VSOutput input)
{
    GBufferOutput output;
    output.AlbedoOcclusion = float4(1.0f, 1.0f, 1.0f, 1.0f);
    output.NormalTarget = float4(normalize(input.Normal), 1.0f);
    output.MaterialTarget = float4(0.0f, 0.4f, 0.0f, 1.0f);
    return output;
}
