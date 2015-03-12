cbuffer Global : register(b0)
{
	float Time;
	float4x4 View;
	float4x4 Projection;
}

cbuffer Uniforms : register(b3)
{

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
	output.position = position;
	output.normal = normal;
	output.texcoord = texcoord;
	output.colour = colour;
	return output;
}

Texture2D TexColour : register(t0);
Texture2D TexNormal : register(t1);
Texture2D TexDepth : register(t2);
SamplerState Sampler;

float4 PS(VOut input) : SV_TARGET
{
	float4 diffuse = TexColour.Sample(Sampler, input.texcoord);
	float4 normal = TexNormal.Sample(Sampler, input.texcoord);
	float4 depth = TexDepth.Sample(Sampler, input.texcoord);

	normal = normal * 2.0f - 1.0f;
	float3 lightDir = float3(0, -1, -1);
	diffuse.rgb *= saturate(dot(-lightDir, normal.rgb));
	return diffuse;
}