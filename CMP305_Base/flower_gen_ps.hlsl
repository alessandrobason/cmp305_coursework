
cbuffer BufferType : register(b0) {
	float2 resolution;
	float scale;
	float padding;
	float4 seed;
};

struct InputType {
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

#include "noise.hlsl"

float4 main(InputType input) : SV_TARGET {
	float2 st = input.position.xy / resolution;
	st.x *= resolution.x / resolution.y;
	
	float n = worley(st * scale + seed.r);
	
	float4 groundCol = float4(1, 0, 0, 1);
	float4 grassCol  = float4(0, 1, 0, 1);
	
	float4 col = groundCol * (1 - n) + (grassCol * n * 0.5);
	// col.r = fbm(st * scale + seed.r);
	// col.g = fbm(st * scale + seed.g);
	// col.b = fbm(st * scale + seed.b);
	// col.a = fbm(st * scale + seed.a);
	
	return col;
}