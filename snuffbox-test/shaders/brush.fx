cbuffer Global : register(b0)
{
	float Time;
	float4x4 View;
	float4x4 Projection;
}

cbuffer Uniforms : register(b2)
{
	float4 Brush;
	float4 Opacity;
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

Texture2D Mask : register(t0);
Texture2D Diffuse : register(t1);
Texture2D Normal : register(t2);
Texture2D Specular : register(t3);

SamplerState Sampler;

struct PSOut
{
	float4 colour : SV_Target0;
	float4 normals : SV_Target1;
	float4 speculars : SV_Target2;
};

PSOut PS(VOut input)
{
	PSOut output;
	float2 brush_coords;

	brush_coords.x = (input.texcoord.x - Brush.x) / (Brush.z - Brush.x);
	brush_coords.y = (input.texcoord.y - Brush.y) / (Brush.w - Brush.y);

	float4 mask = Mask.Sample(Sampler, brush_coords);
	float a = (mask.r + mask.g + mask.b) / 3.0f;

	a *= Opacity.r;

	output.colour = Diffuse.Sample(Sampler, input.texcoord);
	output.colour.a *= a;

	output.normals = Normal.Sample(Sampler, input.texcoord);
	output.normals.a *= a;

	output.speculars = Specular.Sample(Sampler, input.texcoord);
	output.speculars.a *= a;

	return output;
}