// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)

Texture2D heightmap : register(t0);
Texture2D normalmap : register(t1);
Texture2D rockmap   : register(t2);
Texture2D grassmap  : register(t3);
Texture2D shadowmap : register(t4);

SamplerState sampler0 : register(s0);

cbuffer LightBuffer : register(b0) {
	float4 diffuseColour;
	float3 lightDirection;
	float p0;
};

cbuffer GroundBufferType : register(b1) {
	float slopeThreshold;
	float snowThreshold;
	float2 p1;
}

struct InputType {
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 lightViewPos : SPOT_LIGHT_POS;
};

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
	float intensity = saturate(dot(normal, lightDirection));
	float4 colour = saturate(diffuse * intensity);
	return colour;
}

float getHeight(float2 uv) {
	float2 height = heightmap.SampleLevel(sampler0, uv, 0).rg;
	return height.r + height.g;
}

float2 getProjectiveCoords(float4 lightViewPosition) {
	// Calculate the projected texture coordinates.
	float2 projTex = lightViewPosition.xy / lightViewPosition.w;
	projTex *= float2(0.5, -0.5);
	projTex += float2(0.5f, 0.5f);
	return projTex;
}

bool hasDepthData(float2 uv) {
	return uv.x >= 0.f && uv.x <= 1.f && uv.y >= 0.f && uv.y <= 1.f;
}

#define SHADOW_USE_BLUR
#define SHADOW_BLUR_SAMPLE 5
#define SHADOW_BLUR_SIZE ((SHADOW_BLUR_SAMPLE * 2 + 1) * (SHADOW_BLUR_SAMPLE * 2 + 1))

float getShadow(float4 lightViewPos, float bias) {
	float lightDepthValue = (lightViewPos.z / lightViewPos.w) - bias;
	float2 uv = getProjectiveCoords(lightViewPos);
	
#ifndef SHADOW_USE_BLUR
	if (!hasDepthData(uv)) {
		return 0.;
	}

	float depthValue = shadowmap.Sample(sampler0, uv).r;
	return (lightDepthValue >= depthValue);
#else
	float2 texelSize;
	shadowmap.GetDimensions(texelSize.x, texelSize.y);
	texelSize = 1. / texelSize;
	
	float shadow = 0.;
	
	for (int y = -SHADOW_BLUR_SAMPLE; y <= SHADOW_BLUR_SAMPLE; ++y) {
		for (int x = -SHADOW_BLUR_SAMPLE; x <= SHADOW_BLUR_SAMPLE; ++x) {
			float2 offset = float2(x, y) * texelSize;
			float depthValue = shadowmap.Sample(sampler0, uv + offset).r;
			shadow += (lightDepthValue >= depthValue);
		}
	}
	
	return shadow / SHADOW_BLUR_SIZE;
	
#endif
}

#define OCTAVES 8
#define AMPLITUDE_CHANGE 0.5
#define COORD_CHANGE 2.8
#include "noise.hlsl"

float4 main(InputType input) : SV_TARGET {
	float4 normMapValue = normalmap.Sample(sampler0, input.tex);
	
	// we put the slope value in the normal map in the alpha value, this way
	// we can pre-calculate it just once
	float3 norm = normMapValue.rgb;
	float slope = normMapValue.a;
	
	float4 colour;
	
	float thresholdHigh = 1 - (slopeThreshold / 2);
	float thresholdLow = thresholdHigh - slopeThreshold;
	
	float4 groundCol = rockmap.Sample(sampler0, input.tex);
	float4 grassCol = grassmap.Sample(sampler0, input.tex);
	
	if (slope > thresholdLow) {
		slope = thresholdHigh - slope;
		
		//float height = getHeight(input.tex) * maxHeight;
		//if (height > snowThreshold) {
		//	grassCol = lerp(grassCol, 1, (height - snowThreshold) / (snowThreshold));
		//}
		
		colour = lerp(grassCol, groundCol, slope / slopeThreshold);
	}
	else {
		colour = groundCol;
	}
	
	float4 heightMap = heightmap.Sample(sampler0, input.tex);
	float waterHeight = heightMap.g;
	if (waterHeight > 0) {
		float4 waterColour = float4(0, 0, 1, 1);
		colour = lerp(colour, waterColour, waterHeight * 30);
	}
	//colour = lerp(colour, waterColour, waterHeight * 10);
	
	//colour = heightmap.Sample(sampler0, input.tex);
	//float b = colour.b;
	//colour.b = colour.g;
	//colour.g = b;
	
	//float waterHeight = heightmap.Sample(sampler0, input.tex).g;
	//if (waterHeight > 0) {
	//	colour = heightmap.Sample(sampler0, input.tex);
	//	colour.b = 1 * waterHeight * 5;
	//}
	
	//float4 waterCol = float4(0, 0, 1, 1);
	//colour = lerp(colour, waterCol, waterHeight * 5.);
	
	//float threshold2 = (1 - slopeThreshold) * 2;
	//if (slope > (1 - threshold2)) {
	//	slope = 1 - slope;
	//	colour = lerp(grassColour, groundColour, slope / threshold2);
	//}
	//else {
	//	colour = groundColour;
	//}
	
	float4 lightColour = calculateLighting(-lightDirection, norm, diffuseColour);
	float shadow = getShadow(input.lightViewPos, 0.005);
	return (colour * lightColour) - shadow;
}



