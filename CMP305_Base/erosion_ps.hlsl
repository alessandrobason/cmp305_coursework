Texture2D heightmap   : register(t0);
Texture2D fluxmap     : register(t1);
Texture2D velocitymap : register(t2);
Texture2D normalmap   : register(t3);
SamplerState sampler0 : register(s0);

cbuffer BufferType : register(b0) {
	float2 cellSize;
	float sedimentCapacity;
	float dissolvingConst;
	float depositionConst;
	float evaporationConst;
	float2 padding;
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
	
	float4 height = HEIGHT_MAP(input.tex);
	
	float4 heightLeft  = HEIGHT_MAP(input.tex + float2(-1,  0) * texelSize);
	float4 heightRight = HEIGHT_MAP(input.tex + float2( 1,  0) * texelSize);
	float4 heightUp    = HEIGHT_MAP(input.tex + float2( 0,  1) * texelSize);
	float4 heightDown  = HEIGHT_MAP(input.tex + float2( 0, -1) * texelSize);
	float2 velocity    = VELOCITY_MAP(input.tex);
	
	// Tilt angle
	float3 normal = normalmap.Sample(sampler0, input.tex).rgb;
	
	float sinTiltAngle = abs(normal.y);
	
	// sediment transport capacity
	float C = sedimentCapacity * sinTiltAngle * length(velocity);
	
	if (C > SEDIMENT(height)) {
		float sedimentDiff = dt * dissolvingConst * (C - SEDIMENT(height));
		TERRAIN(height)  -= sedimentDiff;
		SEDIMENT(height) += sedimentDiff;
	}
	else {
		float sedimentDiff = dt * depositionConst * (SEDIMENT(height) - C);
		TERRAIN(height)  += sedimentDiff;
		SEDIMENT(height) -= sedimentDiff;
	}
	
	WATER(height) *= 1. - evaporationConst * dt;
	
	return height;
}