
cbuffer BufferType : register(b0) {
	float2 resolution;
	float scale;
	float exponent;
	float4 rockLowColour;
	float4 rockHighColour;
};

struct InputType {
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

//#define USE_RIDGED_NOISE
#define USE_POWERED_NOISE
#define OCTAVES 8
#define AMPLITUDE_CHANGE 0.5
#define COORD_CHANGE 2.8
#include "noise.hlsl"

float4 main(InputType input) : SV_TARGET {
	float2 st = input.position.xy / resolution;
	st.x *= resolution.x / resolution.y;
	
	st *= scale;
	
	float w = 1 - fbmWorley(st);
	float p = fbmPerlin(st);
	
	float n = pow(w * p, exponent);
	return rockLowColour * n + rockHighColour * (1 - n);
}