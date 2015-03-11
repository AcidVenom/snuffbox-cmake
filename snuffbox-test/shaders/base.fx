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

struct Light
{
	float4 Translation;
	float4 Direction;
	float4 Colour;

	float SpotAngle;
	float ConstantAttenuation;
	float LinearAttenuation;
	float QuadraticAttenuation;

	int Type;
	bool Activated;
};

cbuffer PerObject : register(b1)
{
	float4x4 World;
	float4x4 InvWorld;
	float4 AnimationCoords;
	float3 Blend;
	float Alpha;

	Attributes Material;
}

cbuffer Lighting : register(b2)
{
	float4 AmbientColour;
	int NumLights;
	Light Lights[MAX_LIGHTS];
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
	float4 world_pos : TEXCOORD1;
	float4 view : TEXCOORD3;
};

VOut VS(float4 position : POSITION, float4 colour : COLOUR, float2 texcoord : TEXCOORD0, float3 normal : NORMAL)
{
	VOut output;
	output.position = mul(position, World);
	output.position = mul(output.position, View);
	output.position = mul(output.position, Projection);
	output.world_pos = mul(position, World);
	output.view = EyePosition - output.world_pos;
	output.normal = mul(normal, (float3x3)InvWorld);
	output.texcoord = texcoord;
	output.colour = colour;
	return output;
}

TextureCube TexCube : register(t0);
Texture2D TexDiffuse : register(t1);
Texture2D TexNormal : register(t2);
Texture2D TexSpecular : register(t3);
Texture2D TexLight : register(t4);

SamplerState Sampler;

struct LightResult
{
	float4 Diffuse;
	float4 Specular;
};

float4 Diffuse(Light light, float3 l, float3 n)
{
	float d = saturate(dot(n, l));
	return light.Colour * d;
}

float4 Specular(Light light, float3 l, float3 v, float3 n)
{
	float3 r = normalize(2.0f * n * dot(n, l) - l);
	float d = max(dot(r, v), 0.0f);

	return light.Colour * pow(d, Material.SpecularIntensity);
}

float Attenuation(Light light, float d)
{
	return 1.0f / (light.ConstantAttenuation + light.LinearAttenuation * d + light.QuadraticAttenuation * d * d);
}

LightResult Directional(Light light, float3 v, float3 n)
{
	LightResult result;
	float3 l = normalize(-light.Direction.xyz);

	result.Diffuse = Diffuse(light, l, n);
	result.Specular = Specular(light, l, v, n);

	return result;
}

float SpotCone(Light light, float3 l)
{
	float min_cos = cos(light.SpotAngle);
	float max_cos = (min_cos + 1.0f) / 2.0f;
	float cos_angle = dot(light.Direction.xyz, -l);
	return smoothstep(min_cos, max_cos, cos_angle);
}

LightResult SpotLight(Light light, float3 v, float4 p, float3 n)
{
	LightResult result;

	float3 l = (light.Translation - p).xyz;
	float distance = length(l);

	l /= distance;

	float attenuation = Attenuation(light, distance);
	float spot_intensity = SpotCone(light, l);

	result.Diffuse = Diffuse(light, l, n) * attenuation * spot_intensity;
	result.Specular = Specular(light, v, l, n) * attenuation * spot_intensity;

	return result;
}

LightResult PointLight(Light light, float3 v, float4 p, float3 n)
{
	LightResult result;
	float3 l = (light.Translation - p).xyz;
	float distance = length(l);
	l /= distance;

	float attenuation = Attenuation(light, distance);

	result.Diffuse = Diffuse(light, l, n) * attenuation;
	result.Specular = Specular(light, l, v, n) * attenuation;

	return result;
}

LightResult ComputeLighting(float3 view, float4 p, float3 normal)
{
	LightResult total = {
		{ 0.0, 0.0, 0.0, 0.0 },
		{ 0.0, 0.0, 0.0, 0.0 }
	};

	LightResult result;
	for (int i = 0; i < NumLights; ++i)
	{
		switch (Lights[i].Type)
		{
			case LIGHT_DIRECTIONAL:
			result = Directional(Lights[i], view, normal);
			total.Diffuse += result.Diffuse;
			total.Specular += result.Specular;
			break;

			case LIGHT_POINT:
			result = PointLight(Lights[i], view, p, normal);
			total.Diffuse += result.Diffuse;
			total.Specular += result.Specular;
			break;

			case LIGHT_SPOT:
			result = SpotLight(Lights[i], view, p, normal);
			total.Diffuse += result.Diffuse;
			total.Specular += result.Specular;
			break;
		}
	}

	total.Diffuse = saturate(total.Diffuse);
	total.Specular = saturate(total.Specular);

	return total;
}

float4 Reflection(float3 view, float3 normal)
{
	float3 r = normalize(reflect(view, normal));
	return TexCube.Sample(Sampler, r);
}

float4 PS(VOut input) : SV_TARGET
{
	float x = (input.texcoord.x * AnimationCoords.z) + AnimationCoords.x;
	float y = (input.texcoord.y * AnimationCoords.w) + AnimationCoords.y;
	float2 coords = float2(x, y);
	float3 view = normalize(input.view.xyz);
	float3 normal = normalize(input.normal);
	float4 normal_map = TexNormal.Sample(Sampler, coords);
	normal_map = (normal_map * 2.0f - 1.0f) * Material.NormalScale;

	LightResult result = ComputeLighting(view, input.world_pos, normal);

	float4 emissive = Material.Emissive * TexLight.Sample(Sampler, coords);
	float4 ambient = Material.Ambient * AmbientColour;
	float4 diffuse = result.Diffuse * Material.Diffuse;
	float4 specular = result.Specular * Material.Specular * TexSpecular.Sample(Sampler, coords);
	float4 diffuse_map = TexDiffuse.Sample(Sampler, coords);

	float4 r = Reflection(view, normal - normal_map.xyz);
	diffuse_map = lerp(diffuse_map, r, Material.Reflectivity);

	float4 colour = (saturate(ambient + diffuse) * diffuse_map + specular) + emissive;

	float alpha = Material.Diffuse.a * diffuse_map.a;
	colour.a *= alpha;
	colour *= input.colour;

	return colour;
}