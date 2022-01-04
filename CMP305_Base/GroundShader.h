#pragma once

#include "PixelOnlyShader.h"

class TextureGenShader : public PixelOnlyShader {
	struct BufferType {
		float2 resolution;
		float scale;
		float exponent;
		float4 lowColour;
		float4 highColour;
	};

public:
	TextureGenShader(Device *device, HWND hwnd, const wchar_t *shader);
	~TextureGenShader();

	void setShaderParameters(
		DeviceContext *ctx,
		const mat4 &world, const mat4 &view, const mat4 &proj,
		float2 resolution, float scale, float exponent,
		const float4 &lowColour, 
		const float4 &highColour
	);

private:
	Buffer *buffer = nullptr;
};

class GroundShader : public DefaultShader {
	struct BufferType {
		float maxHeight;
		float3 padding;
		mat4 lightMVP;
	};

	struct GroundBufferType {
		float slopeThreshold;
		float snowThreshold;
		float2 padding;
	};
public:
	GroundShader(Device *device, HWND hwnd);
	~GroundShader();

	void setShaderParameters(
		DeviceContext *ctx,
		const mat4 &world, const mat4 &view, const mat4 &proj,
		TextureType *heightmap,
		TextureType *normalmap,
		TextureType *groundmap,
		TextureType *grassmap,
		TextureType *shadowmap,
		Light &light,
		float maxHeight,
		float slopeThreshold,
		float snowThreshold
	);

private:
	Buffer *buf = nullptr;
	Buffer *groundBuf = nullptr;
};