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
	
	float4 flux      = FLUX_MAP(input.tex);
	float4 fluxLeft  = FLUX_MAP(input.tex + float2(-1,  0) * texelSize);
	float4 fluxRight = FLUX_MAP(input.tex + float2( 1,  0) * texelSize);
	float4 fluxUp    = FLUX_MAP(input.tex + float2( 0,  1) * texelSize);
	float4 fluxDown  = FLUX_MAP(input.tex + float2( 0, -1) * texelSize);
	
	float2 velocity = float2(
		RIGHT(fluxLeft) - LEFT(flux) + RIGHT(flux) - LEFT(fluxRight),
		   UP(fluxDown) - DOWN(flux) +    UP(flux) - DOWN(fluxUp)
	);
	
	return float4(velocity/2., 0.0, 1.0);
}