
cbuffer BufferType : register(b0) {
	float2 resolution;
	float exponent;
	float scale;
	float2 offset;
	float2 padding;
};

struct InputType {
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

//#define USE_RIDGED_NOISE
//#define USE_POWERED_NOISE
#define AMPLITUDE_CHANGE 0.5
#define COORD_CHANGE 1.8

#include "noise.hlsl"

float4 main(InputType input) : SV_TARGET {
	//return float4(0, 0, 0, 1);
	
	float2 st = input.position.xy / resolution;
	st.x *= resolution.x / resolution.y;
	
	float height = pow(fbmPerlin(st * scale  + offset), exponent);

	return float4(height, 0, 0, 1);
}