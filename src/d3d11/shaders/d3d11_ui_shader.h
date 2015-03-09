#pragma once

#define D3D11_UI_SHADER "\
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
cbuffer PerObject : register(b1)\n\
{\n\
\tfloat4x4 World;\n\
\tfloat4x4 InvWorld;\n\
\tfloat4 AnimationCoords;\n\
\tfloat3 Blend;\n\
\tfloat Alpha;\n\
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
\tfloat shift : TEXCOORD3;\n\
};\n\
\n\
VOut VS(float4 position : POSITION, float4 colour : COLOUR, float2 texcoord : TEXCOORD0, float3 normal : NORMAL)\n\
{\n\
\tVOut output;\n\
\toutput.shift = position.x - floor(position.x);\n\
\toutput.position = mul(position, World);\n\
\toutput.position = mul(output.position, Projection);\n\
\toutput.normal = mul(normal, (float3x3)InvWorld);\n\
\toutput.texcoord = texcoord;\n\
\toutput.colour = colour;\n\
\treturn output;\n\
}\n\
\n\
Texture2D TexDiffuse : register(t1);\n\
SamplerState Sampler;\n\
\n\
float4 PS(VOut input) : SV_TARGET\n\
{\n\
\tfloat4 diffuse = TexDiffuse.Sample(Sampler, input.texcoord);\n\
\tdiffuse.rgb *= input.colour.rgb * Blend;\n\
\tdiffuse.a *= Alpha;\n\
\treturn diffuse;\n\
}"