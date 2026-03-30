Texture2D AlbedoTexture : register(t0);
SamplerState AlbedoSampler : register(s0);

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

float4 PSMain(VSOutput input) : SV_TARGET
{
    return AlbedoTexture.Sample(AlbedoSampler, input.Uv);
}
