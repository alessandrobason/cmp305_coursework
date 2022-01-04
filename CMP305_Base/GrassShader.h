#pragma once

#include "PixelOnlyShader.h"

class GrassPatternShader : public PixelOnlyShader {
	struct GrassBuffer {
		float2 resolution;
		float2 offset;
		float scale;
		float exponent;
		float threshold;
		float padding;
	};

public:
	GrassPatternShader(Device *device, HWND hwnd);
	~GrassPatternShader();

	void setData(
		DeviceContext *ctx, 
		float2 resolution, float2 offset, 
		float scale, float exponent, float threshold
	);
	
private:
	Buffer *grassBuf = nullptr;
};

class GrassCompute : public DefaultShader {
public:
	GrassCompute(Device *device, HWND hwnd);
};

class GrassShader : public DefaultShader {
	struct GrassBuffer {
		float3 cameraPos;
		float slopeThreshold;
	};

	struct HullBuffer {
		float3 cameraPos;
		float cutoffDistance;
		float maxDist;
		float maxFactor;
		float2 padding;
	};

public:
	GrassShader(Device *device, HWND hwnd);
	~GrassShader();

	void setShaderParameters(
		DeviceContext *ctx, 
		const mat4 &world,
		const mat4 &view, 
		const mat4 &proj, 
		TextureType *heightmap,
		TextureType *normalmap,
		TextureType *texture,
		Light &light,
		float3 cameraPos,
		float slopeThreshold,
		float cutoffDistance,
		float maxDist,
		float maxFactor
	);

	void render(DeviceContext *ctx, MMesh &mesh);

private:
	Buffer *grassBuf = nullptr;
	Buffer *hullBuf = nullptr;
};