#define MAX_SCENE_LIGHTS 8
#define MAX_SHADOW_CASCADES 4

cbuffer CameraConstants : register(b0)
{
    matrix View;
    matrix Projection;
    matrix ViewProjection;
    float4 CameraWorldPosition;
};

cbuffer ObjectConstants : register(b1)
{
    matrix World;
    matrix WorldInverseTranspose;
};

cbuffer MaterialConstants : register(b2)
{
    float4 BaseColor;
    float4 SpecularColorAndPower;
    float4 EmissiveAndAmbient;
};

struct LightGpu
{
    float4 Position;
    float4 Direction;
    float4 Color;
    float4 Parameters;
    float4 Extension;
};

cbuffer LightsConstants : register(b3)
{
    uint TotalLightCount;
    uint3 _pad0;
    LightGpu Lights[MAX_SCENE_LIGHTS];
};

cbuffer ShadowCascadeConstants : register(b4)
{
    matrix LightViewProjection[MAX_SHADOW_CASCADES];
    float4 CascadeSplits;
    uint CascadeCount;
    uint3 _ShadowCascadePadding;
};

cbuffer ShadowSamplingConstants : register(b5)
{
    float4 DepthBiasAndPcfKernel;
    float2 InvShadowMapTexelSize;
    float2 _ShadowSamplingPad0;
    uint ShadowEnabled;
    uint ShadowedDirectionalLightGpuIndex;
    uint ShadowMapCascadeCount;
    uint _ShadowSamplingPad1;
};

Texture2D ShadowMapDepth : register(t0);
SamplerComparisonState ShadowMapSampler : register(s0);

struct VSInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float4 Color : COLOR;
};

struct PSInput
{
    float4 Position : SV_POSITION;
    float3 WorldNormal : NORMAL;
    float3 WorldPosition : TEXCOORD0;
    float4 Color : COLOR;
};

PSInput VSMain(VSInput input)
{
    PSInput output;

    float4 worldPosition = mul(float4(input.Position, 1.0f), World);
    output.WorldPosition = worldPosition.xyz;

    float3x3 worldInverseTranspose = (float3x3)WorldInverseTranspose;
    output.WorldNormal = normalize(mul(input.Normal, worldInverseTranspose));

    output.Position = mul(worldPosition, ViewProjection);
    output.Color = input.Color;

    return output;
}

struct VSInputNoVertexColor
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
};

PSInput VSMainNoVertexColor(VSInputNoVertexColor input)
{
    PSInput output;

    float4 worldPosition = mul(float4(input.Position, 1.0f), World);
    output.WorldPosition = worldPosition.xyz;

    float3x3 worldInverseTranspose = (float3x3)WorldInverseTranspose;
    output.WorldNormal = normalize(mul(input.Normal, worldInverseTranspose));

    output.Position = mul(worldPosition, ViewProjection);
    output.Color = float4(1.0f, 1.0f, 1.0f, 1.0f);

    return output;
}

static const float kDirectionalKind = 0.0f;
static const float kPointKind = 1.0f;
static const float kSpotKind = 2.0f;

void EvaluateLight(
    LightGpu light,
    float3 worldPosition,
    float3 worldNormal,
    float3 viewDirection,
    out float3 diffuseContribution,
    out float3 specularContribution
)
{
    diffuseContribution = float3(0.0f, 0.0f, 0.0f);
    specularContribution = float3(0.0f, 0.0f, 0.0f);

    const float3 lightColor = light.Color.rgb;
    float3 lightVector = float3(0.0f, 0.0f, 0.0f);
    float attenuation = 1.0f;

    const float kind = light.Parameters.x;

    if (abs(kind - kDirectionalKind) < 0.01f)
    {
        lightVector = normalize(light.Direction.xyz);
    }
    else if (abs(kind - kPointKind) < 0.01f)
    {
        const float3 toLight = light.Position.xyz - worldPosition;
        const float distanceToLight = length(toLight);
        lightVector = toLight / max(distanceToLight, 0.0001f);
        const float range = light.Parameters.y;
        attenuation = saturate(1.0f - distanceToLight / max(range, 0.0001f));
        attenuation = attenuation * attenuation;
    }
    else if (abs(kind - kSpotKind) < 0.01f)
    {
        const float3 toLight = light.Position.xyz - worldPosition;
        const float distanceToLight = length(toLight);
        lightVector = toLight / max(distanceToLight, 0.0001f);
        const float range = light.Parameters.y;
        attenuation = saturate(1.0f - distanceToLight / max(range, 0.0001f));
        attenuation = attenuation * attenuation;

        const float3 axis = normalize(light.Direction.xyz);
        const float3 fromLightToPoint = normalize(worldPosition - light.Position.xyz);
        const float cosAngle = dot(axis, fromLightToPoint);
        const float cosInner = light.Parameters.z;
        const float cosOuter = light.Parameters.w;
        const float spotFactor = saturate((cosAngle - cosOuter) / max(cosInner - cosOuter, 0.0001f));
        attenuation *= spotFactor;
    }

    const float normalDotLight = saturate(dot(worldNormal, lightVector));
    diffuseContribution = lightColor * normalDotLight * attenuation;

    const float3 reflection = reflect(-lightVector, worldNormal);
    const float specularPower = max(SpecularColorAndPower.w, 1.0f);
    const float specularTerm = pow(saturate(dot(reflection, viewDirection)), specularPower);
    specularContribution = SpecularColorAndPower.rgb * lightColor * specularTerm * attenuation;
}

