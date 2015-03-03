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
	int2 Padding;
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

cbuffer Lighting : register(b2)
{
	float4 AmbientColour;
	int NumLights;
	Light Lights[64];
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
    output.normal = mul(normal, (float3x3) InvWorld);
	output.texcoord = texcoord;
	output.colour = colour;
	return output;
}

TextureCube TexCube : register(t0);
Texture2D TexDiffuse : register(t1);
Texture2D TexNormal : register(t2);

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

LightResult Directional(Light light, float3 v, float3 n)
{
    LightResult result;
    float3 l = normalize(-light.Direction.xyz);

    result.Diffuse = Diffuse(light, l, n);
    result.Specular = Specular(light, l, v, n);

    return result;
}

LightResult ComputeLighting(float3 view, float3 normal)
{
    LightResult total = {
        {0.0, 0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0, 0.0}
    };

    LightResult result;
    for (int i = 0; i < NumLights; ++i)
    {
        switch(Lights[i].Type)
        {
            case LIGHT_DIRECTIONAL:
            result = Directional(Lights[i], view, normal);
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
    float3 r = normalize(reflect(-view, normal));
    return TexCube.Sample(Sampler, r);
}

float4 PS(VOut input) : SV_TARGET
{
    float3 view = normalize(input.view.xyz);
    float3 normal = normalize(input.normal);
    float4 normal_map = TexNormal.Sample(Sampler, input.texcoord);
    normal_map = (normal_map * 2.0f - 1.0f) * Material.NormalScale;

    LightResult resA = ComputeLighting(view, normal);
    LightResult resB = ComputeLighting(view, normal_map.xyz);
    
    float4 emissive = Material.Emissive;
    float4 ambient = Material.Ambient * AmbientColour;
    float4 diffuse = normalize(resA.Diffuse + resB.Diffuse) * Material.Diffuse;
    float4 specular = (resA.Specular + resB.Specular) * Material.Specular;
    float4 diffuse_map = TexDiffuse.Sample(Sampler, input.texcoord);
    
    float4 r = Reflection(view, normal - normal_map.xyz);
    diffuse_map = lerp(diffuse_map, r, Material.Reflectivity);

    float4 colour = (saturate(ambient + diffuse) * diffuse_map + specular) + emissive;

    float alpha = Material.Diffuse.a * diffuse_map.a;

    return colour;
}