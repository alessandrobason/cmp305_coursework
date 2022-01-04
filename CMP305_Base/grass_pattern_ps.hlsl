cbuffer GrassBuffer : register(b0) {
	float2 resolution;
	float2 offset;
	float scale;
	float exponent;
	float threshold;
	float padding;
};

struct InputType {
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

//#define USE_RIDGED_NOISE
//#define USE_POWERED_NOISE
#define OCTAVES 8
#define AMPLITUDE_CHANGE 0.5
#define COORD_CHANGE 2.8
#include "noise.hlsl"

float4 main(InputType input) : SV_TARGET {
	float2 st = input.position.xy / resolution;
	st.x *= resolution.x / resolution.y;
	
	st = st * scale + offset;
	
	float w = 1 - fbmWorley(st);
	float p = fbmPerlin(st);
	
	return pow(w * p, exponent) < threshold;
}