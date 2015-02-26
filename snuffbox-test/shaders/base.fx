cbuffer Global : register(b0)
{
	float Time;
	float4x4 View;
	float4x4 Projection;
	float3 ViewVector;
}

cbuffer PerObject : register(b1)
{
	float4x4 World;
	float4x4 InvWorld;
	float Alpha;
	float3 Blend;
	float4 AnimationCoords;
}

struct VOut
{
	float4 position : SV_POSITION;
	float4 colour : COLOUR;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL;
	float3 worldPos : POSITION;
};

VOut VS(float4 position : POSITION, float4 colour : COLOUR, float2 texcoord : TEXCOORD0, float3 normal : NORMAL)
{
	VOut output;
	position.z += 0.3;
	output.position = mul(position, World);
	output.worldPos = output.position.xyz;
	output.position = mul(output.position, View);
	output.position = mul(output.position, Projection);
	output.normal = normalize(mul(float4(normal, 0), InvWorld).xyz);
	output.texcoord = texcoord;
	output.colour = colour;
	return output;
}

Texture2D Tex2D[3];
SamplerState Sampler;

float4 PS(VOut input) : SV_TARGET
{
	float3 viewVec = normalize(float3(View[3][0], View[3][1], View[3][2]) - input.worldPos.xyz);
 	float4 base = Tex2D[0].Sample(Sampler, input.texcoord);
 	float3 normMap = Tex2D[1].Sample(Sampler, input.texcoord).rgb;

 	normMap = normMap * 2 - 1;

 	float3 light = normalize(float3(0, sin(Time) * 2, 1));
    float3 normal = normalize(input.normal * normMap);
    float3 r = normalize(2 * dot(-light, normal) * normal - light);
    float3 v = normalize(mul(normalize(float4(viewVec, 0)), World)).xyz;

    float dotProduct = dot(r, v);
    float4 specular = 1 * float4(1,0.5,0.5,1) * max(pow(abs(dotProduct), 30), 0) * length(base);

    v = saturate(dot(light, normMap));
    float4 normColour = float4(v, 1);

    return float4(input.colour * base + specular) * normColour;
}