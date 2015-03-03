#define MAX_LIGHTS 64
#define LIGHT_POINT 0
#define LIGHT_DIRECTIONAL 1
#define LIGHT_SPOT 2

cbuffer Global : register(b0)
{
    float Time;
    float4x4 View;
    float4x4 Projection;
    float4 EyePosition;
}

struct Attributes
{
    float4 Emissive;
    float4 Diffuse;
    float4 Ambient;
    float4 Specular;
    float SpecularIntensity;
    float Reflectivity;
    float NormalScale;
};

cbuffer PerObject : register(b1)
{
    float4x4 World;
    float4x4 InvWorld;
    float4 AnimationCoords;

    Attributes Material;
}

struct VOut
{
    float4 position : SV_POSITION;
    float4 colour : COLOUR;
    float2 texcoord : TEXCOORD0;
    float3 normal : TEXCOORD2;
};

VOut VS(float4 position : POSITION, float4 colour : COLOUR, float2 texcoord : TEXCOORD0, float3 normal : NORMAL)
{
    VOut output;
    output.position = mul(position, World);
    output.position = mul(output.position, Projection);
    output.normal = mul(normal, (float3x3) InvWorld);
    output.texcoord = texcoord;
    output.colour = colour;
    return output;
}

Texture2D TexDiffuse : register(t1);

SamplerState Sampler;

float4 PS(VOut input) : SV_TARGET
{
    float4 diffuse_map = TexDiffuse.Sample(Sampler, input.texcoord);
    diffuse_map *= Material.Diffuse;
    
    diffuse_map.a *= Material.Diffuse.a;
    return diffuse_map;
}