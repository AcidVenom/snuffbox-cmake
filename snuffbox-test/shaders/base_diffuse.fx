cbuffer Global : register(b0)
{
	float Time;
	float4x4 View;
	float4x4 Projection;
	float4 EyePosition;
}

cbuffer PerObject : register(b1)
{
	float4x4 World;
	float4x4 InvWorld;
	float4 AnimationCoords;
	float3 Blend;
	float Alpha;
}

cbuffer Uniforms : register(b2)
{

}

struct VOut
{
	float4 position : SV_POSITION;
	float4 colour : COLOUR;
	float2 texcoord : TEXCOORD0;
	float3 normal : TEXCOORD2;
	float3 tangent : TEXCOORD3;
	float3 bitangent : TEXCOORD4;
};

VOut VS(float4 position : POSITION, float4 colour : COLOUR, float2 texcoord : TEXCOORD0, float3 normal : NORMAL, float3 tangent : TANGENT, float3 bitangent : BITANGENT)
{
	VOut output;
	output.position = mul(position, World);
	output.position = mul(position, View);
	output.position = mul(output.position, Projection);
	output.normal = mul(normal, (float3x3)InvWorld);
	output.texcoord = texcoord;
	output.colour = colour;
	output.normal = normalize(mul(normal, (float3x3)InvWorld));
	output.tangent = normalize(mul(tangent, (float3x3)InvWorld));
	output.bitangent = normalize(mul(bitangent, (float3x3)InvWorld));
	return output;
}

Texture2D TexDiffuse : register(t1);
SamplerState Sampler;

float4 PS(VOut input) : SV_TARGET
{
	float x = (input.texcoord.x * AnimationCoords.z) + AnimationCoords.x;
	float y = (input.texcoord.y * AnimationCoords.w) + AnimationCoords.y;
	float2 coords = float2(x, y);
	float4 diffuse = TexDiffuse.Sample(Sampler, coords);
	diffuse.rgb *= input.colour.rgb * Blend;
	diffuse.a *= Alpha;
	return diffuse;
}