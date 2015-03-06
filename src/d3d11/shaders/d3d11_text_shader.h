#pragma once

#define D3D11_TEXT_SHADER "\
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
\n\
SamplerState Sampler;\n\
\n\
float4 PS(VOut input) : SV_TARGET\n\
{\n\
\tfloat shift = input.shift;\n\
\tfloat3 font_atlas_vector = float3(1.0 / 2048, 1.0 / 2048, 4);\n\
\n\
\tfloat2 coords = input.texcoord;\n\
\n\
\tfloat4 current = TexDiffuse.Sample(Sampler, coords);\n\
\tfloat4 previous = TexDiffuse.Sample(Sampler, coords + float2(-1, 0) * font_atlas_vector.xy);\n\
\tfloat4 next = TexDiffuse.Sample(Sampler, coords + float2(1, 0) * font_atlas_vector.xy);\n\
\n\
\tfloat r = current.r;\n\
\tfloat g = current.g;\n\
\tfloat b = current.b;\n\
\n\
\tif (shift <= 0.333)\n\
\t{\n\
\t\tfloat z = shift / 0.333;\n\
\t\tr = lerp(current.r, previous.b, z);\n\
\t\tg = lerp(current.g, current.r, z);\n\
\t\tb = lerp(current.b, current.g, z);\n\
\t}\n\
\telse if (shift <= 0.666)\n\
\t{\n\
\t\tfloat z = (shift - 0.33) / 0.333;\n\
\t\tr = lerp(previous.b, previous.g, z);\n\
\t\tg = lerp(current.r, previous.b, z);\n\
\t\tb = lerp(current.g, current.r, z);\n\
\t}\n\
\telse if (shift < 1.0)\n\
\t{\n\
\t\tfloat z = (shift - 0.66) / 0.334;\n\
\t\tr = lerp(previous.g, previous.r, z);\n\
\t\tg = lerp(previous.b, previous.g, z);\n\
\t\tb = lerp(current.r, previous.b, z);\n\
\t}\n\
\n\
\tfloat t = max(max(r, g), b);\n\
\tfloat4 colour = float4(1, 1, 1, (r + g + b) / 3.0);\n\
\n\
\tfloat alpha = colour.a * input.colour.a * Alpha;\n\
\treturn float4(colour.rgb * input.colour.rgb * Blend, alpha);\n\
}"