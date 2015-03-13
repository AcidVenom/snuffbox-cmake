cbuffer Global : register(b0)
{
	float Time;
	float4x4 View;
	float4x4 Projection;
	float4 EyePosition;
	float4x4 InvViewProjection;
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
Texture2D TexDepth : register(t3);
SamplerState Sampler;

float4 Specular(float3 v, float3 l, float3 n, float i)
{
	float3 h = normalize(-l + v);
    float d = max(0, dot(n, h));

    return pow(d, 1);
}

float4 PS(VOut input) : SV_TARGET
{
	float4 diffuse = TexColour.Sample(Sampler, input.texcoord);
	float4 normal = TexNormal.Sample(Sampler, input.texcoord);
	float4 depth = TexDepth.Sample(Sampler, input.texcoord);

	float4 position;
	position.x = (1 - input.texcoord.x) * 2.0f - 1.0f;
	position.y = (1 - input.texcoord.y) * 2.0f - 1.0f;
	position.z = depth.r;
	position.w = 1.0f;

	position = mul(position, InvViewProjection);

	position.xyz /= position.w;

	float3 light_dir = float3(0.0f, -1.0f, -1.0f);

	float3 view = normalize(EyePosition.xyz - position.xyz);

	float specular_intensity = normal.a;
	normal = normalize(normal * 2.0f - 1.0f);

	float4 specular = Specular(view, light_dir, normal.rgb, specular_intensity);
	
	diffuse.rgb *= saturate(dot(-light_dir, normal.rgb));
	diffuse.rgb += specular.rgb;

	if (depth.r == 1)
	{
		return 0;
	}
	return specular;
}