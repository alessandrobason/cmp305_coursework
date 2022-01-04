Texture2D heightmap   : register(t0);
Texture2D fluxmap     : register(t1);
Texture2D velocitymap : register(t2);
SamplerState sampler0 : register(s0);

cbuffer BufferType : register(b0) {
	float2 cellSize;
	float2 padding;
};

struct InputType {
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

#include "erosion_common.hlsl"

float4 main(InputType input) : SV_TARGET {
	float2 texelSize;
	heightmap.GetDimensions(texelSize.x, texelSize.y);
	texelSize = 1. / texelSize;
	
	float4 height   = HEIGHT_MAP(input.tex);
	float2 velocity = VELOCITY_MAP(input.tex);
	
	float2 uv = input.tex - velocity * dt;
	// if the coordinate is outside the texture, do interpolation
	// on neighbours
	if (uv.x < 0 || uv.x > 1 || uv.y < 0 || uv.y > 1) {
		// bilinear interpolation from
		// https://en.wikipedia.org/wiki/Bilinear_interpolation#On_the_unit_square
		
		// sample bilinear
		float2 f = floor(uv);
		float2 c = ceil(uv);
		float2 i = uv - f;
		
		// cordinates in a square around point
		float2 uv00 = float2(f);
		float2 uv01 = float2(f.x, c.y);
		float2 uv10 = float2(c.x, f.y);
		float2 uv11 = float2(c.x, c.y);
		
		// weights
		float w00 = (1 - i.x) * (1 - i.y);
		float w01 = (1 - i.x) * i.y;
		float w10 = i.x * (1 - i.y);
		float w11 = i.x * i.y;
		
		// sediment values
		float s00 = SEDIMENT(HEIGHT_MAP(uv00)) * w00;
		float s01 = SEDIMENT(HEIGHT_MAP(uv01)) * w01;
		float s10 = SEDIMENT(HEIGHT_MAP(uv10)) * w10;
		float s11 = SEDIMENT(HEIGHT_MAP(uv11)) * w11;
		
		SEDIMENT(height) = s00 + s01 + s10 + s11;
	}
	else {
		SEDIMENT(height) = SEDIMENT(HEIGHT_MAP(uv));
	}
	
	return height;
}