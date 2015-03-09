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

cbuffer Uniforms : register(b3)
{

}

struct VOut
{
	float4 position : SV_POSITION;
	float4 colour : COLOUR;
	float2 texcoord : TEXCOORD0;
	float3 normal : TEXCOORD2;
	float shift : TEXCOORD3;
};

VOut VS(float4 position : POSITION, float4 colour : COLOUR, float2 texcoord : TEXCOORD0, float3 normal : NORMAL)
{
	VOut output;
	output.shift = position.x - floor(position.x);
	output.position = mul(position, World);
	output.position = mul(output.position, Projection);
	output.normal = mul(normal, (float3x3)InvWorld);
	output.texcoord = texcoord;
	output.colour = colour;
	return output;
}

Texture2D TexDiffuse : register(t1);

SamplerState Sampler;

float4 PS(VOut input) : SV_TARGET
{
	float shift = input.shift;
	float3 font_atlas_vector = float3(1.0 / 2048, 1.0 / 2048, 4);

	float2 coords = input.texcoord;

	float4 current = TexDiffuse.Sample(Sampler, coords);
	float4 previous = TexDiffuse.Sample(Sampler, coords + float2(-1, 0) * font_atlas_vector.xy);
	float4 next = TexDiffuse.Sample(Sampler, coords + float2(1, 0) * font_atlas_vector.xy);

	float r = current.r;
	float g = current.g;
	float b = current.b;

	if (shift <= 0.333)
	{
		float z = shift / 0.333;
		r = lerp(current.r, previous.b, z);
		g = lerp(current.g, current.r, z);
		b = lerp(current.b, current.g, z);
	}
	else if (shift <= 0.666)
	{
		float z = (shift - 0.33) / 0.333;
		r = lerp(previous.b, previous.g, z);
		g = lerp(current.r, previous.b, z);
		b = lerp(current.g, current.r, z);
	}
	else if (shift < 1.0)
	{
		float z = (shift - 0.66) / 0.334;
		r = lerp(previous.g, previous.r, z);
		g = lerp(previous.b, previous.g, z);
		b = lerp(current.r, previous.b, z);
	}

	float t = max(max(r, g), b);
	float4 colour = float4(1, 1, 1, (r + g + b) / 3.0);

	float alpha = colour.a * input.colour.a * Alpha;
	return float4(colour.rgb * input.colour.rgb * Blend, alpha);
}