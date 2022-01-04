#include "CopyShader.h"

CopyShader::CopyShader(Device *device, HWND hwnd)
	: PixelOnlyShader(device, hwnd) {
	initShader(L"copy_ps.cso");
	addDiffuseSampler();
}

void CopyShader::setShaderParameters(
	DeviceContext *ctx, 
	const mat4 &world, 
	const mat4 &view, 
	const mat4 &proj, 
	TextureType *input
) {
	PixelOnlyShader::setShaderParameters(ctx, world, view, proj);
	ctx->PSSetShaderResources(0, 1, &input);
	ctx->PSSetSamplers(0, 1, &sampleState);
}
