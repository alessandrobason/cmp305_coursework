#include "HeightShader.h"

#include "utility.h"

HeightShader::HeightShader(Device *device, HWND hwnd) 
	: PixelOnlyShader(device, hwnd) {
	initShader(L"height_ps.cso");

	addDynamicBuffer<BufferType>(&buffer);
}

HeightShader::~HeightShader() {
	SAFE_RELEASE(buffer);
}

void HeightShader::setShaderParameters(
	DeviceContext *ctx, 
	const mat4 &world, 
	const mat4 &view, 
	const mat4 &proj, 
	float2 resolution, 
	float exponent,
	float scale, 
	float2 offset
) {
	PixelOnlyShader::setShaderParameters(ctx, world, view, proj);

	auto buf = MappedBuffer<BufferType>(ctx, buffer, PS, 0);
	buf->resolution = resolution;
	buf->exponent = exponent;
	buf->scale = scale;
	buf->offset = offset;
}

NormalShader::NormalShader(Device *device, HWND hwnd) 
	: PixelOnlyShader(device, hwnd) {
	initShader(L"normal_ps.cso");

	addDynamicBuffer<BufferType>(&buffer);
	addDiffuseSampler();
}

NormalShader::~NormalShader() {
	SAFE_RELEASE(buffer);
}

void NormalShader::setShaderParameters(
	DeviceContext *ctx, 
	const mat4 &world, 
	const mat4 &view, 
	const mat4 &proj, 
	TextureType *heightmap, 
	float maxHeight
) {
	PixelOnlyShader::setShaderParameters(ctx, world, view, proj);
	
	auto buf = MappedBuffer<BufferType>(ctx, buffer, PS, 0);
	buf->maxHeight = maxHeight;

	ctx->PSSetSamplers(0, 1, &sampleState);
	ctx->PSSetShaderResources(0, 1, &heightmap);
}