Texture2D heightmap   : register(t0);
Texture2D fluxmap     : register(t1);
Texture2D velocitymap : register(t2);
SamplerState sampler0 : register(s0);

cbuffer BufferType : register(b0) {
	float2 cellSize;
	float gravity;
	float pipeArea;
	float pipeLen;
	float3 padding;
}

struct InputType {
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

#include "erosion_common.hlsl"

float4 main(InputType input) : SV_TARGET {
	float2 texelSize;
	heightmap.GetDimensions(texelSize.x, texelSize.y);
	texelSize = 1. / texelSize;
	
	float4 flux = fluxmap.Sample(sampler0, input.tex);
	float4 height      = heightmap.Sample(sampler0, input.tex);
	float4 heightLeft  = heightmap.Sample(sampler0, input.tex + float2(-1,  0) * texelSize);
	float4 heightRight = heightmap.Sample(sampler0, input.tex + float2( 1,  0) * texelSize);
	float4 heightUp    = heightmap.Sample(sampler0, input.tex + float2( 0,  1) * texelSize);
	float4 heightDown  = heightmap.Sample(sampler0, input.tex + float2( 0, -1) * texelSize);
	
	float4 heightDiff = FULL_HEIGHT(height) - float4(
		FULL_HEIGHT(heightLeft),
		FULL_HEIGHT(heightRight),
		FULL_HEIGHT(heightUp),
		FULL_HEIGHT(heightDown) 
	);
	
	flux = max(0, flux + dt * gravity * pipeArea * heightDiff / pipeLen);
	flux *= min(1, WATER(height) * cellSize.x * cellSize.y / (COMPONENT_SUM(flux) * dt));
	
	if(input.tex.x <= texelSize.x)        LEFT(flux)  = 0;
	if(input.tex.x >= (1. - texelSize.x)) RIGHT(flux) = 0;
	if(input.tex.y <= texelSize.y)        UP(flux)    = 0;
	if(input.tex.y >= (1. - texelSize.y)) DOWN(flux)  = 0;
	
	return max(0, flux);
}