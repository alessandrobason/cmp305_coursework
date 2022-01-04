#include "GroundShader.h"

#include "utility.h"

GroundShader::GroundShader(Device *device, HWND hwnd) 
	: DefaultShader(device, hwnd) {
	initShader(L"ground_vs.cso", L"ground_ps.cso");
	addDynamicBuffer<BufferType>(&buf);
	addDynamicBuffer<GroundBufferType>(&groundBuf);
}

GroundShader::~GroundShader() {
	SAFE_RELEASE(groundBuf);
}

void GroundShader::setShaderParameters(
	DeviceContext *ctx, 
	const mat4 &world, 
	const mat4 &view, 
	const mat4 &proj, 
	TextureType *heightmap, 
	TextureType *normalmap,
	TextureType *groundmap,
	TextureType *grassmap,
	TextureType *shadowmap,
	Light &light,
	float maxHeight,
	float slopeThreshold,
	float snowThreshold
) {
	DefaultShader::setShaderParameters(ctx, world, view, proj, heightmap, light);

	mat4 lightView = light.getViewMatrix();
	mat4 lightProj = light.getOrthoMatrix();

	auto buffer = MappedBuffer<BufferType>(ctx, buf, VS, 1);
	buffer->maxHeight = maxHeight;
	buffer->lightMVP = XMMatrixTranspose(lightView * lightProj);

	auto ground = MappedBuffer<GroundBufferType>(ctx, groundBuf, PS, 1);
	ground->slopeThreshold   = slopeThreshold;
	ground->snowThreshold    = snowThreshold;

	if (!isDepth) {
		ctx->PSSetShaderResources(1, 1, &normalmap);
		ctx->PSSetShaderResources(2, 1, &groundmap);
		ctx->PSSetShaderResources(3, 1, &grassmap);
		ctx->PSSetShaderResources(4, 1, &shadowmap);
	}

	ctx->VSSetShaderResources(0, 1, &heightmap);
	ctx->VSSetSamplers(0, 1, &sampleState);
}

TextureGenShader::TextureGenShader(Device *device, HWND hwnd, const wchar_t *shader)
	: PixelOnlyShader(device, hwnd) {
	initShader(shader);
	addDynamicBuffer<BufferType>(&buffer);
}

TextureGenShader::~TextureGenShader() {
	SAFE_RELEASE(buffer);
}

void TextureGenShader::setShaderParameters(
	DeviceContext *ctx, 
	const mat4 &world, 
	const mat4 &view, 
	const mat4 &proj, 
	float2 resolution, 
	float scale, 
	float exponent, 
	const float4 &lowColour, 
	const float4 &highColour
) {
	PixelOnlyShader::setShaderParameters(ctx, world, view, proj);

	auto buf = MappedBuffer<BufferType>(ctx, buffer, PS, 0);
	buf->resolution = resolution;
	buf->scale = scale;
	buf->exponent = exponent;
	buf->lowColour = lowColour;
	buf->highColour = highColour;
}
