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
	
	float4 height  = HEIGHT_MAP(input.tex);
	float4 fluxOut = FLUX_MAP(input.tex);
	
	float4 fluxIn = float4(
		RIGHT(FLUX_MAP(input.tex + float2(-1,  0) * texelSize)),
		 LEFT(FLUX_MAP(input.tex + float2( 1,  0) * texelSize)),
		 DOWN(FLUX_MAP(input.tex + float2( 0,  1) * texelSize)),
		   UP(FLUX_MAP(input.tex + float2( 0, -1) * texelSize))
	);
	
	float waterDelta = COMPONENT_SUM(fluxIn) - COMPONENT_SUM(fluxOut);
	WATER(height) += dt * waterDelta / (cellSize.x * cellSize.y);
	return height;
}