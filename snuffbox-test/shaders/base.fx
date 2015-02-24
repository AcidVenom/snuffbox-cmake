cbuffer Global : register(b0)
{
	float Time;
	float4x4 View;
	float4x4 Projection;
}

cbuffer PerObject : register(b1)
{
	float4x4 World;
	float4x4 InvWorld;
	float Alpha;
	float3 Blend;
	float4 AnimationCoords;
}

struct VOut
{
	float4 position : SV_POSITION;
	float4 colour : COLOUR;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL;
};

VOut VS(float4 position : POSITION, float4 colour : COLOUR, float2 texcoord : TEXCOORD0, float3 normal : NORMAL)
{
	VOut output;
	output.position = mul(position, World);
	output.position = mul(output.position, View);
	output.position = mul(output.position, Projection);
	output.normal = normal;
	output.texcoord = texcoord;
	output.colour = colour;
	return output;
}

Texture2D Tex2D;
SamplerState Sampler;

float4 PS(VOut input) : SV_TARGET
{
	input.colour.rgb *= Blend;
	input.colour.a *= Alpha;
	return input.colour;
}