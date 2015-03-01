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
	float3 normal : NORMAL;
	float4 worldPos : POSITION;
};

VOut VS(float4 position : POSITION, float4 colour : COLOUR, float2 texcoord : TEXCOORD0, float3 normal : NORMAL)
{
	VOut output;
	output.position = mul(position, World);
	output.worldPos = mul(position, World);
	output.position = mul(output.position, View);
	output.position = mul(output.position, Projection);
	output.normal = normalize(mul(float4(normal, 0), InvWorld).xyz);
	output.texcoord = texcoord;
	output.colour = colour;
	return output;
}

float4 Diffuse(Light light, float3 p, float3 normal)
{
    float d = max(0, dot(normal, p));
    return light.Colour * d;
}

float4 Specular(Light light, float3 view, float3 p, float3 normal)
{
    float3 reflected = normalize(reflect(-p, normal));
    float d = max(0, dot(reflected, view));

    return light.Colour * pow(d, Material.SpecularIntensity);
}

float Attenuation(Light light, float d)
{
    return 1.0f / (light.ConstantAttenuation + light.LinearAttenuation * d + light.QuadraticAttenuation * d * d);
}

struct LightingResult
{
	float4 Diffuse;
	float4 Specular;
};

LightingResult PointLight(Light light, float3 view, float4 p, float3 normal)
{
	LightingResult result;
 
    float3 l = (light.Translation - p).xyz;
    float d = length(l);
    l = l / d;
 
    float attenuation = Attenuation(light, d);
 
    result.Diffuse = Diffuse(light, l, normal) * attenuation;
    result.Specular = Specular(light, view, l, normal) * attenuation;
 
    return result;
}

LightingResult DirectionalLight(Light light, float3 view, float4 p, float3 normal)
{
    LightingResult result;
 
    float3 l = light.Direction.xyz;
 
    result.Diffuse = Diffuse(light, l, normal);
    result.Specular = Specular(light, view, l, normal);
 
    return result;
}

float SpotCone(Light light, float3 l)
{
    float min_cos = cos(light.SpotAngle);
    float max_cos = (min_cos + 1.0f) / 2.0f;
    float cos_angle = dot(light.Direction.xyz, -l);
    return smoothstep(min_cos, max_cos, cos_angle); 
}

LightingResult SpotLight(Light light, float3 view, float4 p, float3 normal)
{
    LightingResult result;
 
    float3 l = (light.Translation - p).xyz;
    float d = length(l);
    l = l / d;
 
    float attenuation = Attenuation(light, d);
    float spot_intensity = SpotCone(light, l);
 
    result.Diffuse = Diffuse(light, l, normal) * attenuation * spot_intensity;
    result.Specular = Specular(light, view, l, normal) * attenuation * spot_intensity;
 
    return result;
}

LightingResult ComputeLighting(float4 p, float4 eye, float3 normal)
{
    float3 v = normalize(eye - p).xyz;
 
    LightingResult total = {{0, 0, 0, 0}, {0, 0, 0, 0}};
 
    for(int i = 0; i < NumLights; ++i)
    {
        LightingResult result = {{0, 0, 0, 0}, {0, 0, 0, 0}};

        switch(Lights[i].Type)
        {
        case LIGHT_DIRECTIONAL:
            {
                result = DirectionalLight(Lights[i], v, p, normal);
            }
            break;
        case LIGHT_POINT: 
            {
                result = PointLight(Lights[i], v, p, normal);
            }
            break;
        case LIGHT_SPOT:
            {
                result = SpotLight(Lights[i], v, p, normal);
            }
            break;
        }
        
        total.Diffuse += result.Diffuse;
        total.Specular += result.Specular;
    }
 
    total.Diffuse = saturate(total.Diffuse);
    total.Specular = saturate(total.Specular);
 
    return total;
}

Texture2D Tex2D[3];
SamplerState Sampler;

float4 PS(VOut input) : SV_TARGET
{
	LightingResult lit = ComputeLighting(input.worldPos, EyePosition, input.normal);
     
    float4 emissive = Material.Emissive;
    float4 ambient = Material.Ambient * AmbientColour;
    float4 diffuse = Material.Diffuse * lit.Diffuse;
    float4 specular = Material.Specular * lit.Specular;
 
    float4 base = Tex2D[0].Sample(Sampler, input.texcoord);
 
    float4 final = (emissive + ambient + diffuse + specular) * base;
 
    return final;
}