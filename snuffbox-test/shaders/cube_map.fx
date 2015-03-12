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

Texture2D Tex2D;
SamplerState Sampler;

float4 PS(VOut input) : SV_TARGET
{
	return Tex2D.Sample(Sampler, input.texcoord);
}