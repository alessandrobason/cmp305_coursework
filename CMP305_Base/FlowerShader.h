#pragma once

#include "PixelOnlyShader.h"

class FlowerGenShader : public PixelOnlyShader {
	struct BufferType {
		float2 resolution;
		float scale;
		float padding;
		float4 seed;
	};

public:
	FlowerGenShader(Device *device, HWND hwnd);
	~FlowerGenShader();

	void setShaderParameters(
		DeviceContext *ctx,
		const mat4 &world, const mat4 &view, const mat4 &proj,
		float2 resolution, float scale, const float4 &seed
	);

private:
	Buffer *buffer = nullptr;
};

class FlowerShader : public DefaultShader {
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
	FlowerShader(Device *device, HWND hwnd);
	~FlowerShader();

	void setShaderParameters(
		DeviceContext *ctx,
		const mat4 &world,
		const mat4 &view,
		const mat4 &proj,
		TextureType *heightmap,
		TextureType *normalmap,
		TextureType *flowermap,
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