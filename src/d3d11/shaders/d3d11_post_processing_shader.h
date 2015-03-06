#pragma once

#define D3D11_POST_PROCESSING_SHADER "\
cbuffer Global : register(b0)\n\
{\n\
\tfloat Time;\n\
\tfloat4x4 View;\n\
\tfloat4x4 Projection;\n\
}\n\
\n\
struct VOut\n\
{\n\
\tfloat4 position : SV_POSITION;\n\
\tfloat4 colour : COLOUR;\n\
\tfloat2 texcoord : TEXCOORD0;\n\
\tfloat3 normal : NORMAL;\n\
};\n\
\n\
VOut VS(float4 position : POSITION, float4 colour : COLOUR, float2 texcoord : TEXCOORD0, float3 normal : NORMAL)\n\
{\n\
\tVOut output;\n\
\toutput.position = position;\n\
\toutput.normal = normal;\n\
\toutput.texcoord = texcoord;\n\
\toutput.colour = colour;\n\
\treturn output;\n\
}\n\
\n\
Texture2D Tex2D;\n\
SamplerState Sampler;\n\
\n\
float4 PS(VOut input) : SV_TARGET\n\
{\n\
\treturn Tex2D.Sample(Sampler, input.texcoord);\n\
}"