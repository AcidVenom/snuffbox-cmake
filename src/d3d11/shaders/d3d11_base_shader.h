#pragma once

#define D3D11_BASE_SHADER "\
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
cbuffer Uniforms : register(b2)\n\
{\n\
\n\
}\n\
\n\
struct VOut\n\
{\n\
\tfloat4 position : SV_POSITION;\n\
\tfloat4 colour : COLOUR;\n\
\tfloat2 texcoord : TEXCOORD0;\n\
\tfloat3 normal : TEXCOORD1;\n\
\tfloat3 tangent : TEXCOORD2;\n\
\tfloat3 bitangent : TEXCOORD3;\n\
\tfloat4 world_pos : TEXCOORD4;\n\
};\n\
\n\
VOut VS(float4 position : POSITION, float4 colour : COLOUR, float2 texcoord : TEXCOORD0, float3 normal : NORMAL, float3 tangent : TANGENT, float3 bitangent : BITANGENT)\n\
{\n\
\tVOut output;\n\
\toutput.world_pos = position;\n\
\toutput.position = mul(position, World);\n\
\toutput.position = mul(output.position, View);\n\
\toutput.position = mul(output.position, Projection);\n\
\toutput.normal = normalize(mul(normal, (float3x3)InvWorld));\n\
\toutput.tangent = normalize(mul(tangent, (float3x3)InvWorld));\n\
\toutput.bitangent = normalize(mul(bitangent, (float3x3)InvWorld));\n\
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
struct PSOut\n\
{\n\
\tfloat4 colour : SV_Target0;\n\
\tfloat4 normal : SV_Target1;\n\
};\n\
float4 Reflection(float4 p, float4 eye, float3 normal)\n\
{\n\
\tnormal.y *= -1;\n\
\tfloat3 i = normalize(p.xyz - eye.xyz);\n\
\tfloat3 r = reflect(i, normal.xyz);\n\
\treturn TexCube.Sample(Sampler, r);\n\
}\n\
\n\
PSOut PS(VOut input)\n\
{\n\
\tPSOut output;\n\
\tfloat x = (input.texcoord.x * AnimationCoords.z) + AnimationCoords.x;\n\
\tfloat y = (input.texcoord.y * AnimationCoords.w) + AnimationCoords.y;\n\
\tfloat2 coords = float2(x, y);\n\
\n\
\tfloat4 normal = normalize(TexNormal.Sample(Sampler, coords) * 2.0f - 1.0f);\n\
\tnormal.rgb = lerp(normal.rgb, float3(0, 0, 1), 1.0f - Material.NormalScale);\n\
\n\
\tnormal = float4((normal.x * input.tangent) + (normal.y * input.bitangent) + (normal.z * input.normal), 1.0f);\n\
\n\
\tfloat4 r = Reflection(input.world_pos, EyePosition, normal.rgb);\n\
\n\
\toutput.colour = lerp(TexDiffuse.Sample(Sampler, coords), r, Material.Reflectivity) * Material.Diffuse;\n\
\toutput.normal = float4((normal.rgb + 1.0f) / 2.0f, 1.0f);\n\
\n\
\treturn output;\n\
}"