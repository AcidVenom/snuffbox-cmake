cbuffer Global : register(b0)
{
	float Time;
	float4x4 View;
	float4x4 Projection;
}

struct Attributes
{
	float4 Emissive;
	float4 Diffuse;
	float4 Ambient;
	float4 Specular;
	float SpecularIntensity;
};

cbuffer PerObject : register(b1)
{
	float4x4 World;
	float4x4 InvWorld;
	float Alpha;
	float3 Blend;
	float4 AnimationCoords;
	
	Attributes Material;
}

struct VOut
{
	float4 position : SV_POSITION;
	float4 colour : COLOUR;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL;
	float3 worldPos : POSITION;
};

VOut VS(float4 position : POSITION, float4 colour : COLOUR, float2 texcoord : TEXCOORD0, float3 normal : NORMAL)
{
	VOut output;
	output.position = mul(position, World);
	output.worldPos = output.position.xyz;
	output.position = mul(output.position, View);
	output.position = mul(output.position, Projection);
	output.normal = normalize(mul(float4(normal, 0), InvWorld).xyz);
	output.texcoord = texcoord;
	output.colour = colour;
	return output;
}

Texture2D Tex2D[3];
SamplerState Sampler;

float4 PS(VOut input) : SV_TARGET
{
 	float4 base = Tex2D[0].Sample(Sampler, input.texcoord);
    return Material.Ambient;
}