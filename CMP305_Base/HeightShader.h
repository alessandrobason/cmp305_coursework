#pragma once

#include "PixelOnlyShader.h"

class HeightShader : public PixelOnlyShader {
	struct BufferType {
		float2 resolution;
		float exponent;
		float scale;
		float2 offset;
		float2 padding = float2(0, 0);
	};

public:
	HeightShader(Device *device, HWND hwnd);
	~HeightShader();

	void setShaderParameters(
		DeviceContext *ctx,
		const mat4 &world, const mat4 &view, const mat4 &proj,
		float2 resolution, float exponent, float scale, float2 offset
	);

private:
	Buffer *buffer = nullptr;
};

class NormalShader : public PixelOnlyShader {
	struct BufferType {
		float maxHeight;
		float3 padding;
	};

public:
	NormalShader(Device *device, HWND hwnd);
	~NormalShader();

	void setShaderParameters(
		DeviceContext *ctx,
		const mat4 &world, const mat4 &view, const mat4 &proj,
		TextureType *heightmap, float maxHeight
	);

private:
	Buffer *buffer = nullptr;
};