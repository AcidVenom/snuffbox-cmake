#define LIGHT_POINT 0
#define LIGHT_DIRECTIONAL 1
#define LIGHT_SPOT 2

cbuffer Global : register(b0)
{
	float Time;
	float4x4 View;
	float4x4 Projection;
	float4 EyePosition;
	float4x4 InvViewProjection;
}

cbuffer Uniforms : register(b2)
{

}

struct LightAttributes
{
	float4 Translation;
	float4 Direction;
	float4 Colour;

	float SpotAngle;
	float Radius;

	int Type;
};

cbuffer LightBuffer : register(b3)
{
	LightAttributes Light;
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

struct LightResult
{
	float4 Diffuse;
	float4 Specular;
};

float4 Diffuse(float3 l, float3 n)
{
    float d = saturate(dot(normalize(l), n));
    return Light.Colour * d;
}

float4 Specular(float3 v, float3 l, float3 n, float i, float p)
{
	float3 r = normalize(reflect(normalize(l), normalize(n)));
    float d = saturate(dot(r, v));

    return i * pow(d, p) * Light.Colour;
}

LightResult Directional(float3 v, float3 n, float i, float p)
{
	LightResult result;
	float3 l = Light.Direction.xyz;

	result.Diffuse = Diffuse(-l, n);
	result.Specular = Specular(v, l, n, i, p);

	return result;
}

LightResult Point(float3 v, float3 pos, float3 n, float i, float p)
{
	LightResult result;
	float r = Light.Radius;

	float3 light_pos = Light.Translation.xyz;
	light_pos.y *= -1;

    float3 l = light_pos - pos;
    float dist = length(l);
    float d = max(dist - r, 0);
    l /= dist;
 
    float dd = d / r + 1;
    float attenuation = 1 / (dd * dd);

    attenuation = max(attenuation, 0);
 
    result.Diffuse = Diffuse(-l, n) * attenuation;
    result.Specular = Specular(v, -l, n, i, p) * attenuation;
 
    return result;
}

LightResult ComputeLighting(float3 v, float3 pos, float3 n, float i, float p)
{
	LightResult result = {{0,0,0,0}, {0,0,0,0}};
	switch(Light.Type)
	{
		case LIGHT_DIRECTIONAL:
			result = Directional(v, n, i, p);
		break;

		case LIGHT_POINT:
			result = Point(v, pos, n, i, p);
		break;
	}

	return result;
}

float4 PS(VOut input) : SV_TARGET
{
	float4 diffuse = TexColour.Sample(Sampler, input.texcoord);
	float4 normal = TexNormal.Sample(Sampler, input.texcoord);
	float4 depth = TexDepth.Sample(Sampler, input.texcoord);

	float4 position;
	position.x = input.texcoord.x * 2.0f - 1.0f;
	position.y = (1 - input.texcoord.y) * 2.0f - 1.0f;
	position.z = depth.r;
	position.w = 1.0f;

	position = mul(position, InvViewProjection);

	position.xyz /= position.w;
	position.w = 1.0f;

	float3 light_dir = Light.Direction.xyz;

	float3 view = normalize(EyePosition.xyz - position.xyz);

	float specular_intensity = normal.a;
	float specular_power = diffuse.a * 256;

	normal = normal * 2.0f - 1.0f;

	LightResult result = ComputeLighting(view, position.xyz, normal.rgb, specular_intensity, specular_power);
	
	float4 final = diffuse;
	final += result.Specular;
	final *= result.Diffuse;
	final.a = result.Specular.a;
	return final;
}