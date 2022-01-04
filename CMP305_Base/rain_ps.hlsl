Texture2D heightmap : register(t0);
SamplerState sampler0 : register(s0);

#include "erosion_common.hlsl"
#include "noise.hlsl"

cbuffer BufferType : register(b0) {
	float2 cellSize;
	float rainProbability;
	float rainAmount;
	float2 seed;
	float2 padding;
};

struct InputType {
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET {
	float4 height = HEIGHT_MAP(input.tex);
	
	float r = random(input.tex + seed);
	if (r < rainProbability) {
		WATER(height) += rainAmount * dt;
	}
	
	return height;
}