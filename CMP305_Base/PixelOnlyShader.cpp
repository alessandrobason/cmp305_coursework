#include "PixelOnlyShader.h"

PixelOnlyShader::PixelOnlyShader(Device *device, HWND hwnd)
	: DefaultShader(device, hwnd) {

}

void PixelOnlyShader::setShaderParameters(
	DeviceContext *ctx, 
	const mat4 &world, 
	const mat4 &view, 
	const mat4 &proj
) {
	// == MATRIX BUFFER =========================
	// Transpose the matrices to prepare them for the shader.
	mat4 tworld = XMMatrixTranspose(world);
	mat4 tview = XMMatrixTranspose(view);
	mat4 tproj = XMMatrixTranspose(proj);

	auto matrixPtr = MappedBuffer<MatrixBufferType>(ctx, matrixBuffer, VS, 0);
	matrixPtr->world = tworld;
	matrixPtr->view = tview;
	matrixPtr->projection = tproj;
}

void PixelOnlyShader::initShader(const wchar_t *ps) {
	loadVertexShader(L"texture_vs.cso");
	loadPixelShader(ps);

	addDynamicBuffer<MatrixBufferType>(&matrixBuffer);
	//addDiffuseSampler();
}