float SampleShadowPcf(float2 shadowUv, float depthReference, int pcfRadius)
{
    float accumulated = 0.0f;
    int sampleCount = 0;
    for (int offsetV = -pcfRadius; offsetV <= pcfRadius; ++offsetV)
    {
        for (int offsetU = -pcfRadius; offsetU <= pcfRadius; ++offsetU)
        {
            const float2 sampleUv = shadowUv + float2(offsetU, offsetV) * InvShadowMapTexelSize;
            accumulated += ShadowMapDepth.SampleCmpLevelZero(ShadowMapSampler, sampleUv, depthReference);
            sampleCount++;
        }
    }
    return accumulated / max(sampleCount, 1);
}

float DirectionalShadowAttenuation(float3 worldPosition, float3 worldNormal)
{
    if (ShadowEnabled == 0u || ShadowMapCascadeCount == 0u)
    {
        return 1.0f;
    }
    if (ShadowedDirectionalLightGpuIndex >= TotalLightCount)
    {
        return 1.0f;
    }

    const float3 towardLight = normalize(Lights[ShadowedDirectionalLightGpuIndex].Direction.xyz);
    const float3 biasedWorldPosition = worldPosition + worldNormal * DepthBiasAndPcfKernel.z;

    const float4 lightClip = mul(float4(biasedWorldPosition, 1.0f), LightViewProjection[0]);
    const float invW = 1.0f / max(abs(lightClip.w), 1.0e-5f);
    const float3 ndc = lightClip.xyz * invW;
    const float2 shadowUv = float2(ndc.x * 0.5f + 0.5f, -ndc.y * 0.5f + 0.5f);

    if (shadowUv.x < 0.0f || shadowUv.x > 1.0f || shadowUv.y < 0.0f || shadowUv.y > 1.0f)
    {
        return 1.0f;
    }

    const float ndcZ = ndc.z;
    const float slopeTerm = saturate(1.0f - dot(worldNormal, towardLight));
    const float depthReference = ndcZ
        - DepthBiasAndPcfKernel.x
        - DepthBiasAndPcfKernel.y * slopeTerm;

    const int pcfRadius = clamp((int)DepthBiasAndPcfKernel.w, 1, 4);
    return SampleShadowPcf(shadowUv, depthReference, pcfRadius);
}

float4 PSMain(PSInput input) : SV_TARGET
{
    const float3 worldNormal = normalize(input.WorldNormal);
    const float3 viewDirection = normalize(CameraWorldPosition.xyz - input.WorldPosition);

    const float directionalShadow = DirectionalShadowAttenuation(input.WorldPosition, worldNormal);

    float3 diffuseAccumulation = float3(0.0f, 0.0f, 0.0f);
    float3 specularAccumulation = float3(0.0f, 0.0f, 0.0f);

    const uint lightCount = min(TotalLightCount, MAX_SCENE_LIGHTS);

    for (uint lightIndex = 0u; lightIndex < lightCount; ++lightIndex)
    {
        float3 diffuseContribution = float3(0.0f, 0.0f, 0.0f);
        float3 specularContribution = float3(0.0f, 0.0f, 0.0f);
        EvaluateLight(
            Lights[lightIndex],
            input.WorldPosition,
            worldNormal,
            viewDirection,
            diffuseContribution,
            specularContribution
        );

        float shadowFactor = 1.0f;
        if (ShadowEnabled != 0u
            && lightIndex == ShadowedDirectionalLightGpuIndex
            && abs(Lights[lightIndex].Parameters.x - kDirectionalKind) < 0.01f)
        {
            shadowFactor = directionalShadow;
        }

        diffuseAccumulation += diffuseContribution * shadowFactor;
        specularAccumulation += specularContribution * shadowFactor;
    }

    const float3 baseSample = input.Color.rgb * BaseColor.rgb;
    const float ambientScale = EmissiveAndAmbient.w;
    const float3 ambientTerm = baseSample * ambientScale;
    const float3 emissiveTerm = EmissiveAndAmbient.xyz;

    const float3 litColor = ambientTerm + emissiveTerm + diffuseAccumulation * baseSample + specularAccumulation;

    return float4(litColor, BaseColor.a * input.Color.a);
}
