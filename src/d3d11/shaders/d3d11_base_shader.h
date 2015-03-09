#pragma once

#define D3D11_BASE_SHADER "\
#define MAX_LIGHTS 64\n\
#define LIGHT_POINT 0\n\
#define LIGHT_DIRECTIONAL 1\n\
#define LIGHT_SPOT 2\n\
\n\
cbuffer Global : register(b0)\n\
{\n\
\tfloat Time;\n\
\tfloat4x4 View;\n\
\tfloat4x4 Projection;\n\
\tfloat4 EyePosition;\n\
}\n\
\n\
struct Attributes\n\
{\n\
\tfloat4 Emissive;\n\
\tfloat4 Diffuse;\n\
\tfloat4 Ambient;\n\
\tfloat4 Specular;\n\
\tfloat SpecularIntensity;\n\
\tfloat Reflectivity;\n\
\tfloat NormalScale;\n\
};\n\
\n\
struct Light\n\
{\n\
\tfloat4 Translation;\n\
\tfloat4 Direction;\n\
\tfloat4 Colour;\n\
\n\
\tfloat SpotAngle;\n\
\tfloat ConstantAttenuation;\n\
\tfloat LinearAttenuation;\n\
\tfloat QuadraticAttenuation;\n\
\n\
\tint Type;\n\
\tbool Activated;\n\
};\n\
\n\
cbuffer PerObject : register(b1)\n\
{\n\
\tfloat4x4 World;\n\
\tfloat4x4 InvWorld;\n\
\tfloat4 AnimationCoords;\n\
\tfloat3 Blend;\n\
\tfloat Alpha;\n\
\n\
\tAttributes Material;\n\
}\n\
\n\
cbuffer Lighting : register(b2)\n\
{\n\
\tfloat4 AmbientColour;\n\
\tint NumLights;\n\
\tLight Lights[MAX_LIGHTS];\n\
}\n\
\n\
cbuffer Uniforms : register(b3)\n\
{\n\
\n\
}\n\
\n\
struct VOut\n\
{\n\
\tfloat4 position : SV_POSITION;\n\
\tfloat4 colour : COLOUR;\n\
\tfloat2 texcoord : TEXCOORD0;\n\
\tfloat3 normal : TEXCOORD2;\n\
\tfloat4 world_pos : TEXCOORD1;\n\
\tfloat4 view : TEXCOORD3;\n\
};\n\
\n\
VOut VS(float4 position : POSITION, float4 colour : COLOUR, float2 texcoord : TEXCOORD0, float3 normal : NORMAL)\n\
{\n\
\tVOut output;\n\
\toutput.position = mul(position, World);\n\
\toutput.position = mul(output.position, View);\n\
\toutput.position = mul(output.position, Projection);\n\
\toutput.world_pos = mul(position, World);\n\
\toutput.view = EyePosition - output.world_pos;\n\
\toutput.normal = mul(normal, (float3x3)InvWorld);\n\
\toutput.texcoord = texcoord;\n\
\toutput.colour = colour;\n\
\treturn output;\n\
}\n\
\n\
TextureCube TexCube : register(t0);\n\
Texture2D TexDiffuse : register(t1);\n\
Texture2D TexNormal : register(t2);\n\
Texture2D TexSpecular : register(t3);\n\
Texture2D TexLight : register(t4);\n\
\n\
SamplerState Sampler;\n\
\n\
struct LightResult\n\
{\n\
\tfloat4 Diffuse;\n\
\tfloat4 Specular;\n\
};\n\
\n\
float4 Diffuse(Light light, float3 l, float3 n)\n\
{\n\
\tfloat d = saturate(dot(n, l));\n\
\treturn light.Colour * d;\n\
}\n\
\n\
float4 Specular(Light light, float3 l, float3 v, float3 n)\n\
{\n\
\tfloat3 r = normalize(2.0f * n * dot(n, l) - l);\n\
\tfloat d = max(dot(r, v), 0.0f);\n\
\n\
\treturn light.Colour * pow(d, Material.SpecularIntensity);\n\
}\n\
\n\
float Attenuation(Light light, float d)\n\
{\n\
\treturn 1.0f / (light.ConstantAttenuation + light.LinearAttenuation * d + light.QuadraticAttenuation * d * d);\n\
}\n\
\n\
LightResult Directional(Light light, float3 v, float3 n)\n\
{\n\
\tLightResult result;\n\
\tfloat3 l = normalize(-light.Direction.xyz);\n\
\n\
\tresult.Diffuse = Diffuse(light, l, n);\n\
\tresult.Specular = Specular(light, l, v, n);\n\
\n\
\treturn result;\n\
}\n\
\n\
float SpotCone(Light light, float3 l)\n\
{\n\
\tfloat min_cos = cos(light.SpotAngle);\n\
\tfloat max_cos = (min_cos + 1.0f) / 2.0f;\n\
\tfloat cos_angle = dot(light.Direction.xyz, -l);\n\
\treturn smoothstep(min_cos, max_cos, cos_angle);\n\
}\n\
\n\
LightResult SpotLight(Light light, float3 v, float4 p, float3 n)\n\
{\n\
\tLightResult result;\n\
\n\
\tfloat3 l = (light.Translation - p).xyz;\n\
\tfloat distance = length(l);\n\
\n\
\tl /= distance;\n\
\n\
\tfloat attenuation = Attenuation(light, distance);\n\
\tfloat spot_intensity = SpotCone(light, l);\n\
\n\
\tresult.Diffuse = Diffuse(light, l, n) * attenuation * spot_intensity;\n\
\tresult.Specular = Specular(light, v, l, n) * attenuation * spot_intensity;\n\
\n\
\treturn result;\n\
}\n\
\n\
LightResult PointLight(Light light, float3 v, float4 p, float3 n)\n\
{\n\
\tLightResult result;\n\
\tfloat3 l = (light.Translation - p).xyz;\n\
\tfloat distance = length(l);\n\
\tl /= distance;\n\
\n\
\tfloat attenuation = Attenuation(light, distance);\n\
\n\
\tresult.Diffuse = Diffuse(light, l, n) * attenuation;\n\
\tresult.Specular = Specular(light, l, v, n) * attenuation;\n\
\n\
\treturn result;\n\
}\n\
\n\
LightResult ComputeLighting(float3 view, float4 p, float3 normal)\n\
{\n\
\tLightResult total = {\n\
\t\t{ 0.0, 0.0, 0.0, 0.0 },\n\
\t\t{ 0.0, 0.0, 0.0, 0.0 }\n\
\t};\n\
\n\
\tLightResult result;\n\
\tfor (int i = 0; i < NumLights; ++i)\n\
\t{\n\
\t\tswitch (Lights[i].Type)\n\
\t\t{\n\
\t\t\tcase LIGHT_DIRECTIONAL:\n\
\t\t\tresult = Directional(Lights[i], view, normal);\n\
\t\t\ttotal.Diffuse += result.Diffuse;\n\
\t\t\ttotal.Specular += result.Specular;\n\
\t\t\tbreak;\n\
\n\
\t\t\tcase LIGHT_POINT:\n\
\t\t\tresult = PointLight(Lights[i], view, p, normal);\n\
\t\t\ttotal.Diffuse += result.Diffuse;\n\
\t\t\ttotal.Specular += result.Specular;\n\
\t\t\tbreak;\n\
\n\
\t\t\tcase LIGHT_SPOT:\n\
\t\t\tresult = SpotLight(Lights[i], view, p, normal);\n\
\t\t\ttotal.Diffuse += result.Diffuse;\n\
\t\t\ttotal.Specular += result.Specular;\n\
\t\t\tbreak;\n\
\t\t}\n\
\t}\n\
\n\
\ttotal.Diffuse = saturate(total.Diffuse);\n\
\ttotal.Specular = saturate(total.Specular);\n\
\n\
\treturn total;\n\
}\n\
\n\
float4 Reflection(float3 view, float3 normal)\n\
{\n\
\tfloat3 r = normalize(reflect(view, normal));\n\
\treturn TexCube.Sample(Sampler, r);\n\
}\n\
\n\
float4 PS(VOut input) : SV_TARGET\n\
{\n\
\tfloat3 view = normalize(input.view.xyz);\n\
\tfloat3 normal = normalize(input.normal);\n\
\tfloat4 normal_map = TexNormal.Sample(Sampler, input.texcoord);\n\
\tnormal_map = (normal_map * 2.0f - 1.0f) * Material.NormalScale;\n\
\n\
\tLightResult result = ComputeLighting(view, input.world_pos, normal);\n\
\n\
\tfloat4 emissive = Material.Emissive * TexLight.Sample(Sampler, input.texcoord);\n\
\tfloat4 ambient = Material.Ambient * AmbientColour;\n\
\tfloat4 diffuse = result.Diffuse * Material.Diffuse;\n\
\tfloat4 specular = result.Specular * Material.Specular * TexSpecular.Sample(Sampler, input.texcoord);\n\
\tfloat4 diffuse_map = TexDiffuse.Sample(Sampler, input.texcoord);\n\
\n\
\tfloat4 r = Reflection(view, normal - normal_map.xyz);\n\
\tdiffuse_map = lerp(diffuse_map, r, Material.Reflectivity);\n\
\n\
\tfloat4 colour = (saturate(ambient + diffuse) * diffuse_map + specular) + emissive;\n\
\n\
\tfloat alpha = Material.Diffuse.a * diffuse_map.a;\n\
\tcolour.a *= alpha;\n\
\tcolour *= input.colour;\n\
\n\
\treturn colour;\n\
}"